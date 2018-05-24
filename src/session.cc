/*
 * Copyright (c) 2018 Elastos Foundation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include "elca.h"

namespace elca {

    void session_request_callback(ElaCarrier *w, const char *from,
                const char *sdp, size_t len, void *context);
    void session_request_complete_callback(ElaSession *ws, int status,
                    const char *reason, const char *sdp, size_t len, void *context);
    static void createSessionSelfFunctions(napi_env env, napi_value session, Elca *elca);
    void createStreamFunctions(napi_env env, napi_value carrier, Elca *elca);

    static napi_value elca_session_init(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_value context = nullptr;

        int ret;

        size_t argc = 2;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("initSession", 1, value_false);
        CHECK_ELACARRIER_PTR(value_false);

        if (argc > 1) context = args[1];

        setCallbackHandle(env, args[0], context, SESSION_REQUEST, elca);

        ret = ela_session_init(elca->elacarrier, session_request_callback, elca);
        if (ret == -1) {
            log_err(env, "Run ela_session_init error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    static napi_value elca_session_cleanup(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        CHECK_ELACARRIER_PTR(nullptr);

        ela_session_cleanup(elca->elacarrier);
        return nullptr;
    }

    static napi_value elca_session_new(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        Elca* session_elca = nullptr;
        ElaSession * session = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char address[ELA_MAX_ADDRESS_LEN + 1];
        napi_value obj = value_null, carrier;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("newSession", 1, value_null);
        CHECK_ELACARRIER_PTR(value_null);

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], address, ELA_MAX_ADDRESS_LEN + 1, &len);
        if (status != napi_ok) return value_null;

        //Alloc one new Elca
        HEAP_ALLOC(session_elca, Elca, 1, value_null);
        session_elca->type = SESSION;

        session = ela_session_new(elca->elacarrier, address);
        if (!session) {
            log_err(env, "Run ela_session_new error:0x%X", ela_get_error());
            free(session_elca);
            return value_null;
        }

        status = napi_create_object(env, &obj);
        if (status != napi_ok || !obj) {
            return value_null;
        }

        session_elca->elasession = session;
        session_elca->env = env;
        status = napi_create_reference(env, obj, 1, &session_elca->object);
        CHECK_STATUS;

        // Set the propertie
        status = napi_get_reference_value(env, elca->object, &carrier);
        CHECK_STATUS;

        napi_property_descriptor descriptors[] = {
            { "carrier", NULL, 0, 0, 0, carrier, napi_default, 0 },
        };

        status = napi_define_properties(env, obj,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;

        createSessionSelfFunctions(env, obj, session_elca);
        createStreamFunctions(env, obj, session_elca);

        return obj;
    }

    static napi_value elca_session_close(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        CHECK_ELASESSION_PTR(nullptr);

        ela_session_close(elca->elasession);
        for (int i = 0; i < CALLBACK_COUNT; i++) {
                deleteCallbackHandle(env, i, elca);
        }
        napi_delete_reference(env, elca->object);
        free(elca);

        return nullptr;
    }

    static napi_value elca_session_get_peer(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        char address[2 * ELA_MAX_ID_LEN + 1];
        char *ret;
        napi_value peer = nullptr;
        napi_status status;

        status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        CHECK_STATUS;
        CHECK_ELASESSION_PTR(value_null);

        ret = ela_session_get_peer(elca->elasession, address, 2 * ELA_MAX_ID_LEN + 1);
        if (!ret) return value_null;

        napi_create_string_utf8(env, ret, NAPI_AUTO_LENGTH, &peer);

        return peer;
    }


    static napi_value elca_session_request(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_value context = nullptr;

        int ret;

        size_t argc = 2;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("session.request", 1, value_null);
        CHECK_ELASESSION_PTR(value_null);

        if (argc > 1) context = args[1];

        setCallbackHandle(env, args[0], context, SESSION_REQUEST_COMPLETE, elca);

        ret = ela_session_request(elca->elasession, session_request_complete_callback, elca);
        if (ret == -1) {
            log_err(env, "Run ela_session_request error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    static napi_value elca_session_reply_request(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char reason[ELA_MAX_APP_MESSAGE_LEN + 1], *ptr = NULL;
        int ret = -1;
        uint32_t reply_status;

        size_t argc = 2, len;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("replyRequest", 2, value_false);
        CHECK_ELASESSION_PTR(value_false);

        //get status
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_false;
        status = napi_get_value_uint32(env, args[0], &reply_status);
        if (status != napi_ok) return value_false;

        //get reason
        status = napi_typeof(env, args[1], &valuetype);
        if (valuetype == napi_string) {
            status = napi_get_value_string_utf8(env, args[1], reason, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
            if (status != napi_ok) return value_false;
            ptr = reason;
        }

        ret = ela_session_reply_request(elca->elasession, reply_status, ptr);
        if (ret == -1) {
            log_err(env, "Run ela_session_reply_request error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    static napi_value elca_session_start(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char sdp[ELA_MAX_APP_MESSAGE_LEN + 1];
        int ret = -1;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("session.start", 1, value_false);
        CHECK_ELASESSION_PTR(value_false);

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], sdp, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_session_start(elca->elasession, sdp, len);
        if (ret == -1) {
            log_err(env, "Run ela_session_start error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

static napi_value elca_session_add_service(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char service[ELA_MAX_APP_MESSAGE_LEN + 1];
        char host[ELA_MAX_APP_MESSAGE_LEN + 1];
        char port[MAX_PORT_LEN];
        int ret = -1;
        uint32_t protocol;

        size_t argc = 4, len;
        napi_value args[4];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("addService", 4, value_false);
        CHECK_ELASESSION_PTR(value_false);

        //get service
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], service, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        //get protocol
        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_false;
        status = napi_get_value_uint32(env, args[1], &protocol);
        if (status != napi_ok) return value_false;

        //get host
        status = napi_typeof(env, args[2], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[2], host, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        //get port
        status = napi_typeof(env, args[3], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[3], port, MAX_PORT_LEN, &len);
        if (status != napi_ok) return value_false;

        ret = ela_session_add_service(elca->elasession, service, (PortForwardingProtocol)protocol, host, port);
        if (ret == -1) {
            log_err(env, "Run ela_session_add_service error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    static napi_value elca_session_remove_service(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char service[ELA_MAX_APP_MESSAGE_LEN + 1];

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("removeService", 1, nullptr);
        CHECK_ELASESSION_PTR(nullptr);

        //get service
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return nullptr;
        status = napi_get_value_string_utf8(env, args[0], service, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return nullptr;

        ela_session_remove_service(elca->elasession, service);
        return nullptr;
    }

    void createSessionFunctions(napi_env env, napi_value carrier, Elca *elca) {
        napi_status status;
        napi_value fn[3];
        memset(fn, 0, sizeof(napi_value) * 3);

        napi_create_function(env, nullptr, 0, elca_session_init, (void*)elca, &fn[0]);
        napi_create_function(env, nullptr, 0, elca_session_cleanup, (void*)elca, &fn[1]);
        napi_create_function(env, nullptr, 0, elca_session_new, (void*)elca, &fn[2]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "initSession", NULL, 0, 0, 0, fn[0], napi_default, 0 },
            { "cleanupSession", NULL, 0, 0, 0, fn[1], napi_default, 0 },
            { "newSession", NULL, 0, 0, 0, fn[2], napi_default, 0 },
        };

        status = napi_define_properties(env, carrier,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }

    static void createSessionSelfFunctions(napi_env env, napi_value session, Elca *elca) {
        napi_status status;
        napi_value fn[7];
        memset(fn, 0, sizeof(napi_value) * 7);

        napi_create_function(env, nullptr, 0, elca_session_close, (void*)elca, &fn[0]);
        napi_create_function(env, nullptr, 0, elca_session_get_peer, (void*)elca, &fn[1]);
        napi_create_function(env, nullptr, 0, elca_session_request, (void*)elca, &fn[2]);
        napi_create_function(env, nullptr, 0, elca_session_reply_request, (void*)elca, &fn[3]);
        napi_create_function(env, nullptr, 0, elca_session_start, (void*)elca, &fn[4]);
        napi_create_function(env, nullptr, 0, elca_session_add_service, (void*)elca, &fn[5]);
        napi_create_function(env, nullptr, 0, elca_session_remove_service, (void*)elca, &fn[6]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "close", NULL, 0, 0, 0, fn[0], napi_default, 0 },
            { "getPeer", NULL, 0, 0, 0, fn[1], napi_default, 0 },
            { "request", NULL, 0, 0, 0, fn[2], napi_default, 0 },
            { "replyRequest", NULL, 0, 0, 0, fn[3], napi_default, 0 },
            { "start", NULL, 0, 0, 0, fn[4], napi_default, 0 },
            { "addService", NULL, 0, 0, 0, fn[5], napi_default, 0 },
            { "removeService", NULL, 0, 0, 0, fn[6], napi_default, 0 },
        };

        status = napi_define_properties(env, session,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }
} // namespace elca
