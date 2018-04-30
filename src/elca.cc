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

#include <unistd.h>

namespace elca {

    #define ANSI_RESET "\033[0m"
    #define ANSI_RED   "\033[31m"

    void log_info(const char *format, ...) {
        va_list args;

        printf("Info:");
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
        printf("\n");
    }

    void log_err(napi_env env, const char *format, ...) {
        char buf[1024];
        va_list args;

        const char *reset_color = ANSI_RESET;
        const char *red_color = ANSI_RED;

        // Only print colors to a terminal
        if (!isatty(STDOUT_FILENO)) {
            reset_color = "";
            red_color = "";
        }

        printf("%s", red_color);
        printf("Error:");
        va_start(args, format);
        vsnprintf(buf, 1024, format, args);
        va_end(args);
        puts(buf);
        printf("%s", reset_color);

        if (env) {
#ifdef DEBUG
        // napi_throw_error(env, nullptr, buf);
#endif
        }
    }

    static void run_Execute(uv_work_t * work) {
        ElaCarrier* carrier = (ElaCarrier*)work->data;
        int rc = ela_run(carrier, 10);
        if (rc != 0) {
            log_err(nullptr, "Run ela_run error:0x%X", ela_get_error());
        }
    }

    static void run_Completed(uv_work_t * work, int) {
        if (work) {
            free(work);
        }
    }

    static napi_value elca_run(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elcarrier) {
            uv_work_t* work = nullptr;
            HEAP_ALLOC(work, uv_work_t, 1, nullptr);
            work->data = elca->elcarrier;
            uv_queue_work(uv_default_loop(), work, run_Execute, run_Completed);
        }
        return nullptr;
    }

    static napi_value elca_kill(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elcarrier) {
            ela_kill(elca->elcarrier);
            for (int i = 0; i < CALLBACK_COUNT; i++) {
                 delete_CallbackHandle(env, i, elca);
            }
            napi_delete_reference(env, elca->carrier);
            free(elca);
        }
        return nullptr;
    }

    static napi_value elca_is_ready(napi_env env, napi_callback_info info) {
        napi_value result = nullptr;
        Elca* elca = nullptr;
        bool ret = false;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elcarrier) {
            ret = ela_is_ready(elca->elcarrier);
        }

        napi_get_boolean(env, ret, &result);
        return result;
    }

//------------------------------------------------------------------------------
    static void create_InstanceFunctions(napi_env env, napi_value carrier, Elca *elca) {
        napi_status status;
        napi_value fn[3];
        memset(fn, 0, sizeof(napi_value) * 3);

        napi_create_function(env, nullptr, 0, elca_run, (void*)elca, &fn[0]);
        napi_create_function(env, nullptr, 0, elca_kill, (void*)elca, &fn[1]);
        napi_create_function(env, nullptr, 0, elca_is_ready, (void*)elca, &fn[2]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "run", NULL, 0, 0, 0, fn[0], napi_default, 0 },
            { "destroy", NULL, 0, 0, 0, fn[1], napi_default, 0 },
            { "isReady", NULL, 0, 0, 0, fn[2], napi_default, 0 },
        };

        status = napi_define_properties(env, carrier,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }

    napi_value elca_new(napi_env env, napi_callback_info info) {
        Elca *elca = nullptr;
        char buf[1024];

        ElaOptions opts;
        ElaCallbacks callbacks;
        BootstrapBuf *bootstraps_buf = nullptr;
        ElaCarrier *carrier;

        napi_status status;
        napi_value obj = nullptr;
        napi_value context = nullptr;

        size_t argc = 3;
        napi_value args[3];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        if(status != napi_ok || argc < 2) {
            log_err(env, "elca_new: Wrong number of arguments");
            return nullptr;
        }

        if (argc > 2) context = args[2];

        memset(&opts, 0, sizeof(ElaOptions));
        opts.persistent_location = buf;
        if (get_OptionsValue(env, args[0], &opts, &bootstraps_buf) == nullptr) {
            goto EIXT;
        }

        HEAP_ALLOC(elca, Elca, 1, nullptr);

        set_CallbackFuntions(env, args[1], context, elca);
        set_NativeCallbacks(&callbacks);

        carrier = ela_new(&opts, &callbacks, elca);
        if (!carrier) {
            log_err(env, "Run ela_new error:0x%X", ela_get_error());
            obj = nullptr;
            goto EIXT;
        }

        status = napi_create_object(env, &obj);
        if (status != napi_ok || !obj) {
            obj = nullptr;
            goto EIXT;
        }

        elca->elcarrier = carrier;
        elca->env = env;
        status = napi_create_reference(env, obj, 1, &elca->carrier);
        CHECK_STATUS;

        create_InstanceFunctions(env, obj, elca);
        create_CallBackFunctions(env, obj, elca);
        create_NodeInfoFunctions(env, obj, elca);
        create_FriendFunctions(env, obj, elca);

EIXT:
        if (!obj && elca) free(elca);
        if (opts.bootstraps) free(opts.bootstraps);
        if (bootstraps_buf) free(bootstraps_buf);

        return obj;
    }

    napi_value init(napi_env env, napi_value exports) {
        napi_status status;
        napi_value fn;

        status = napi_create_function(env, nullptr, 0, elca_new, nullptr, &fn);
        if (status != napi_ok) return nullptr;

        status = napi_set_named_property(env, exports, "createObject", fn);
        if (status != napi_ok) return nullptr;

        create_Constants(env, exports);
        create_UtilityFunctions(env, exports);

        return exports;
    }

    NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

} // namespace elca
