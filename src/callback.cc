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

    const char* carrier_cb_name[] = {
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
        "friendInviteResponse",
        "sessionRequest"
    };

    const char* session_cb_name[] = {
        "sessionRequestComplete"
    };

    const char* stream_cb_name[] = {
        "stateChanged",
        "streamData",
        "channelOpen",
        "channelOpened",
        "channelData",
        "channelPending",
        "channelResume",
        "channelClose"
    };

    typedef size_t get_arguments(napi_env env, void *args, napi_value* result);

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

    typedef struct _SessionRequestArgs {
        char from[ELA_MAX_ID_LEN + 1];
        char sdp[ELA_MAX_APP_MESSAGE_LEN + 1];
    } SessionRequestArgs;

    typedef struct _WorkerInfo {
        uv_work_t work;
        napi_env env;
        napi_ref handle;
        napi_ref object;
        napi_value context;
        const char* fn_name;
        void* args;
        get_arguments* get_args;
    } WorkerInfo;

    static const char** getCallbackNamesAndCount(Elca *elca, int *count) {
        switch(elca->type) {
            case CARRIER:
                if (count) *count = CARRIER_CALLBACK_COUNT;
                return carrier_cb_name;
            case SESSION:
                if (count) *count = SESSION_CALLBACK_COUNT;
                return session_cb_name;
            case STREAM:
                if (count) *count = STREAM_CALLBACK_COUNT;
                return stream_cb_name;
        }
        if (count) *count = 0;
        return nullptr;
    }

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

        napi_value object;
        status = napi_get_reference_value(env, info->object, &object);
        CHECK_STATUS;

        if (object) {
            argv[0] = object;
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
                if (status != napi_ok) {
                    log_err(env, "Callback function '%s' run wrong!", info->fn_name);
                    // napi_throw_error(env, "", "callback function error!");
                }
            }
        }

        status = napi_close_handle_scope(env, scope);
        CHECK_STATUS;

        free(info);
    }

    static inline void send_async_work(int index,  get_arguments* get_args, void* args, void *context) {
        Elca *elca = (Elca*)context;
        const char** cb_name;
        int count;

        if (!elca->handle[index]) return;
        cb_name = getCallbackNamesAndCount(elca, &count);
        if (!cb_name || index >= count) return;

        WorkerInfo* info = nullptr;
        HEAP_ALLOC_VOID(info, WorkerInfo, 1);

        info->work.data = info;
        info->env = elca->env;
        info->get_args = get_args;
        info->args = args;
        info->handle = elca->handle[index];
        info->context = elca->context[index];
        info->object = elca->object;
        info->fn_name = cb_name[index];

        uv_queue_work(uv_default_loop(), &info->work, work_Execute, work_Completed);
    }


    static size_t getIntValueArgs(napi_env env, void *args, napi_value* argv) {
        int32_t value = *(int32_t*)args;
        napi_create_int32(env, value, argv);
        return 1;
    }

    static void setIntValueCallback(int cb_no, int value, void* context) {
        int* args = nullptr;
        HEAP_ALLOC_VOID(args, int, 1);
        *args = value;

        send_async_work(cb_no, getIntValueArgs, args, context);
    }

    //idle_callback
    static void idle_callback(ElaCarrier *w, void *context) {
        send_async_work(IDLE, nullptr, nullptr, context);
    }

    //connection_callback
    static void connection_callback(ElaCarrier *w, ElaConnectionStatus status,
                                    void *context) {
        setIntValueCallback(CONNECTION_STATUS, status, context);
    }

    //friends_list_callback
    //friends_iterate_callback
    //friend_added_callback
    static size_t getElaFriendInfoArgs(napi_env env, void *args, napi_value* argv) {
        ElaFriendInfo* friend_info = (ElaFriendInfo*)args;
        argv[0] = createFriendInfoJsObj(env, friend_info);
        return 1;
    }

    static bool setFriendInfoCallback(int cb_no, const ElaFriendInfo *info,
                                    void *context) {
        ElaFriendInfo* friend_info = nullptr;
        if (info) {
            HEAP_ALLOC(friend_info, ElaFriendInfo, 1, false);
            memcpy(friend_info, info, sizeof(ElaFriendInfo));
        }

        send_async_work(cb_no, getElaFriendInfoArgs, friend_info, context);
        return true;
    }

    static bool friends_list_callback(ElaCarrier *w, const ElaFriendInfo *info,
                                    void *context) {
        return setFriendInfoCallback(FRIENDS_LIST, info, context);
    }

    bool friends_iterate_callback(const ElaFriendInfo *info, void *context) {
        return setFriendInfoCallback(FRIENDS_ITERATE, info, context);
    }

    static void friend_added_callback(ElaCarrier *w, const ElaFriendInfo *info,
                                    void *context) {
        setFriendInfoCallback(FRIEND_ADDED, info, context);
    }

    //friend_removed_callback
    static size_t getFriendRemovedArgs(napi_env env, void *args, napi_value* argv) {
        napi_create_string_utf8(env, (char*)args, NAPI_AUTO_LENGTH, argv);
        return 1;
    }

    static void friend_removed_callback(ElaCarrier *w, const char *friendid,  void *context) {
        char* id = nullptr;
        if (friendid) {
            HEAP_ALLOC_VOID(id, char, ELA_MAX_ID_LEN + 1);
            strncpy(id, friendid, ELA_MAX_ID_LEN);
        }

        send_async_work(FRIEND_REMOVED, getFriendRemovedArgs, id, context);
    }

    //friend_info_callback
    static size_t getFriendInfoArgs(napi_env env, void *args, napi_value* argv) {
        FriendInfoArgs* fi = (FriendInfoArgs*)args;
        napi_create_string_utf8(env, fi->id, NAPI_AUTO_LENGTH, &argv[0]);
        argv[1] = createFriendInfoJsObj(env, &fi->info);

        return 2;
    }

    static void friend_info_callback(ElaCarrier *w, const char *friend_id,
                                    const ElaFriendInfo *info, void *context) {
        FriendInfoArgs* fi = nullptr;
        HEAP_ALLOC_VOID(fi, FriendInfoArgs, 1);
        if (friend_id) strncpy(fi->id, friend_id, ELA_MAX_ID_LEN);
        if (info) memcpy(&fi->info, info, sizeof(ElaFriendInfo));

        send_async_work(FRIEND_INFO, getFriendInfoArgs, fi, context);
    }

    //friend_connection_callback
    //friend_presence_callback
    static size_t getFriendStatusArgs(napi_env env, void *args, napi_value* argv) {
        FriendStatusArgs *fs = (FriendStatusArgs*)args;
        napi_create_string_utf8(env, fs->id, NAPI_AUTO_LENGTH, &argv[0]);
        napi_create_int32(env,fs->status, &argv[1]);

        return 2;
    }

    static void setFriendStatusCallback(int cb_no, const char *id, int32_t status,
                                        void *context) {
        FriendStatusArgs* fs = nullptr;
        HEAP_ALLOC_VOID(fs, FriendStatusArgs, 1);
        if (id) strncpy(fs->id, id, ELA_MAX_ID_LEN);
        fs->status = status;

        send_async_work(cb_no, getFriendStatusArgs, fs, context);
    }

    static void friend_connection_callback(ElaCarrier *w, const char *friend_id,
                                        ElaConnectionStatus status, void *context) {
        setFriendStatusCallback(FRIEND_CONNECTION, friend_id, status, context);
    }

    static void friend_presence_callback(ElaCarrier *w, const char *friend_id,
                                        ElaPresenceStatus status, void *context) {
        setFriendStatusCallback(FRIEND_PRESENCE, friend_id, status, context);
    }

    //friend_request_callback
    static size_t getFriendFequestArgs(napi_env env, void *args, napi_value* argv) {
        FriendRequestArgs *fr = (FriendRequestArgs*)args;
        napi_create_string_utf8(env, fr->id, NAPI_AUTO_LENGTH, &argv[0]);
        argv[1] = createUserInfoJsObj(env, &fr->info);
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

        send_async_work(FRIEND_REQUEST, getFriendFequestArgs, fr, context);
    }

    //message_callback
    //invite_request_callback
    static size_t getMessageArgs(napi_env env, void *args, napi_value* argv) {
        MessageArgs* message = (MessageArgs*)args;

        napi_create_string_utf8(env, message->from, NAPI_AUTO_LENGTH, &argv[0]);
        napi_create_string_utf8(env, message->data, NAPI_AUTO_LENGTH, &argv[1]);
        napi_create_int32(env, message->len, &argv[2]);

        return 3;
    }

    static void setMessageCallback(int cb_no, const char *from,
                                const char *data, size_t len, void *context) {
        MessageArgs* message = nullptr;
        HEAP_ALLOC_VOID(message, MessageArgs, 1);
        if (from) strncpy(message->from, from, ELA_MAX_ID_LEN);
        if (data) strncpy(message->data, data, ELA_MAX_APP_MESSAGE_LEN);
        message->len = len;

        send_async_work(cb_no, getMessageArgs, message, context);
    }

    static void friend_message_callback(ElaCarrier *w, const char *from,
                                const char *msg, size_t len, void *context) {

        setMessageCallback(FRIEND_MESSAGE, from, msg, len, context);
    }

    static void invite_request_callback(ElaCarrier *w, const char *from,
                                        const char *data, size_t len, void *context) {
        setMessageCallback(FRIEND_INVITE, from, data, len, context);
    }

    //friend_invite_response_callback
    static size_t getFriendInviteResponseArgs(napi_env env, void *args, napi_value* argv) {
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

        send_async_work(FRIEND_INVITE_RESPONSE, getFriendInviteResponseArgs, response, context);
    }

    //session_request_callback
    static size_t getSessionRequestArgs(napi_env env, void *args, napi_value* argv) {
        SessionRequestArgs *session = (SessionRequestArgs*)args;
        napi_create_string_utf8(env, session->from, NAPI_AUTO_LENGTH, &argv[0]);
        napi_create_string_utf8(env, session->sdp, NAPI_AUTO_LENGTH, &argv[1]);

        return 2;
    }

    void session_request_callback(ElaCarrier *w, const char *from,
                const char *sdp, size_t len, void *context) {

        SessionRequestArgs* session = nullptr;
        HEAP_ALLOC_VOID(session, SessionRequestArgs, 1);
        if (from) strncpy(session->from, from, ELA_MAX_ID_LEN);
        if (sdp) strncpy(session->sdp, sdp, ELA_MAX_APP_MESSAGE_LEN);

        send_async_work(SESSION_REQUEST, getSessionRequestArgs, session, context);
    }

