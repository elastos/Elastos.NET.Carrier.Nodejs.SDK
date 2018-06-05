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

    static void createStreamSelfFunctions(napi_env env, napi_value stream, Elca *elca);
    void createChannelFunctions(napi_env env, napi_value stream, Elca *elca);

    static napi_value elca_session_add_stream(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        Elca* stream_elca = nullptr;
        napi_value session = nullptr, context = nullptr;
        napi_value obj = value_null, id;
        napi_status status;
        napi_valuetype valuetype;
        int ret = -1;
        uint32_t options;
        uint32_t type;
        ElaStreamCallbacks callbacks;

        size_t argc = 4;
        napi_value args[4];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("addStream", 3, value_null);
        CHECK_ELASESSION_PTR(value_null);

        if (argc > 3) context = args[3];

        //get type
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_null;
        status = napi_get_value_uint32(env, args[0], &type);
        if (status != napi_ok) return value_null;

        //get options
        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_false;
        status = napi_get_value_uint32(env, args[1], &options);
        if (status != napi_ok) return value_null;

        HEAP_ALLOC(stream_elca, Elca, 1, value_null);
        stream_elca->type = STREAM;
        stream_elca->stream = -1;

        setCallbackFuntions(env, args[2], context, stream_elca);
        setStreamNativeCallbacks(&callbacks);

        ret = ela_session_add_stream(elca->elasession, (ElaStreamType)type, options, &callbacks, stream_elca);
        if (ret == -1) {
            log_err(env, "Run elca_session_add_stream error:0x%X", ela_get_error());
            free(stream_elca);
            return value_null;
        }

        status = napi_create_object(env, &obj);
        if (status != napi_ok || !obj) {
            return value_null;
        }

        stream_elca->elasession = elca->elasession;
        stream_elca->stream = ret;
        stream_elca->env = env;
        status = napi_create_reference(env, obj, 1, &stream_elca->object);
        CHECK_STATUS;

        // Set the propertie
        status = napi_get_reference_value(env, elca->object, &session);
        CHECK_STATUS;

        status = napi_create_int32(env, ret, &id);
        CHECK_STATUS;

        napi_property_descriptor descriptors[] = {
            { "session", NULL, 0, 0, 0, session, napi_default, 0 },
            { "id", NULL, 0, 0, 0, id, napi_default, 0 },
        };

        status = napi_define_properties(env, obj,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;

        createStreamSelfFunctions(env, obj, stream_elca);
        createChannelFunctions(env, obj, stream_elca);

        return obj;
    }

    static napi_value elca_session_remove_stream(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        int ret;
        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        CHECK_STREAM_VALUE(nullptr);

        ret = ela_session_remove_stream(elca->elasession, elca->stream);
        if (ret != -1) {
            for (int i = 0; i < STREAM_CALLBACK_COUNT; i++) {
                deleteCallbackHandle(env, i, elca);
            }
            napi_delete_reference(env, elca->object);
            free(elca);

            return value_true;
        }
        else return value_false;
    }

    static napi_value elca_stream_get_type(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        ElaStreamType type;
        int ret;
        napi_value result = value_null;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        CHECK_STREAM_VALUE(value_null);

        ret = ela_stream_get_type(elca->elasession, elca->stream, &type);
        if (ret == -1) return value_null;
        napi_create_uint32(env, type, &result);
        return result;
    }

    static napi_value elca_stream_get_state(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        ElaStreamState state;
        int ret;
        napi_value result = value_null;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        CHECK_STREAM_VALUE(value_null);

        ret = ela_stream_get_state(elca->elasession, elca->stream, &state);
        if (ret == -1) return value_null;
        napi_create_uint32(env, state, &result);
        return result;
    }

    static napi_value elca_stream_get_transport_info(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        ElaTransportInfo transport_info;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        CHECK_STREAM_VALUE(value_null);

        if (!ela_stream_get_transport_info(elca->elasession, elca->stream, &transport_info)) {
            return createUserInfoFromJsObj(env, &transport_info);
        }

        return value_null;
    }

    static napi_value elca_stream_write(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        void *data;
        size_t len;
        int ret;
        napi_value result;
        napi_status status;

        size_t argc = 1;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("stream.write", 1, value_n1);
        CHECK_STREAM_VALUE(value_n1);

        status = napi_get_buffer_info(env, args[0], &data, &len);
        CHECK_STATUS;

        ret = ela_stream_write(elca->elasession, elca->stream, data, len);
        if (ret == -1) {
            log_err(env, "Run ela_stream_write error:0x%X", ela_get_error());
            return value_n1;
        }
        else {
            napi_create_int32(env, ret, &result);
            return result;
        }
    }

    static napi_value elca_stream_open_port_forwarding(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char service[ELA_MAX_APP_MESSAGE_LEN + 1];
        char host[ELA_MAX_APP_MESSAGE_LEN + 1];
        char port[MAX_PORT_LEN];
        int ret = -1;
        uint32_t protocol;
        napi_value result;

        size_t argc = 4, len;
        napi_value args[4];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("openPortForwarding", 4, value_n1);
        CHECK_STREAM_VALUE(value_n1);

        //get service
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_n1;
        status = napi_get_value_string_utf8(env, args[0], service, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return value_n1;

        //get protocol
        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_n1;
        status = napi_get_value_uint32(env, args[1], &protocol);
        if (status != napi_ok) return value_n1;

        //get host
        status = napi_typeof(env, args[2], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_n1;
        status = napi_get_value_string_utf8(env, args[2], host, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return value_n1;

        //get port
        status = napi_typeof(env, args[3], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_n1;
        status = napi_get_value_string_utf8(env, args[3], port, MAX_PORT_LEN, &len);
        if (status != napi_ok) return value_n1;

        ret = ela_stream_open_port_forwarding(elca->elasession, elca->stream, service, (PortForwardingProtocol)protocol, host, port);
        if (ret == -1) {
            log_err(env, "Run ela_stream_open_port_forwarding error:0x%X", ela_get_error());
            return value_n1;
        }
        else {
            napi_create_int32(env, ret, &result);
            return result;
        }
    }

    static napi_value elca_stream_close_port_forwarding(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        uint32_t portforwarding;
        int ret;

        size_t argc = 1;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("closePortForwarding", 1, value_false);
        CHECK_STREAM_VALUE(value_false);

        //get portforwarding
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_false;
        status = napi_get_value_uint32(env, args[0], &portforwarding);
        if (status != napi_ok) return value_false;

        ret = ela_stream_close_port_forwarding(elca->elasession, elca->stream, portforwarding);
        if (ret == -1) {
            log_err(env, "Run ela_stream_close_port_forwarding error:0x%X", ela_get_error());
            return value_false;
        }
        else {
            return value_true;
        }
    }

    void createStreamFunctions(napi_env env, napi_value session, Elca *elca) {
        napi_status status;
        napi_value fn[1];
        memset(fn, 0, sizeof(napi_value) * 1);

        napi_create_function(env, nullptr, 0, elca_session_add_stream, (void*)elca, &fn[0]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "addStream", NULL, 0, 0, 0, fn[0], napi_default, 0 },
        };

        status = napi_define_properties(env, session,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }

    static void createStreamSelfFunctions(napi_env env, napi_value stream, Elca *elca) {
        napi_status status;
        napi_value fn[7];
        memset(fn, 0, sizeof(napi_value) * 7);

        napi_create_function(env, nullptr, 0, elca_session_remove_stream, (void*)elca, &fn[0]);
        napi_create_function(env, nullptr, 0, elca_stream_get_type, (void*)elca, &fn[1]);
        napi_create_function(env, nullptr, 0, elca_stream_get_state, (void*)elca, &fn[2]);
        napi_create_function(env, nullptr, 0, elca_stream_get_transport_info, (void*)elca, &fn[3]);
        napi_create_function(env, nullptr, 0, elca_stream_write, (void*)elca, &fn[4]);
        napi_create_function(env, nullptr, 0, elca_stream_open_port_forwarding, (void*)elca, &fn[5]);
        napi_create_function(env, nullptr, 0, elca_stream_close_port_forwarding, (void*)elca, &fn[6]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "remove", NULL, 0, 0, 0, fn[0], napi_default, 0 },
            { "getType", NULL, 0, 0, 0, fn[1], napi_default, 0 },
            { "getState", NULL, 0, 0, 0, fn[2], napi_default, 0 },
            { "getTransportInfo", NULL, 0, 0, 0, fn[3], napi_default, 0 },
            { "write", NULL, 0, 0, 0, fn[4], napi_default, 0 },
            { "openPortForwarding", NULL, 0, 0, 0, fn[5], napi_default, 0 },
            { "closePortForwarding", NULL, 0, 0, 0, fn[6], napi_default, 0 },
        };

        status = napi_define_properties(env, stream,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }

} // namespace elca
