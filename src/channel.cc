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

    static napi_value elca_stream_open_channel(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        napi_value result;
        char cookie[ELA_MAX_APP_MESSAGE_LEN + 1], *ptr = NULL;
        int ret;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        // CHECK_STATUS_AND_ARGC("openChannel", 1, value_null);
        CHECK_STREAM_VALUE(value_n1);

        if (argc) {
            status = napi_typeof(env, args[0], &valuetype);
            CHECK_STATUS;
            if (valuetype == napi_string) {
                status = napi_get_value_string_utf8(env, args[0], cookie, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
                if (status != napi_ok) return value_n1;
                ptr = cookie;
            }
        }

        ret = ela_stream_open_channel(elca->elasession, elca->stream, ptr);
        if (ret == -1) {
            log_err(env, "Run ela_stream_open_channel error:0x%X", ela_get_error());
            return value_n1;
        }
        else {
            napi_create_int32(env, ret, &result);
            return result;
        }
    }

    static napi_value elca_stream_close_channel(napi_env env, napi_callback_info info) {
        Elca *elca = nullptr;
        int ret, id;
        napi_status status;

        size_t argc = 1;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("closeChannel", 1, value_false);
        GET_AND_CHECK_CHANNEL_ID(value_false);

        ret = ela_stream_close_channel(elca->elasession, elca->stream, id);
        if (ret != -1) {
            return value_true;
        }
        else return value_false;
    }

    static napi_value elca_stream_write_channel(napi_env env, napi_callback_info info) {
        Elca *elca = nullptr;
        void *data;
        int ret, id;
        napi_status status;
        napi_value result;

        size_t argc = 2, len;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("writeChannel", 2, value_n1);
        GET_AND_CHECK_CHANNEL_ID(value_n1);

        napi_get_buffer_info(env, args[1], &data, &len);

        ret = ela_stream_write_channel(elca->elasession, elca->stream, id, data, len);
        if (ret == -1) {
            log_err(env, "Run ela_stream_write_channel error:0x%X", ela_get_error());
            return value_n1;
        }
        else {
            napi_create_int32(env, ret, &result);
            return result;
        }
    }

    static napi_value elca_stream_pend_channel(napi_env env, napi_callback_info info) {
        Elca *elca = nullptr;
        int ret, id;
        napi_status status;

        size_t argc = 1;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("pendChannel", 1, value_false);
        GET_AND_CHECK_CHANNEL_ID(value_false);

        ret = ela_stream_pend_channel(elca->elasession, elca->stream, id);
        if (ret != -1) return value_true;
        else return value_false;
    }

    static napi_value elca_stream_resume_channel(napi_env env, napi_callback_info info) {
        Elca *elca = nullptr;
        int ret, id;
        napi_status status;

        size_t argc = 1;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        CHECK_STATUS_AND_ARGC("resumeChannel", 1, value_false);
        GET_AND_CHECK_CHANNEL_ID(value_false);

        ret = ela_stream_resume_channel(elca->elasession, elca->stream, id);
        if (ret != -1) return value_true;
        else return value_false;
    }

    void createChannelFunctions(napi_env env, napi_value stream, Elca *elca) {
        napi_status status;
        napi_value fn[5];
        memset(fn, 0, sizeof(napi_value) * 5);

        napi_create_function(env, nullptr, 0, elca_stream_open_channel, (void*)elca, &fn[0]);
        napi_create_function(env, nullptr, 0, elca_stream_close_channel, (void*)elca, &fn[1]);
        napi_create_function(env, nullptr, 0, elca_stream_write_channel, (void*)elca, &fn[2]);
        napi_create_function(env, nullptr, 0, elca_stream_pend_channel, (void*)elca, &fn[3]);
        napi_create_function(env, nullptr, 0, elca_stream_resume_channel, (void*)elca, &fn[4]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "openChannel", NULL, 0, 0, 0, fn[0], napi_default, 0 },
            { "closeChannel", NULL, 0, 0, 0, fn[1], napi_default, 0 },
            { "writeChannel", NULL, 0, 0, 0, fn[2], napi_default, 0 },
            { "pendChannel", NULL, 0, 0, 0, fn[3], napi_default, 0 },
            { "resumeChannel", NULL, 0, 0, 0, fn[4], napi_default, 0 },
        };

        status = napi_define_properties(env, stream,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }
} // namespace elca