//----- session callbaks ---------------------------------------------------------
    typedef struct _SessionRequestCompleteArgs {
        int32_t status;
        char reason[ELA_MAX_APP_MESSAGE_LEN + 1];
        char sdp[ELA_MAX_APP_MESSAGE_LEN + 1];
    } SessionRequestCompleteArgs;

    typedef struct _StreamDataArgs {
        void* data;
        size_t len;
    } StreamDataArgs;

    typedef struct _ChannelDataArgs {
        int32_t id;
        void* data;
        size_t len;
    } ChannelDataArgs;

    typedef struct _ChannelOpenArgs {
        int32_t id;
        char cookie[ELA_MAX_APP_MESSAGE_LEN + 1];
    } ChannelOpenArgs;

    typedef struct _ChannelCloseArgs {
        int32_t id;
        uint32_t reason;
    } ChannelCloseArgs;

    //session_request_complete_callback
    static size_t getSessionRequestCompleteArgs(napi_env env, void *args, napi_value* argv) {
        SessionRequestCompleteArgs *session = (SessionRequestCompleteArgs*)args;
        napi_create_uint32(env, session->status, &argv[0]);
        napi_create_string_utf8(env, session->reason, NAPI_AUTO_LENGTH, &argv[1]);
        napi_create_string_utf8(env, session->sdp, NAPI_AUTO_LENGTH, &argv[2]);

        return 3;
    }

    void session_request_complete_callback(ElaSession *ws, int status,
                    const char *reason, const char *sdp, size_t len, void *context) {

        SessionRequestCompleteArgs* session = nullptr;
        HEAP_ALLOC_VOID(session, SessionRequestCompleteArgs, 1);
        if (reason) strncpy(session->reason, reason, ELA_MAX_APP_MESSAGE_LEN);
        if (sdp) strncpy(session->sdp, sdp, ELA_MAX_APP_MESSAGE_LEN);
        session->status = status;

        send_async_work(SESSION_REQUEST_COMPLETE, getSessionRequestCompleteArgs, session, context);
    }

    //state_changed
    static size_t getSataChangedArgs(napi_env env, void *args, napi_value* argv) {
        int32_t state = *(int32_t*)args;
        napi_create_int32(env, state, argv);
        return 1;
    }

    static void state_changed(ElaSession *ws, int stream, ElaStreamState state, void *context) {
        int* args = nullptr;
        HEAP_ALLOC_VOID(args, int, 1);
        *args = (int)state;

        send_async_work(STATE_CHANGED, getSataChangedArgs, args, context);
    }

    //stream_data
    static size_t getStreamDataArgs(napi_env env, void *args, napi_value* argv) {
        StreamDataArgs* dataArgs = (StreamDataArgs*)args;
        void *data;
        napi_create_buffer_copy(env, dataArgs->len, dataArgs->data, &data, &argv[0]);
        free(dataArgs->data);
        return 1;
    }

    static void stream_data(ElaSession *ws, int stream, const void *data,
                            size_t len, void *context) {
        if (!data || len < 1) return;

        StreamDataArgs* dataArgs = nullptr;
        HEAP_ALLOC_VOID(dataArgs, StreamDataArgs, 1);
        HEAP_ALLOC_VOID(dataArgs->data, char, len);
        memcpy(dataArgs->data, data, len);
        dataArgs->len = len;

        send_async_work(STREAM_DATA, getStreamDataArgs, dataArgs, context);
    }

    //channel_data
    static size_t getChannelDataArgs(napi_env env, void *args, napi_value* argv) {
        ChannelDataArgs* dataArgs = (ChannelDataArgs*)args;
        void *data;
        napi_create_int32(env, dataArgs->id, &argv[0]);
        napi_create_buffer_copy(env, dataArgs->len, dataArgs->data, &data, &argv[1]);
        free(dataArgs->data);
        return 2;
    }

    bool channel_data(ElaSession *ws, int stream, int channel,
                        const void *data, size_t len, void *context) {
        if (!data || len < 1) return true;

        ChannelDataArgs* dataArgs = nullptr;
        HEAP_ALLOC(dataArgs, ChannelDataArgs, 1, true);
        HEAP_ALLOC(dataArgs->data, char, len, true);
        memcpy(dataArgs->data, data, len);
        dataArgs->len = len;
        dataArgs->id = channel;

        send_async_work(CHANNEL_DATA, getChannelDataArgs, dataArgs, context);
        return true;
    }

    //channel_open
    static size_t getChannelOpenArgs(napi_env env, void *args, napi_value* argv) {
        ChannelOpenArgs* channelArgs = (ChannelOpenArgs*)args;
        napi_create_string_utf8(env, channelArgs->cookie, NAPI_AUTO_LENGTH, &argv[0]);
        return 1;
    }

    bool channel_open(ElaSession *ws, int stream, int channel,
                        const char *cookie, void *context) {
        ChannelOpenArgs* channelArgs = nullptr;
        HEAP_ALLOC(channelArgs, ChannelOpenArgs, 1, true);
        if (cookie) strncpy(channelArgs->cookie, cookie, ELA_MAX_APP_MESSAGE_LEN);

        send_async_work(CHANNEL_OPEN, getChannelOpenArgs, channelArgs, context);
        return true;
    }

    //channel_close
    static size_t getChannelCloseArgs(napi_env env, void *args, napi_value* argv) {
        ChannelCloseArgs* channelArgs = (ChannelCloseArgs*)args;
        napi_create_int32(env, channelArgs->id, &argv[0]);
        napi_create_int32(env, channelArgs->reason, &argv[1]);
        return 2;
    }

    void channel_close(ElaSession *ws, int stream, int channel,
                        CloseReason reason, void *context) {
        ChannelCloseArgs* channelArgs = nullptr;
        HEAP_ALLOC_VOID(channelArgs, ChannelCloseArgs, 1);
        channelArgs->id = channel;
        channelArgs->reason = reason;

        send_async_work(CHANNEL_DATA, getChannelCloseArgs, channelArgs, context);
    }

    //channel_opened
    void channel_opened(ElaSession *ws, int stream, int channel, void *context) {
        setIntValueCallback(CHANNEL_OPENED, channel, context);
    }

    //channel_pending
    void channel_pending(ElaSession *ws, int stream, int channel, void *context) {
        setIntValueCallback(CHANNEL_PENDING, channel, context);
    }

    //channel_resume
    void channel_resume(ElaSession *ws, int stream, int channel, void *context) {
        setIntValueCallback(CHANNEL_RESUME, channel, context);
    }

