// /*
//  * Copyright (c) 2018 Elastos Foundation
//  *
//  * Permission is hereby granted, free of charge, to any person obtaining a copy
//  * of this software and associated documentation files (the "Software"), to deal
//  * in the Software without restriction, including without limitation the rights
//  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  * copies of the Software, and to permit persons to whom the Software is
//  * furnished to do so, subject to the following conditions:
//  *
//  * The above copyright notice and this permission notice shall be included in all
//  * copies or substantial portions of the Software.
//  *
//  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  * SOFTWARE.
//  */

#include "elca.h"

namespace elca {

    typedef size_t get_arguments(napi_env env, void *args, napi_value* result);

    typedef struct _ConnectionArgs {
        int32_t status;
    } ConnectionArgs;

    typedef struct _FriendInfoArgs {
        char id[ELA_MAX_ID_LEN + 1];
        ElaFriendInfo info;
    } FriendInfoArgs;

    typedef struct _FriendStatusArgs {
        char id[ELA_MAX_ID_LEN + 1];
        int32_t status;
    } FriendStatusArgs;

    typedef struct _FriendRequestArgs {
        char id[ELA_MAX_ID_LEN + 1];
        ElaUserInfo info;
        char hello[ELA_MAX_APP_MESSAGE_LEN + 1];
    } FriendRequestArgs;

    typedef struct _MessageArgs {
        char from[ELA_MAX_ID_LEN + 1];
        char data[ELA_MAX_APP_MESSAGE_LEN + 1];
        uint32_t len;
    } MessageArgs;

    typedef struct _FriendInviteResponseArgs {
        char from[ELA_MAX_ID_LEN + 1];
        int32_t status;
        char reason[ELA_MAX_APP_MESSAGE_LEN + 1];
        char data[ELA_MAX_APP_MESSAGE_LEN + 1];
        uint32_t len;
    } FriendInviteResponseArgs;

    typedef struct _WorkerInfo {
        uv_work_t work;
        napi_env env;
        napi_ref handle;
        napi_ref carrier;
        napi_value context;
        void* args;
        get_arguments* get_args;
    } WorkerInfo;

    void work_Execute(uv_work_t * work) {
        // printf("uvAsync thread id: %ul\n", pthread_self());
    }

    static void work_Completed(uv_work_t * work, int) {
        napi_status status;
        napi_value argv[8];
        size_t argc = 0;

        _WorkerInfo* info = (_WorkerInfo*)work->data;
        napi_env env = info->env;

        napi_handle_scope scope;
        status = napi_open_handle_scope(env, &scope);
        CHECK_STATUS;

        napi_value carrier;
        status = napi_get_reference_value(env, info->carrier, &carrier);
        CHECK_STATUS;

        if (carrier) {
            argv[0] = carrier;
            if (info->get_args) {
                argc = info->get_args(env, info->args, argv + 1);
            }
            if (info->args) free(info->args);
            argc++;

            if (info->context) {
                argv[argc] = info->context;
                argc++;
            }

            napi_value global;
            status = napi_get_global(env, &global);
            CHECK_STATUS;

            napi_value fn;
            status = napi_get_reference_value(env, info->handle, &fn);
            if (fn) {
                napi_value result;
                status = napi_call_function(env, global, fn, argc, argv, &result);
                CHECK_STATUS;
            }
        }

        status = napi_close_handle_scope(env, scope);
        CHECK_STATUS;

        free(info);
    }

    static inline void send_async_work(int index,  get_arguments* get_args, void* args, void *context) {
        Elca *elca = (Elca*)context;

        if (!elca->handle[index]) return;

        WorkerInfo* info = nullptr;
        HEAP_ALLOC_VOID(info, WorkerInfo, 1);

        info->work.data = info;
        info->env = elca->env;
        info->get_args = get_args;
        info->args = args;
        info->handle = elca->handle[index];
        info->context = elca->context[index];
        info->carrier = elca->carrier;

        uv_queue_work(uv_default_loop(), &info->work, work_Execute, work_Completed);
    }

    //idle_callback
    static void idle_callback(ElaCarrier *w, void *context) {
        send_async_work(IDLE, nullptr, nullptr, context);
    }

    //connection_callback
    static size_t get_ConnectionArgs(napi_env env, void *args, napi_value* argv) {
        int32_t status = *(int32_t*)args;
        napi_create_int32(env, status, argv);
        return 1;
    }

