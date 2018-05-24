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

    static napi_value elca_get_address(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        char address[ELA_MAX_ADDRESS_LEN + 1];
        char* ret = nullptr;
        napi_value result;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elacarrier) {
            ret = ela_get_address(elca->elacarrier, address, ELA_MAX_ADDRESS_LEN + 1);
            if (!ret) return value_null;
            napi_create_string_utf8(env, address, NAPI_AUTO_LENGTH, &result);
            return result;
        }

        return value_null;
    }

    static napi_value elca_get_nodeid(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        char nodeid[ELA_MAX_ID_LEN + 1];
        char* ret = nullptr;
        napi_value result;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elacarrier) {
            ret = ela_get_nodeid(elca->elacarrier, nodeid, ELA_MAX_ID_LEN + 1);
            if (!ret) return value_null;
            napi_create_string_utf8(env, nodeid, NAPI_AUTO_LENGTH, &result);
            return result;
        }

        return value_null;
    }

    static napi_value elca_get_userid(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        char userid[ELA_MAX_ID_LEN + 1];
        char* ret = nullptr;
        napi_value result;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elacarrier) {
            ret = ela_get_userid(elca->elacarrier, userid, ELA_MAX_ID_LEN + 1);
            if (!ret) return value_null;
            napi_create_string_utf8(env, userid, NAPI_AUTO_LENGTH, &result);
            return result;
        }

        return value_null;
    }

    static napi_value elca_get_id_by_address(napi_env env, napi_callback_info info) {
        char address[ELA_MAX_ADDRESS_LEN + 1];
        char user_id[ELA_MAX_ID_LEN + 1];
        char* ret = nullptr;
        napi_value result;
        napi_status status;
        napi_valuetype valuetype;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        CHECK_STATUS_AND_ARGC("getIdByAddress", 1, value_null);

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_null;

        status = napi_get_value_string_utf8(env, args[0], address, ELA_MAX_ADDRESS_LEN + 1, &len);
        if (status != napi_ok) return value_null;

        ret = ela_get_id_by_address(address, user_id, ELA_MAX_ID_LEN + 1);
        if (!ret) return value_null;

        napi_create_string_utf8(env, user_id, NAPI_AUTO_LENGTH, &result);
        return result;

        return value_null;
    }

    static napi_value elca_get_self_nospam(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        uint32_t nospam;
        int ret;
        napi_value result;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elacarrier) {
            ret = ela_get_self_nospam(elca->elacarrier, &nospam);
            if (ret == -1) return value_null;
            napi_create_uint32(env, nospam, &result);
            return result;
        }

        return value_null;
    }

    static napi_value elca_set_self_nospam(napi_env env, napi_callback_info info) {
        Elca* elca;
        napi_status status;
        napi_valuetype valuetype;
        uint32_t nospam;
        int ret;

        size_t argc = 1;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("setSelfNospam", 1, value_false);
        CHECK_ELACARRIER_PTR(value_false);

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_false;

        status = napi_get_value_uint32(env, args[0],  &nospam);
        if (status != napi_ok) return value_false;

        ret = ela_set_self_nospam(elca->elacarrier, nospam);
        if (ret == -1) {
            log_err(env, "Run ela_set_self_nospam error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    static napi_value elca_get_self_presence(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        ElaPresenceStatus presence;
        int ret;
        napi_value result;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elacarrier) {
            ret = ela_get_self_presence(elca->elacarrier, &presence);
            if (ret == -1) return value_null;
            napi_create_uint32(env, presence, &result);
            return result;
        }

        return value_null;
    }

    static napi_value elca_set_self_presence(napi_env env, napi_callback_info info) {
        Elca* elca;
        napi_status status;
        napi_valuetype valuetype;
        uint32_t presence;
        int ret;

        size_t argc = 1;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("setSelfPresence", 1, value_false);

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_false;

        status = napi_get_value_uint32(env, args[0],  &presence);
        if (status != napi_ok) return value_false;

        ret = ela_set_self_presence(elca->elacarrier, (ElaPresenceStatus)presence);
        if (ret == -1) return value_false;
        else return value_true;
    }

    static napi_value elca_get_self_info(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        ElaUserInfo user_info;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elacarrier) {
            if (!ela_get_self_info(elca->elacarrier, &user_info)) {
                return createUserInfoJsObj(env, &user_info);
            }
        }

        return value_null;
    }

    static napi_value elca_set_self_info(napi_env env, napi_callback_info info) {
        Elca* elca;
        napi_status status;
        napi_valuetype valuetype;
        ElaUserInfo user_info;
        int ret = -1;

        size_t argc = 1;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("setSelfInfo", 1, value_false);

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_object) return value_false;

        if (createUserInfoFromJsObj(env, args[0], &user_info)) {
            ret = ela_set_self_info(elca->elacarrier, &user_info);
        }

        if (ret == -1) {
            log_err(env, "Run ela_set_self_info error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    void createNodeInfoFunctions(napi_env env, napi_value carrier, Elca *elca) {
        napi_status status;
        napi_value fn[10];
        memset(fn, 0, sizeof(napi_value) * 10);

        napi_create_function(env, nullptr, 0, elca_get_address, (void*)elca, &fn[0]);
        napi_create_function(env, nullptr, 0, elca_get_nodeid, (void*)elca, &fn[1]);
        napi_create_function(env, nullptr, 0, elca_get_userid, (void*)elca, &fn[2]);
        napi_create_function(env, nullptr, 0, elca_get_id_by_address, (void*)elca, &fn[3]);
        napi_create_function(env, nullptr, 0, elca_set_self_nospam, (void*)elca, &fn[4]);
        napi_create_function(env, nullptr, 0, elca_get_self_nospam, (void*)elca, &fn[5]);
        napi_create_function(env, nullptr, 0, elca_set_self_info, (void*)elca, &fn[6]);
        napi_create_function(env, nullptr, 0, elca_get_self_info, (void*)elca, &fn[7]);
        napi_create_function(env, nullptr, 0, elca_set_self_presence, (void*)elca, &fn[8]);
        napi_create_function(env, nullptr, 0, elca_get_self_presence, (void*)elca, &fn[9]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "getAddress", NULL, 0, 0, 0, fn[0], napi_default, 0 },
            { "getNodeId", NULL, 0, 0, 0, fn[1], napi_default, 0 },
            { "getUserId", NULL, 0, 0, 0, fn[2], napi_default, 0 },
            { "getIdByAddress", NULL, 0, 0, 0, fn[3], napi_default, 0 },
            { "setSelfNospam", NULL, 0, 0, 0, fn[4], napi_default, 0 },
            { "getSelfNospam", NULL, 0, 0, 0, fn[5], napi_default, 0 },
            { "setSelfInfo", NULL, 0, 0, 0, fn[6], napi_default, 0 },
            { "getSelfInfo", NULL, 0, 0, 0, fn[7], napi_default, 0 },
            { "setSelfPresence", NULL, 0, 0, 0, fn[8], napi_default, 0 },
            { "getSelfPresence", NULL, 0, 0, 0, fn[9], napi_default, 0 },
        };

        status = napi_define_properties(env, carrier,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }

} // namespace elca