//--------------------------------------------------------------------------------

    void setCarrierNativeCallbacks(ElaCallbacks* callbacks) {
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

    void setStreamNativeCallbacks(ElaStreamCallbacks* callbacks) {
        memset(callbacks, 0, sizeof(ElaStreamCallbacks));
        callbacks->state_changed = state_changed;
        callbacks->stream_data = stream_data;
        callbacks->channel_open = channel_open;
        callbacks->channel_opened = channel_opened;
        callbacks->channel_data = channel_data;
        callbacks->channel_pending = channel_pending;
        callbacks->channel_resume = channel_resume;
        callbacks->channel_close = channel_close;
    }

    void deleteCallbackHandle(napi_env env, int cb_no, Elca *elca) {
        if (elca->handle[cb_no]) {
            napi_delete_reference(env, elca->handle[cb_no]);
            elca->handle[cb_no] = nullptr;
        }

        elca->context[cb_no] = nullptr;
    }

    void setCallbackHandle(napi_env env, napi_value callback, napi_value context,
                                    int cb_no, Elca *elca) {
        napi_status status;
        napi_valuetype valuetype;

        if (!callback) return;

        status = napi_typeof(env, callback, &valuetype);
        CHECK_STATUS;

        deleteCallbackHandle(env, cb_no, elca);

        if (valuetype != napi_function) {
            return;
        };

        status = napi_create_reference(env, callback, 1, &elca->handle[cb_no]);
        CHECK_STATUS;

        elca->context[cb_no] = context;
    }

    static void setCallbackHandleByProperty(napi_env env, napi_value callbacks,
                                        napi_value context, const char* name, int cb_no, Elca *elca) {
        napi_status status;
        napi_value callback = nullptr;

        status = napi_get_named_property(env, callbacks, name, &callback);
        if(status != napi_ok || !callback) {
            return;
        }

        return setCallbackHandle(env, callback, context, cb_no, elca);
    }

    void setCallbackFuntions(napi_env env, napi_value callbacks, napi_value context, Elca *elca) {
        napi_status status;
        napi_valuetype valuetype;
        int count;

        status = napi_typeof(env, callbacks, &valuetype);
        CHECK_STATUS;

        if (valuetype != napi_object) return;

        const char** cb_name = getCallbackNamesAndCount(elca, &count);

        for (int i = 0; i < count; i++) {
            setCallbackHandleByProperty(env, callbacks, context, cb_name[i], i, elca);
        }
    }

//-----------------------------------------------------------------------


    static void setCallbackHandleByName(napi_env env, const char* name,
                                        napi_value callback, napi_value context, Elca *elca) {
        int count;
        const char** cb_name = getCallbackNamesAndCount(elca, &count);

        for (int i = 0; i < count; i++) {
            if (!strcmp(name, cb_name[i])) {
                return setCallbackHandle(env, callback, context, i, elca);
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
        CHECK_STATUS_AND_ARGC("on", 2, nullptr);
        CHECK_ELACARRIER_PTR(value_false);

        if (argc > 2) context = args[2];

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return nullptr;

        status = napi_get_value_string_utf8(env, args[0], name, MAX_EVENT_NAME_LEN, &len);
        if (status != napi_ok) return nullptr;

        setCallbackHandleByName(env, name, args[1], context, elca);
        return nullptr;
    }

    void createOnFunctions(napi_env env, napi_value object, Elca *elca) {
        napi_status status;
        napi_value fn;

        napi_create_function(env, nullptr, 0, elca_event_on, (void*)elca, &fn);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "on", NULL, 0, 0, 0, fn, napi_default, 0 },
        };

        status = napi_define_properties(env, object,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }

} // namespace elca