    static void connection_callback(ElaCarrier *w, ElaConnectionStatus status,
                                    void *context) {
        int* args = nullptr;
        HEAP_ALLOC_VOID(args, int, 1);
        *args = status;

        send_async_work(CONNECTION_STATUS, get_ConnectionArgs, args, context);
    }

    //friends_list_callback
    //friends_iterate_callback
    //friend_added_callback
    static size_t get_ElaFriendInfoArgs(napi_env env, void *args, napi_value* argv) {
        ElaFriendInfo* friend_info = (ElaFriendInfo*)args;
        argv[0] = create_FriendInfoJsObj(env, friend_info);
        return 1;
    }

    static bool set_FriendInfoCallback(int cb_no, const ElaFriendInfo *info,
                                    void *context) {
        ElaFriendInfo* friend_info = nullptr;
        if (info) {
            HEAP_ALLOC(friend_info, ElaFriendInfo, 1, false);
            memcpy(friend_info, info, sizeof(ElaFriendInfo));
        }

        send_async_work(cb_no, get_ElaFriendInfoArgs, friend_info, context);
        return true;
    }

    static bool friends_list_callback(ElaCarrier *w, const ElaFriendInfo *info,
                                    void *context) {
        return set_FriendInfoCallback(FRIENDS_LIST, info, context);
    }

    bool friends_iterate_callback(const ElaFriendInfo *info, void *context) {
        return set_FriendInfoCallback(FRIENDS_ITERATE, info, context);
    }

    static void friend_added_callback(ElaCarrier *w, const ElaFriendInfo *info,
                                    void *context) {
        set_FriendInfoCallback(FRIEND_ADDED, info, context);
    }

    //friend_removed_callback
    static size_t get_FriendRemovedArgs(napi_env env, void *args, napi_value* argv) {
        napi_create_string_utf8(env, (char*)args, NAPI_AUTO_LENGTH, argv);
        return 1;
    }

    static void friend_removed_callback(ElaCarrier *w, const char *friendid,  void *context) {
        char* id = nullptr;
        if (friendid) {
            HEAP_ALLOC_VOID(id, char, ELA_MAX_ID_LEN + 1);
            strncpy(id, friendid, ELA_MAX_ID_LEN);
        }

        send_async_work(FRIEND_REMOVED, get_FriendRemovedArgs, id, context);
    }

    //friend_info_callback
    static size_t get_FriendInfoArgs(napi_env env, void *args, napi_value* argv) {
        FriendInfoArgs* fi = (FriendInfoArgs*)args;
        napi_create_string_utf8(env, fi->id, NAPI_AUTO_LENGTH, &argv[0]);
        argv[1] = create_FriendInfoJsObj(env, &fi->info);

        return 2;
    }

    static void friend_info_callback(ElaCarrier *w, const char *friend_id,
                                    const ElaFriendInfo *info, void *context) {
        FriendInfoArgs* fi = nullptr;
        HEAP_ALLOC_VOID(fi, FriendInfoArgs, 1);
        if (friend_id) strncpy(fi->id, friend_id, ELA_MAX_ID_LEN);
        if (info) memcpy(&fi->info, info, sizeof(ElaFriendInfo));

        send_async_work(FRIEND_INFO, get_FriendInfoArgs, fi, context);
    }

    //friend_connection_callback
    //friend_presence_callback
    static size_t get_FriendStatusArgs(napi_env env, void *args, napi_value* argv) {
        FriendStatusArgs *fs = (FriendStatusArgs*)args;
        napi_create_string_utf8(env, fs->id, NAPI_AUTO_LENGTH, &argv[0]);
        napi_create_int32(env,fs->status, &argv[1]);

        return 2;
    }

    static void set_FriendStatusCallback(int cb_no, const char *id, int32_t status,
                                        void *context) {
        FriendStatusArgs* fs = nullptr;
        HEAP_ALLOC_VOID(fs, FriendStatusArgs, 1);
        if (id) strncpy(fs->id, id, ELA_MAX_ID_LEN);
        fs->status = status;

        send_async_work(cb_no, get_FriendStatusArgs, fs, context);
    }

    static void friend_connection_callback(ElaCarrier *w, const char *friend_id,
                                        ElaConnectionStatus status, void *context) {
        set_FriendStatusCallback(FRIEND_CONNECTION, friend_id, status, context);
    }

