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

    void createConstants(napi_env env, napi_value exports);
    void createUtilityFunctions(napi_env env, napi_value carrier);

    void createOnFunctions(napi_env env, napi_value object, Elca *elca);
    void createNodeInfoFunctions(napi_env env, napi_value carrier, Elca *elca);
    void createFriendFunctions(napi_env env, napi_value carrier, Elca *elca);
    void createSessionFunctions(napi_env env, napi_value carrier, Elca *elca);


    #define ANSI_RESET "\033[0m"
    #define ANSI_RED   "\033[31m"

    void log_info(const char *format, ...) {
        va_list args;

        printf("Info: ");
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
        printf("Error: ");
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
        if (elca && elca->elacarrier) {
            uv_work_t* work = nullptr;
            HEAP_ALLOC(work, uv_work_t, 1, nullptr);
            work->data = elca->elacarrier;
            uv_queue_work(uv_default_loop(), work, run_Execute, run_Completed);
        }
        return nullptr;
    }

    static napi_value elca_kill(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elacarrier) {
            ela_kill(elca->elacarrier);
            for (int i = 0; i < CALLBACK_COUNT; i++) {
                 deleteCallbackHandle(env, i, elca);
            }
            napi_delete_reference(env, elca->object);
            free(elca);
        }
        return nullptr;
    }

    static napi_value elca_is_ready(napi_env env, napi_callback_info info) {
        napi_value result = nullptr;
        Elca* elca = nullptr;
        bool ret = false;

        napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&elca);
        if (elca && elca->elacarrier) {
            ret = ela_is_ready(elca->elacarrier);
        }

        napi_get_boolean(env, ret, &result);
        return result;
    }

//------------------------------------------------------------------------------
    static void createInstanceFunctions(napi_env env, napi_value carrier, Elca *elca) {
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
        napi_value obj = value_null;
        napi_value context = nullptr;

        size_t argc = 3;
        napi_value args[3];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        CHECK_STATUS_AND_ARGC("createObject", 2, value_null);

        if (argc > 2) context = args[2];

        memset(&opts, 0, sizeof(ElaOptions));
        opts.persistent_location = buf;
        if (getOptionsValue(env, args[0], &opts, &bootstraps_buf) == nullptr) {
            goto EIXT;
        }

        HEAP_ALLOC(elca, Elca, 1, value_null);
        elca->type = CARRIER;

        setCallbackFuntions(env, args[1], context, elca);
        setCarrierNativeCallbacks(&callbacks);

        carrier = ela_new(&opts, &callbacks, elca);
        if (!carrier) {
            log_err(env, "Run ela_new error:0x%X", ela_get_error());
            goto EIXT;
        }

        status = napi_create_object(env, &obj);
        if (status != napi_ok || !obj) {
            goto EIXT;
        }

        elca->elacarrier = carrier;
        elca->env = env;
        status = napi_create_reference(env, obj, 1, &elca->object);
        CHECK_STATUS;

        createInstanceFunctions(env, obj, elca);
        createOnFunctions(env, obj, elca);
        createNodeInfoFunctions(env, obj, elca);
        createFriendFunctions(env, obj, elca);
        createSessionFunctions(env, obj, elca);

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

        createConstants(env, exports);
        createUtilityFunctions(env, exports);

        return exports;
    }

    NAPI_MODULE(NODE_GYP_MODULE_NAME, init)

} // namespace elca
