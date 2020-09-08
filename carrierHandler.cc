#include "carrierHandler.h"
#include "carrierUtils.h"
#include "debug.h"
#include <string>
#include <uv.h>

#define ON_IDLE "onIdle"
#define ON_CONNECTION "onConnection"
#define ON_READY "onReady"
#define ON_SELF_INFO_CHANGED "onSelfInfoChanged"
#define ON_FRIEND_CONNECTION "onFriendConnection"
#define ON_FRIEND_INFO_CHANGED "onFriendInfoChanged"
#define ON_FRIEND_PRESENCE "onFriendPresence"
#define ON_FRIEND_REQUEST "onFriendRequest"
#define ON_FRIEND_ADDED "onFriendAdded"
#define ON_FRIEND_REMOVED "onFriendRemoved"
#define ON_FRIEND_MESSAGE "onFriendMessage"

typedef struct WorkerData {
    uv_work_t work;
    char *callbackName;
    HandlerContext *hc;
    ElaCarrier *carrier;
    ElaConnectionStatus elaConnectionStatus;
    char *friendId;
    char *userId;
    ElaUserInfo *elaUserInfo;
    ElaFriendInfo *elaFriendInfo;
    char *hello;
    char *msg;
    char *from;
    int64_t timestamp;
    bool offline;
    ElaPresenceStatus elaPresenceStatus;
} WorkerData;


void setStringParameter(napi_env env, char *value, napi_value *result) {
    napi_create_string_utf8(env, value, strlen(value), result);
}

void setIntParameter(napi_env env, int value, napi_value *result) {
    napi_create_int32(env, value, result);
}

void setInt64Parameter(napi_env env, int64_t value, napi_value *result) {
    napi_create_int64(env, value, result);
}

void setCarrierParameter(napi_env env, napi_ref *value, napi_value *result) {
    napi_get_reference_value(env, *value, result);
}

void onIdleWork(uv_work_t * work) {
    debug("onIdleWork");
}

void onIdleWorkAfter(uv_work_t * work, int state) {
    debug("onIdleWorkAfter");
}

void onIdle(ElaCarrier *carrier, void *context) {
    //debug("onIdle");
}

void onConnectionWork(uv_work_t * work) {
    debug("onConnectionWork");
}