    static void friend_presence_callback(ElaCarrier *w, const char *friend_id,
                                        ElaPresenceStatus status, void *context) {
        set_FriendStatusCallback(FRIEND_PRESENCE, friend_id, status, context);
    }

    //friend_request_callback
    static size_t get_FriendFequestArgs(napi_env env, void *args, napi_value* argv) {
        FriendRequestArgs *fr = (FriendRequestArgs*)args;
        napi_create_string_utf8(env, fr->id, NAPI_AUTO_LENGTH, &argv[0]);
        argv[1] = create_UserInfoJsObj(env, &fr->info);
        napi_create_string_utf8(env, fr->hello, NAPI_AUTO_LENGTH, &argv[2]);

        return 3;
    }

    static void friend_request_callback(ElaCarrier *w, const char *id,
                                        const ElaUserInfo *info, const char *hello,
                                        void *context) {
        FriendRequestArgs* fr = nullptr;
        HEAP_ALLOC_VOID(fr, FriendRequestArgs, 1);
        if (id) strncpy(fr->id, id, ELA_MAX_ID_LEN);
        if (info) memcpy(&fr->info, info, sizeof(ElaUserInfo));
        if (hello) strncpy(fr->hello, hello, ELA_MAX_APP_MESSAGE_LEN);

        send_async_work(FRIEND_REQUEST, get_FriendFequestArgs, fr, context);
    }

    //message_callback
    //invite_request_callback
    static size_t get_MessageArgs(napi_env env, void *args, napi_value* argv) {
        MessageArgs* message = (MessageArgs*)args;

        napi_create_string_utf8(env, message->from, NAPI_AUTO_LENGTH, &argv[0]);
        napi_create_string_utf8(env, message->data, NAPI_AUTO_LENGTH, &argv[1]);
        napi_create_int32(env, message->len, &argv[2]);

        return 3;
    }

    static void set_MessageCallback(int cb_no, const char *from,
                                const char *data, size_t len, void *context) {
        MessageArgs* message = nullptr;
        HEAP_ALLOC_VOID(message, MessageArgs, 1);
        if (from) strncpy(message->from, from, ELA_MAX_ID_LEN);
        if (data) strncpy(message->data, data, ELA_MAX_APP_MESSAGE_LEN);
        message->len = len;

        send_async_work(cb_no, get_MessageArgs, message, context);
    }

    static void friend_message_callback(ElaCarrier *w, const char *from,
                                const char *msg, size_t len, void *context) {

        set_MessageCallback(FRIEND_MESSAGE, from, msg, len, context);
    }

    static void invite_request_callback(ElaCarrier *w, const char *from,
                                        const char *data, size_t len, void *context) {
        set_MessageCallback(FRIEND_INVITE, from, data, len, context);
    }

    //friend_invite_response_callback
    static size_t get_FriendInviteResponseArgs(napi_env env, void *args, napi_value* argv) {
        FriendInviteResponseArgs* response = (FriendInviteResponseArgs*)args;

        napi_create_string_utf8(env, response->from, NAPI_AUTO_LENGTH, &argv[0]);
        napi_create_int32(env, response->status, &argv[1]);
        napi_create_string_utf8(env, response->reason, NAPI_AUTO_LENGTH, &argv[2]);
        napi_create_string_utf8(env, response->data, NAPI_AUTO_LENGTH, &argv[3]);
        napi_create_uint32(env, response->len, &argv[4]);

        return 5;
    }

    void friend_invite_response_callback(ElaCarrier *carrier, const char *from,
                                    int status, const char *reason, const char *data,
                                    size_t len, void *context) {
        FriendInviteResponseArgs* response = nullptr;
        HEAP_ALLOC_VOID(response, FriendInviteResponseArgs, 1);
        if (from) strncpy(response->from, from, ELA_MAX_ID_LEN);
        if (reason) strncpy(response->reason, reason, ELA_MAX_APP_MESSAGE_LEN);
        if (data) strncpy(response->data, data, ELA_MAX_APP_MESSAGE_LEN);
        response->status = status;
        response->len = len;

        send_async_work(FRIEND_INVITE_RESPONSE, get_FriendInviteResponseArgs, response, context);
    }

//--------------------------------------------------------------------------------

