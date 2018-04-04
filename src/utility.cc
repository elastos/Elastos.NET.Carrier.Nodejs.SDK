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

    static napi_value elca_get_version(napi_env env, napi_callback_info info) {
        napi_value result;
        const char* ret = ela_get_version();
        if (!ret) return nullptr;
        napi_create_string_utf8(env, ret, NAPI_AUTO_LENGTH, &result);
        return result;
    }

    static napi_value elca_address_is_valid(napi_env env, napi_callback_info info) {
        napi_status status;
        napi_valuetype valuetype;
        char address[ELA_MAX_ADDRESS_LEN + 1];
        int ret = -1;

        size_t argc = 2, len;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        if(status != napi_ok || argc < 1) {
            log_err(env, "elca_address_is_valid: Wrong number of arguments");
            return value_false;
        }

        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], address, ELA_MAX_ADDRESS_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_address_is_valid(address);
        if (ret) {
            return value_true;
        }
        else {
            return value_false;
        }
    }

    static napi_value elca_log_init(napi_env env, napi_callback_info info) {
        napi_status status;
        napi_valuetype valuetype;
        char log_file[1024];
        char* ptr = NULL;
        uint32_t level;

        size_t argc = 2, len;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        if(status != napi_ok || argc < 1) {
            log_err(env, "elca_log_init: Wrong number of arguments");
            return value_false;
        }

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_false;

        status = napi_get_value_uint32(env, args[0],  &level);
        if (status != napi_ok) return value_false;

        status = napi_typeof(env, args[1], &valuetype);
        CHECK_STATUS;
        if (valuetype == napi_string) {
            status = napi_get_value_string_utf8(env, args[1], log_file, 1024, &len);
            if (status != napi_ok) return value_false;
            ptr = log_file;
        }

        ela_log_init((ElaLogLevel)level, ptr, NULL);
        return nullptr;
    }

    static napi_value elca_id_is_valid(napi_env env, napi_callback_info info) {
        napi_status status;
        napi_valuetype valuetype;
        char id[ELA_MAX_ID_LEN + 1];
        int ret = -1;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        if(status != napi_ok || argc < 1) {
            log_err(env, "elca_id_is_valid: Wrong number of arguments");
            return value_false;
        }

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], id, ELA_MAX_ID_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_id_is_valid(id);
        if (ret) {
            return value_true;
        }
        else {
            return value_false;
        }
    }

    static napi_value elca_get_error(napi_env env, napi_callback_info info) {
        napi_value result;

        int ret = ela_get_error();
        if (ret == -1) return nullptr;
        napi_create_uint32(env, ret, &result);
        return result;
    }

    static napi_value elca_clear_error(napi_env env, napi_callback_info info) {
        ela_clear_error();
        return nullptr;
    }

    void create_UtilityFunctions(napi_env env, napi_value exports) {
        napi_status status;
        napi_value fn[6];
        memset(fn, 0, sizeof(napi_value) * 6);

        napi_create_function(env, nullptr, 0, elca_get_version, nullptr, &fn[0]);
        napi_create_function(env, nullptr, 0, elca_address_is_valid, nullptr, &fn[1]);
        napi_create_function(env, nullptr, 0, elca_id_is_valid, nullptr, &fn[2]);
        napi_create_function(env, nullptr, 0, elca_get_error, nullptr, &fn[3]);
        napi_create_function(env, nullptr, 0, elca_clear_error, nullptr, &fn[4]);
        napi_create_function(env, nullptr, 0, elca_log_init, nullptr, &fn[5]);


        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "getVersion", NULL, 0, 0, 0, fn[0], napi_default, 0 },
            { "addressIsValid", NULL, 0, 0, 0, fn[1], napi_default, 0 },
            { "idIsValid", NULL, 0, 0, 0, fn[2], napi_default, 0 },
            { "getError", NULL, 0, 0, 0, fn[3], napi_default, 0 },
            { "clearError", NULL, 0, 0, 0, fn[4], napi_default, 0 },
            { "logInit", NULL, 0, 0, 0, fn[5], napi_default, 0 },
        };

        status =  napi_define_properties(env, exports,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }

} // namespace elca