void onConnectionWorkAfter(uv_work_t * work, int state) {
    debug("onConnectionWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    ElaConnectionStatus elaConnectionStatus = (ElaConnectionStatus)workerData->elaConnectionStatus;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onConnectionRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 2;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    setIntParameter(env, elaConnectionStatus, &argv[1]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function - onConnection");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onConnection(ElaCarrier *carrier, ElaConnectionStatus status, void *context) {
    debug("onConnection");
    
    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_CONNECTION;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->elaConnectionStatus = status;
    uv_queue_work(uv_default_loop(), &workerData->work, onConnectionWork, onConnectionWorkAfter);
}

void onReadyWork(uv_work_t * work) {
    debug("onReadyWork");
}

void onReadyWorkAfter(uv_work_t * work, int state) {
    debug("onReadyWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onReadyRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 1;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function - onReady");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onReady(ElaCarrier *carrier, void *context) {
    debug("onReady");

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_READY;
    workerData->hc = hc;
    workerData->carrier = carrier;
    uv_queue_work(uv_default_loop(), &workerData->work, onReadyWork, onReadyWorkAfter);
}

void onSelfInfoChangedWork(uv_work_t * work) {
    debug("onSelfInfoChangedWork");
}

void onSelfInfoChangedWorkAfter(uv_work_t * work, int state) {
    debug("onSelfInfoChangedWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    ElaUserInfo *userInfo = workerData->elaUserInfo;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onSelfInfoChangedRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 2;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    newUserInfo(env, userInfo, &argv[1]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function - onSelfInfo");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onSelfInfoChanged(ElaCarrier *carrier, const ElaUserInfo *info, void *context) {
    debug("onSelfInfoChanged");

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_SELF_INFO_CHANGED;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->elaUserInfo = (ElaUserInfo*)calloc(1, sizeof(ElaUserInfo) * 1);
    *workerData->elaUserInfo = *info;
    uv_queue_work(uv_default_loop(), &workerData->work, onSelfInfoChangedWork, onSelfInfoChangedWorkAfter);
}

void onFriendConnectionWork(uv_work_t * work) {
    debug("onFriendConnectionWork");
}

void onFriendConnectionWorkAfter(uv_work_t * work, int state) {
    debug("onFriendConnectionWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    char *friendId = workerData->friendId;
    ElaConnectionStatus elaConnectionStatus = (ElaConnectionStatus)workerData->elaConnectionStatus;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onFriendConnectionRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 3;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    setStringParameter(env, friendId, &argv[1]);
    setIntParameter(env, elaConnectionStatus, &argv[2]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function - onFriendConnection");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onFriendConnection(ElaCarrier *carrier, const char *friendid, ElaConnectionStatus status, void *context) {
    debug("onFriendConnection");

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_FRIEND_CONNECTION;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->elaConnectionStatus = status;
    workerData->friendId = (char*)calloc(1, sizeof(char) * ELA_MAX_ID_LEN + 1);
    strncpy(workerData->friendId, friendid, ELA_MAX_ID_LEN);
    uv_queue_work(uv_default_loop(), &workerData->work, onFriendConnectionWork, onFriendConnectionWorkAfter);
}

void onFriendInfoChangedWork(uv_work_t * work) {
    debug("onFriendInfoChangedWork");
}

void onFriendInfoChangedWorkAfter(uv_work_t * work, int state) {
    debug("onFriendInfoChangedWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    char *friendId = workerData->friendId;
    ElaFriendInfo *friendInfo = workerData->elaFriendInfo;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onFriendInfoChangedRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 3;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    setStringParameter(env, friendId, &argv[1]);
    newFriendInfo(env, friendInfo, &argv[2]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function - onFriendInfo");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onFriendInfoChanged(ElaCarrier *carrier, const char *friendid, const ElaFriendInfo *info, void *context) {
    debug("onFriendInfoChanged");

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_FRIEND_INFO_CHANGED;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->friendId = (char*)calloc(1, sizeof(char) * ELA_MAX_ID_LEN + 1);
    strncpy(workerData->friendId, friendid, ELA_MAX_ID_LEN);
    workerData->elaFriendInfo = (ElaFriendInfo*)calloc(1, sizeof(ElaFriendInfo) * 1);
    *workerData->elaFriendInfo = *info;
    uv_queue_work(uv_default_loop(), &workerData->work, onFriendInfoChangedWork, onFriendInfoChangedWorkAfter);
}

void onFriendPresenceWork(uv_work_t * work) {
    debug("onFriendPresenceWork");
}

void onFriendPresenceWorkAfter(uv_work_t * work, int state) {
    debug("onFriendPresenceWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    char *friendId = workerData->friendId;
    ElaPresenceStatus elaPresenceStatus = (ElaPresenceStatus)workerData->elaPresenceStatus;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onFriendPresenceRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 3;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    setStringParameter(env, friendId, &argv[1]);
    setIntParameter(env, elaPresenceStatus, &argv[2]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function  - onFriendPresence");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onFriendPresence(ElaCarrier *carrier, const char *friendid, ElaPresenceStatus presence, void *context) {
    debug("onFriendPresence");

    cout<<"newPresence: "<<presence<<endl;

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_FRIEND_PRESENCE;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->friendId = (char*)calloc(1, sizeof(char) * ELA_MAX_ID_LEN + 1);
    strncpy(workerData->friendId, friendid, ELA_MAX_ID_LEN);
    workerData->elaPresenceStatus = presence;
    uv_queue_work(uv_default_loop(), &workerData->work, onFriendPresenceWork, onFriendPresenceWorkAfter);
}

void onFriendRequestWork(uv_work_t * work) {
    debug("onFriendRequestWork");
}

void onFriendRequestWorkAfter(uv_work_t * work, int state) {
    debug("onFriendRequestWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    char *userId = workerData->userId;
    ElaUserInfo *userInfo = workerData->elaUserInfo;
    char *hello = workerData->hello;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onFriendRequestRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 4;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    setStringParameter(env, userId, &argv[1]);
    newUserInfo(env, userInfo, &argv[2]);
    setStringParameter(env, hello, &argv[3]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function - onFriendRequest");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onFriendRequest(ElaCarrier *carrier, const char *userid, const ElaUserInfo *info, const char *hello, void *context) {
    debug("onFriendRequest");

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_FRIEND_CONNECTION;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->userId = (char*)calloc(1, sizeof(char) * ELA_MAX_ID_LEN + 1);
    strncpy(workerData->userId, userid, ELA_MAX_ID_LEN);
    workerData->elaUserInfo = (ElaUserInfo*)calloc(1, sizeof(ElaUserInfo) * 1);
    *workerData->elaUserInfo = *info;
    workerData->hello = (char*)calloc(1, sizeof(char) * ELA_MAX_APP_MESSAGE_LEN + 1);
    strncpy(workerData->hello, hello, ELA_MAX_APP_MESSAGE_LEN);
    uv_queue_work(uv_default_loop(), &workerData->work, onFriendRequestWork, onFriendRequestWorkAfter);
}

void onFriendAddedWork(uv_work_t * work) {
    debug("onFriendAddedWork");
}

void onFriendAddedWorkAfter(uv_work_t * work, int state) {
    debug("onFriendAddedWorkAfter");
    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    ElaFriendInfo *friendInfo = (ElaFriendInfo*)workerData->elaFriendInfo;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onFriendAddedRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 2;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    newFriendInfo(env, friendInfo, &argv[1]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function - onFriendAdded");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onFriendAdded(ElaCarrier *carrier, const ElaFriendInfo *info, void *context) {
    debug("onFriendAdded");

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_FRIEND_ADDED;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->elaFriendInfo = (ElaFriendInfo*)calloc(1, sizeof(ElaFriendInfo) * 1);
    *workerData->elaFriendInfo = *info;
    uv_queue_work(uv_default_loop(), &workerData->work, onFriendAddedWork, onFriendAddedWorkAfter);
}

void onFriendRemovedWork(uv_work_t * work) {
    debug("onFriendRemovedWork");
}

void onFriendRemovedWorkAfter(uv_work_t * work, int state) {
    debug("onFriendRemovedWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    char *friendId = (char*)workerData->friendId;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onFriendRemovedRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 2;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    setStringParameter(env, friendId, &argv[1]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function  - onFriendRemoved");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onFriendRemoved(ElaCarrier *carrier, const char *friendid, void *context) {
    debug("onFriendRemoved");

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_FRIEND_REMOVED;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->friendId = (char*)calloc(1, sizeof(char) * ELA_MAX_ID_LEN + 1);
    strncpy(workerData->friendId, friendid, ELA_MAX_ID_LEN);
    uv_queue_work(uv_default_loop(), &workerData->work, onFriendRemovedWork, onFriendRemovedWorkAfter);
}

void onFriendMessageWork(uv_work_t * work) {
    debug("onFriendMessageWork");
}

void onFriendMessageWorkAfter(uv_work_t * work, int state) {
    debug("onFriendMessageWorkAfter");

    napi_status status;
    WorkerData *workerData = (WorkerData*)work->data;
    HandlerContext *hc = workerData->hc;
    napi_env env = hc->env;
    ElaCarrier *carrier = (ElaCarrier*)workerData->carrier;
    char *from = (char*)workerData->from;
    char *msg = (char*)workerData->msg;
    int64_t timestamp = (int64_t)workerData->timestamp;
    bool offline = (bool)workerData->offline;
    
    napi_handle_scope scope = nullptr;
    status = napi_open_handle_scope(env, &scope);
    statusCheck(status, "napi_open_handle_scope");

    napi_value global;
    napi_value callbackFn;
    status = napi_get_reference_value(env, hc->onFriendMessageRef, &callbackFn);
    statusCheck(status, "napi_get_reference_value");

    const size_t argc = 5;
    napi_value argv[argc];
    
    setCarrierParameter(env, &hc->object, &argv[0]);
    setStringParameter(env, from, &argv[1]);
    setStringParameter(env, msg, &argv[2]);
    setInt64Parameter(env, timestamp, &argv[3]);
    setIntParameter(env, offline, &argv[4]);

    status = napi_call_function(env, global, callbackFn, argc, argv, nullptr);
    statusCheck(status, "napi_call_function - onFriendMessage");

    status = napi_close_handle_scope(env, scope);
    statusCheck(status, "napi_close_handle_scope");
}

void onFriendMessage(ElaCarrier *carrier, const char *from, const void *msg, size_t len, int64_t timestamp, bool offline, void *context) {
    debug("onFriendMessage");

    WorkerData *workerData = (WorkerData*)calloc(1, sizeof(WorkerData) * 1);
    workerData->work.data = workerData;
    HandlerContext *hc = (HandlerContext*)context;
    workerData->callbackName = ON_FRIEND_MESSAGE;
    workerData->hc = hc;
    workerData->carrier = carrier;
    workerData->from = (char*)calloc(1, sizeof(char) * ELA_MAX_ID_LEN + 1);
    strncpy(workerData->from, from, ELA_MAX_ID_LEN);
    workerData->msg = (char*)calloc(1, sizeof(char) * len + 1);
    strncpy(workerData->msg, (char*)msg, len);
    workerData->timestamp = timestamp;
    workerData->offline = offline;
    uv_queue_work(uv_default_loop(), &workerData->work, onFriendMessageWork, onFriendMessageWorkAfter);
}

void createCallbackReference(napi_env env, napi_value jsCallbacks, char *callbackName, napi_ref *callbackRef) {
    debug("createCallbackReference");
    napi_status status;
    napi_value callback = nullptr;
    status = napi_get_named_property(env, jsCallbacks, callbackName, &callback);
    statusCheck(status, "napi_get_named_property");

    status = napi_create_reference(env, callback, 1, callbackRef);
    statusCheck(status, "napi_create_reference");
}

void setCarrierCallbacks(napi_env env, napi_value jsCallbacks, ElaCallbacks *callbacks, HandlerContext *hc) {
    debug("setCarrierCallbacks");

    //createCallbackReference(env, jsCallbacks, ON_IDLE, &hc->onIdleRef);
    createCallbackReference(env, jsCallbacks, ON_CONNECTION, &hc->onConnectionRef);
    createCallbackReference(env, jsCallbacks, ON_READY, &hc->onReadyRef);
    createCallbackReference(env, jsCallbacks, ON_SELF_INFO_CHANGED, &hc->onSelfInfoChangedRef);
    createCallbackReference(env, jsCallbacks, ON_FRIEND_CONNECTION, &hc->onFriendConnectionRef);
    createCallbackReference(env, jsCallbacks, ON_FRIEND_INFO_CHANGED, &hc->onFriendInfoChangedRef);
    createCallbackReference(env, jsCallbacks, ON_FRIEND_PRESENCE, &hc->onFriendPresenceRef);
    createCallbackReference(env, jsCallbacks, ON_FRIEND_REQUEST, &hc->onFriendRequestRef);
    createCallbackReference(env, jsCallbacks, ON_FRIEND_ADDED, &hc->onFriendAddedRef);
    createCallbackReference(env, jsCallbacks, ON_FRIEND_REMOVED, &hc->onFriendRemovedRef);
    createCallbackReference(env, jsCallbacks, ON_FRIEND_MESSAGE, &hc->onFriendMessageRef);

    memset(callbacks, 0, sizeof(ElaCallbacks));
    //callbacks->idle = onIdle;
    callbacks->connection_status = onConnection;
    callbacks->ready = onReady;
    callbacks->self_info = onSelfInfoChanged;
    callbacks->friend_connection = onFriendConnection;
    callbacks->friend_info = onFriendInfoChanged;
    callbacks->friend_presence = onFriendPresence;
    callbacks->friend_request = onFriendRequest;
    callbacks->friend_added = onFriendAdded;
    callbacks->friend_removed = onFriendRemoved;
    callbacks->friend_message = onFriendMessage;

}