    const char* cb_name[] = {
        "idle",
        "connectionStatus",
        "friendsList",
        "friendConnection",
        "friendInfo",
        "friendPresence",
        "friendRequest",
        "friendAdded",
        "friendRemoved",
        "friendMessage",
        "friendInvite",
        "friendsIterate",
        "friendInviteResponse"
    };

    void set_NativeCallbacks(ElaCallbacks* callbacks) {
        memset(callbacks, 0, sizeof(ElaCallbacks));
        callbacks->idle = idle_callback;
        callbacks->connection_status = connection_callback;
        callbacks->friend_list = friends_list_callback;
        callbacks->friend_connection = friend_connection_callback;
        callbacks->friend_info = friend_info_callback;
        callbacks->friend_presence = friend_presence_callback;
        callbacks->friend_request = friend_request_callback;
        callbacks->friend_added = friend_added_callback;
        callbacks->friend_removed = friend_removed_callback;
        callbacks->friend_message = friend_message_callback;
        callbacks->friend_invite = invite_request_callback;
    }

    void delete_CallbackHandle(napi_env env, int cb_no, Elca *elca) {
        if (elca->handle[cb_no]) {
            napi_delete_reference(env, elca->handle[cb_no]);
            elca->handle[cb_no] = nullptr;
        }

        elca->context[cb_no] = nullptr;
    }

    void set_CallbackHandle(napi_env env, napi_value callback, napi_value context,
                                    int cb_no, Elca *elca) {
        napi_status status;
        napi_valuetype valuetype;

        if (!callback) return;

        status = napi_typeof(env, callback, &valuetype);
        CHECK_STATUS;

        delete_CallbackHandle(env, cb_no, elca);

        if (valuetype != napi_function) {
            return;
        };

        status = napi_create_reference(env, callback, 1, &elca->handle[cb_no]);
        CHECK_STATUS;

        elca->context[cb_no] = context;
    }

    static void set_CallbackHandleByProperty(napi_env env, napi_value callbacks,
                                        napi_value context, int cb_no, Elca *elca) {
        napi_status status;
        napi_value callback = nullptr;

        status = napi_get_named_property(env, callbacks, cb_name[cb_no], &callback);
        if(status != napi_ok || !callback) {
            return;
        }

        return set_CallbackHandle(env, callback, context, cb_no, elca);
    }

    void set_CallbackFuntions(napi_env env, napi_value callbacks, napi_value context, Elca *elca) {
        napi_status status;
        napi_valuetype valuetype;

        status = napi_typeof(env, callbacks, &valuetype);
        CHECK_STATUS;

        if (valuetype != napi_object) return;

        for (int i = 0; i < CALLBACK_COUNT; i++) {
            set_CallbackHandleByProperty(env, callbacks, context, i, elca);
        }
    }

//-----------------------------------------------------------------------

    static void set_CallbackHandleByName(napi_env env, const char* name,
                                        napi_value callback, napi_value context, Elca *elca) {
        for (int i = 0; i < CALLBACK_COUNT; i++) {
            if (!strcmp(name, cb_name[i])) {
                return set_CallbackHandle(env, callback, context, i, elca);
            }
        }
    }

    static napi_value elca_event_on(napi_env env, napi_callback_info info) {
        Elca *elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char name[MAX_EVENT_NAME_LEN];
        napi_value context = nullptr;

        size_t argc = 3, len;
        napi_value args[3];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 2) {
            log_err(env, "elca_event_on: Wrong number of arguments");
            return nullptr;
        }

        if (!elca || !elca->elcarrier) return value_false;
        if (argc > 2) context = args[2];

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return nullptr;

        status = napi_get_value_string_utf8(env, args[0], name, MAX_EVENT_NAME_LEN, &len);
        if (status != napi_ok) return nullptr;

        set_CallbackHandleByName(env, name, args[1], context, elca);
        return nullptr;
    }

    void create_CallBackFunctions(napi_env env, napi_value carrier, Elca *elca) {
        napi_status status;
        napi_value fn[1];
        memset(fn, 0, sizeof(napi_value) * 1);

        napi_create_function(env, nullptr, 0, elca_event_on, (void*)elca, &fn[0]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "on", NULL, 0, 0, 0, fn[0], napi_default, 0 },
        };

        status = napi_define_properties(env, carrier,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }

} // namespace elca
