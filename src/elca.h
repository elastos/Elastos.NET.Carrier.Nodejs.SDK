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

    #define CHECK_STATUS_AND_ARGC(funtion_name, argc_number, ret_value) do { \
        if (status != napi_ok || argc < argc_number) { \
            log_err(env, "'%s' Wrong number of arguments", funtion_name); \
            return ret_value; \
        } \
    }  while(0)

    #define CHECK_ELACARRIER_PTR(ret_value) do { \
        if (!elca || !elca->elacarrier) { \
            return ret_value; \
        } \
    }  while(0)

    #define CHECK_STREAM_VALUE(ret_value) do { \
        if (!elca|| !elca->elasession || elca->stream < 0) { \
            return ret_value; \
        } \
    }  while(0)

    #define GET_AND_CHECK_CHANNEL_ID(ret_value) do { \
        napi_valuetype valuetype; \
        status = napi_typeof(env, args[0], &valuetype); \
        if (status != napi_ok || valuetype != napi_number) return ret_value; \
        status = napi_get_value_int32(env, args[0], &id);   \
        if (status != napi_ok) return ret_value;    \
        if (!elca || !elca->elasession \
                    || elca->stream < 0 || id < 0) { \
            return ret_value; \
        } \
    }  while(0)

    #define CHECK_ELASESSION_PTR(ret_value) do { \
        if (!elca || !elca->elasession) { \
            return ret_value; \
        } \
    }  while(0)

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
    #define SESSION_REQUEST             13
    #define CARRIER_CALLBACK_COUNT      14

    #define SESSION_REQUEST_COMPLETE    0
    #define SESSION_CALLBACK_COUNT      1

    #define STATE_CHANGED               0
    #define STREAM_DATA                 1
    #define CHANNEL_OPEN                2
    #define CHANNEL_OPENED              3
    #define CHANNEL_DATA                4
    #define CHANNEL_PENDING             5
    #define CHANNEL_RESUME              6
    #define CHANNEL_CLOSE               7
    #define STREAM_CALLBACK_COUNT       8

    #define CALLBACK_COUNT              16

    #define CARRIER                     0
    #define SESSION                     1
    #define STREAM                      2

    extern napi_value value_null;
    extern napi_value value_false;
    extern napi_value value_true;
    extern napi_value value_n1;

    typedef struct _Elca {
        ElaCarrier* elacarrier;
        ElaSession* elasession;
        int stream;

        int type;

        napi_env env;
        napi_ref object;

        napi_ref handle[CALLBACK_COUNT];
        napi_value context[CALLBACK_COUNT];
    } Elca;

    ElaOptions *getOptionsValue(napi_env env, napi_value obj, ElaOptions *opts,
                    BootstrapBuf** bootstraps_buf);

    void setCarrierNativeCallbacks(ElaCallbacks* callbacks);
    void setStreamNativeCallbacks(ElaStreamCallbacks* callbacks);
    void setCallbackHandle(napi_env env, napi_value callback, napi_value context,
                                    int cb_no, Elca *elca);
    void deleteCallbackHandle(napi_env env, int cb_no, Elca *elca);
    void setCallbackFuntions(napi_env env, napi_value callbacks, napi_value context, Elca *elca);

    ElaUserInfo* createUserInfoFromJsObj(napi_env env, napi_value obj, ElaUserInfo *info);

    napi_value createUserInfoJsObj(napi_env env, const ElaUserInfo *info);
    napi_value createFriendInfoJsObj(napi_env env, const ElaFriendInfo *info);
    napi_value createUserInfoFromJsObj(napi_env env, const ElaTransportInfo *info);
}

#endif /* __ELASTOS_CARRIER_ADDON_H__ */