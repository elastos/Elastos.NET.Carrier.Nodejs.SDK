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

#ifndef __ELASTOS_CARRIER_ADDON_H__
#define __ELASTOS_CARRIER_ADDON_H__

#include <node_api.h>
#include <uv.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>

#include <ela_carrier.h>
#include <ela_session.h>

namespace elca {

    void log_info(const char *format, ...);
    void log_err(napi_env env, const char *format, ...);

    #define CHECK_STATUS                                \
        assert(status == napi_ok);

    #define HEAP_ALLOC(ptr, type, size, ret) do { \
            ptr = (type*)calloc(1, sizeof(type) * (size)); \
            if (!ptr) {      \
                log_err(nullptr, "Malloc '#type' out of memory."); \
                return ret; \
            } \
        } while(0)

    #define HEAP_ALLOC_VOID(ptr, type, size) do { \
            ptr = (type*)calloc(1, sizeof(type) * (size)); \
            if (!ptr) {      \
                log_err(nullptr, "Malloc '#type' out of memory."); \
                return; \
            } \
        } while(0)

    #define MAX_IPV4_ADDRESS_LEN (16)
    #define MAX_IPV6_ADDRESS_LEN (48)
    #define MAX_PORT_LEN         (16)
    #define MAX_PUBLIC_KEY_LEN   (48)

    #define MAX_EVENT_NAME_LEN   (64)

    typedef struct _BootstrapBuf {
        char ipv4[MAX_IPV4_ADDRESS_LEN];
        char ipv6[MAX_IPV6_ADDRESS_LEN];
        char port[MAX_PORT_LEN];
        char public_key[MAX_PUBLIC_KEY_LEN];
    } BootstrapBuf;

    #define IDLE                        0
    #define CONNECTION_STATUS           1
    #define FRIENDS_LIST                2
    #define FRIEND_CONNECTION           3
    #define FRIEND_INFO                 4
    #define FRIEND_PRESENCE             5
    #define FRIEND_REQUEST              6
    #define FRIEND_ADDED                7
    #define FRIEND_REMOVED              8
    #define FRIEND_MESSAGE              9
    #define FRIEND_INVITE               10
    #define FRIENDS_ITERATE             11
    #define FRIEND_INVITE_RESPONSE      12
    #define CALLBACK_COUNT              13

    extern napi_value value_null;
    extern napi_value value_false;
    extern napi_value value_true;

    typedef struct _Elca {
        ElaCarrier*  elcarrier;

        napi_env env;
        napi_ref carrier;

        napi_ref handle[CALLBACK_COUNT];
        napi_value context[CALLBACK_COUNT];
    } Elca;

    ElaOptions *get_OptionsValue(napi_env env, napi_value obj, ElaOptions *opts,
                    BootstrapBuf** bootstraps_buf);

    void set_NativeCallbacks(ElaCallbacks* callbacks);
    void set_CallbackHandle(napi_env env, napi_value callback, napi_value context,
                                    int cb_no, Elca *elca);
    void delete_CallbackHandle(napi_env env, int cb_no, Elca *elca);
    void set_CallbackFuntions(napi_env env, napi_value callbacks, napi_value context, Elca *elca);

    ElaUserInfo* get_UserInfoFromJsObj(napi_env env, napi_value obj, ElaUserInfo *info);

    napi_value create_UserInfoJsObj(napi_env env, const ElaUserInfo *info);
    napi_value create_FriendInfoJsObj(napi_env env, const ElaFriendInfo *info);

    void create_Constants(napi_env env, napi_value exports);
    void create_UtilityFunctions(napi_env env, napi_value carrier);

    void create_CallBackFunctions(napi_env env, napi_value carrier, Elca *elca);
    void create_NodeInfoFunctions(napi_env env, napi_value carrier, Elca *elca);
    void create_FriendFunctions(napi_env env, napi_value carrier, Elca *elca);
}

#endif /* __ELASTOS_CARRIER_ADDON_H__ */