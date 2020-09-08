#include <node_api.h>
#include <iostream>
#include <uv.h>
#include "include/ela_carrier.h"
#include "carrierHandler.h"
#include "carrierUtils.h"
#include "debug.h"

#define DECLARE_NAPI_METHOD(name, func) \
{ name, 0, func, 0, 0, 0, napi_default, 0 }

void start_work(uv_work_t * work) {
    debug("start_work");
    HandlerContext *hc = (HandlerContext*)work->data;
    int rc = ela_run(hc->nativeCarrier, hc->interval);
    if (rc < 0) {
        debug("Call ela_run API error");
    }
}

void start_work_after(uv_work_t * work, int status) {
    debug("start_work_after");
    if (work) {
        free(work);
    }
}

napi_value start(napi_env env, napi_callback_info info) {
    debug("start");
    napi_status status;
    napi_value value = nullptr;

    size_t argc = 1;
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    napi_value jsInterval = argv[0];
    int32_t interval;
    status = napi_get_value_int32(env, jsInterval, &interval);
    cout << "interval: " << interval << endl;

    HandlerContext *hc = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&hc);

    uv_work_t *work = (uv_work_t*)calloc(1, sizeof(uv_work_t) * 1);
    hc->interval = interval;
    work->data = hc;
    uv_queue_work(uv_default_loop(), work, start_work, start_work_after);

    return value;
}

napi_value kill(napi_env env, napi_callback_info info) {
    debug("kill");

    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&hc);

    ela_kill(hc->nativeCarrier);

    return value;
}

napi_value getAddress(napi_env env, napi_callback_info info) {
    debug("getAddress");

    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&hc);

    char addrBuf[ELA_MAX_ADDRESS_LEN + 1];
    char *address = NULL;
    address = ela_get_address(hc->nativeCarrier, addrBuf, sizeof(addrBuf));
    if (!address) {
        debug("Call ela_get_address API error");
        return NULL;
    }

    status = napi_create_string_utf8(env, address, strlen(address), &value);
    if (status != napi_ok) {
        debug("Can not convert string");
        return NULL;
    }
    return value;
}

napi_value getNodeId(napi_env env, napi_callback_info info) {
    debug("getNodeId");

    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&hc);

    char nodeIdBuf[ELA_MAX_ADDRESS_LEN + 1];
    char *nodeId = NULL;
    nodeId = ela_get_nodeid(hc->nativeCarrier, nodeIdBuf, sizeof(nodeIdBuf));
    if (!nodeId) {
        debug("Call ela_get_nodeid API error");
        return NULL;
    }

    status = napi_create_string_utf8(env, nodeId, strlen(nodeId), &value);
    if (status != napi_ok) {
        debug("Can not convert string");
        return NULL;
    }
    return value;
}

napi_value getUserId(napi_env env, napi_callback_info info) {
    debug("getUserId");
    napi_value value = getNodeId(env, info);
    return value;
}

//TODO: implement
napi_value setNospam(napi_env env, napi_callback_info info) {
    debug("setNospam");
    napi_status status;
    napi_value value = nullptr;

    status = napi_create_int32(env, 0, &value);
    return value;
}

//TODO: implement
napi_value getNospam(napi_env env, napi_callback_info info) {
    debug("getNospam");
    napi_status status;
    napi_value value = nullptr;

    status = napi_create_int32(env, 0, &value);
    return value;
}

napi_value setSelfInfo(napi_env env, napi_callback_info info) {
    debug("setSelfInfo");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    size_t argc = 1;
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);

    ElaUserInfo ui;
    int rc;

    if (!getNativeUserInfo(env, info, argv[0], &ui)) {
        debug("Construct C-structured USerInfo object error");
        status = napi_create_int32(env, 0, &value);
        return value;
    }

    rc = ela_set_self_info(hc->nativeCarrier, &ui);
    if (rc < 0) {
        debug("Call ela_set_self_info API error");
        status = napi_create_int32(env, 0, &value);
        return value;
    }

    status = napi_create_int32(env, 1, &value);
    return value;
}

napi_value getSelfInfo(napi_env env, napi_callback_info info) {
    debug("getSelfInfo");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&hc);

    ElaUserInfo ui;
    int rc;
    rc = ela_get_self_info(hc->nativeCarrier, &ui);
    if (rc < 0) {
        debug("Call ela_get_self_info API error");
        return NULL;
    }

    if (!newUserInfo(env, &ui, &value)) {
        debug("Construct java UserInfo object error");
        return NULL;
    }

    return value;
}

napi_value setPresence(napi_env env, napi_callback_info info) {
    debug("setPresence");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    size_t argc = 1;
    napi_value argv[1];
    int32_t presenceValue;
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_int32(env, argv[0], &presenceValue);

    ElaPresenceStatus presence = (ElaPresenceStatus)presenceValue;
    int rc;

    rc = ela_set_self_presence(hc->nativeCarrier, presence);
    if (rc < 0) {
        status = napi_create_int32(env, 0, &value);
        return value;
    }

    status = napi_create_int32(env, 1, &value);
    return value;
}

napi_value getPresence(napi_env env, napi_callback_info info) {
    debug("getPresence");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&hc);

    ElaPresenceStatus presence;
    int rc;
    
    rc = ela_get_self_presence(hc->nativeCarrier, &presence);
    if (rc < 0) {
        status = napi_create_int32(env, -1, &value);
        return value;
    }

    status = napi_create_int32(env, presence, &value);
    return value;
}

napi_value isReady(napi_env env, napi_callback_info info) {
    debug("isReady");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&hc);

    bool isReady = ela_is_ready(hc->nativeCarrier);

    status = napi_create_int32(env, isReady, &value);
    return value;
}

//TODO: implement
napi_value getFriends(napi_env env, napi_callback_info info) {
    debug("getFriends");
    napi_status status;
    napi_value value = nullptr;

    status = napi_create_int32(env, 0, &value);
    return value;
}

napi_value getFriend(napi_env env, napi_callback_info info) {
    debug("getFriend");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    char friendId[ELA_MAX_ID_LEN + 1];
    ElaFriendInfo friendInfo;
    int rc;

    size_t size;
    size_t argc = 1;
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_string_utf8(env, argv[0], friendId, ELA_MAX_ID_LEN + 1, &size);

    rc = ela_get_friend_info(hc->nativeCarrier, friendId, &friendInfo);
    if (rc < 0) {
        status = napi_create_int32(env, 0, &value);
        return value;
    }

    newFriendInfo(env, &friendInfo, &value);
    return value;
}

napi_value labelFriend(napi_env env, napi_callback_info info) {
    debug("labelFriend");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    char friendId[ELA_MAX_ID_LEN + 1];
    char label[ELA_MAX_USER_NAME_LEN + 1];
    int rc;

    size_t size;
    size_t argc = 2;
    napi_value argv[2];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_string_utf8(env, argv[0], friendId, ELA_MAX_ID_LEN + 1, &size);
    status = napi_get_value_string_utf8(env, argv[1], label, ELA_MAX_USER_NAME_LEN + 1, &size);

    rc = ela_set_friend_label(hc->nativeCarrier, friendId, label);
    if (rc < 0) {
        status = napi_create_int32(env, 0, &value);
        return value;
    }

    status = napi_create_int32(env, 1, &value);
    return value;
}

napi_value isFriend(napi_env env, napi_callback_info info) {
    debug("isFriend");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    char userId[ELA_MAX_ID_LEN + 1];
    int rc;

    size_t size;
    size_t argc = 1;
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_string_utf8(env, argv[0], userId, ELA_MAX_ID_LEN + 1, &size);

    rc = ela_is_friend(hc->nativeCarrier, userId);
    status = napi_create_int32(env, rc, &value);

    return value;
}

napi_value addFriend(napi_env env, napi_callback_info info) {
    debug("addFriend");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    char address[ELA_MAX_ADDRESS_LEN + 1];
    char hello[ELA_MAX_APP_MESSAGE_LEN + 1];

    size_t size;
    size_t argc = 2;
    napi_value argv[2];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_string_utf8(env, argv[0], address, ELA_MAX_ADDRESS_LEN + 1, &size);
    status = napi_get_value_string_utf8(env, argv[1], hello, ELA_MAX_APP_MESSAGE_LEN + 1, &size);
    
    int rc;
    rc = ela_add_friend(hc->nativeCarrier, address, hello);
    
    if (rc < 0) {
        debug("Call ela_add_friend API_error");
        status = napi_create_int32(env, 0, &value);
        return value;
    }

    status = napi_create_int32(env, 1, &value);
    return value;
}

napi_value acceptFriend(napi_env env, napi_callback_info info) {
    debug("acceptFriend");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    char userId[ELA_MAX_ID_LEN + 1];

    size_t size;
    size_t argc = 1;
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_string_utf8(env, argv[0], userId, ELA_MAX_ID_LEN + 1, &size);

    int rc;
    rc = ela_accept_friend(hc->nativeCarrier, userId);
    if (rc < 0) {
        debug("Call ela_accept_friend API error");
        status = napi_create_int32(env, 0, &value);
        return value;
    }

    status = napi_create_int32(env, 1, &value);
    return value;
}

napi_value removeFriend(napi_env env, napi_callback_info info) {
    debug("removeFriend");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    char friendId[ELA_MAX_ID_LEN + 1];

    size_t size;
    size_t argc = 1;
    napi_value argv[1];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_string_utf8(env, argv[0], friendId, ELA_MAX_ID_LEN + 1, &size);

    int rc;
    rc = ela_remove_friend(hc->nativeCarrier, friendId);
    if (rc < 0) {
        debug("Call ela_remove_friend API error");
        status = napi_create_int32(env, 0, &value);
        return value;
    }

    status = napi_create_int32(env, 1, &value);
    return value;
}

napi_value sendFriendMessage(napi_env env, napi_callback_info info) {
    debug("sendFriendMessage");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    char to[ELA_MAX_ID_LEN + 1];
    char msg[ELA_MAX_APP_MESSAGE_LEN + 1];
    bool isOffline;

    size_t size;
    size_t argc = 2;
    napi_value argv[2];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_string_utf8(env, argv[0], to, ELA_MAX_ID_LEN + 1, &size);
    status = napi_get_value_string_utf8(env, argv[1], msg, ELA_MAX_APP_MESSAGE_LEN + 1, &size);

    int rc;
    rc = ela_send_friend_message(hc->nativeCarrier, to, msg, size, &isOffline);
    if (rc < 0) {
        status = napi_create_int32(env, -1, &value);
    }

    if (isOffline) {
        status = napi_create_int32(env, 1, &value);
    }
    else {
        status = napi_create_int32(env, 0, &value);
    }
    
    return value;
}

napi_value isValidAddress(napi_env env, napi_callback_info info) {
    debug("isValidAddress");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    char address[ELA_MAX_ADDRESS_LEN + 1];

    size_t size;
    size_t argc = 2;
    napi_value argv[2];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, (void**)&hc);
    status = napi_get_value_string_utf8(env, argv[0], address, ELA_MAX_ADDRESS_LEN + 1, &size);

    bool isValid = ela_address_is_valid(address);
    status = napi_create_int32(env, isValid, &value);

    return value;
}

napi_value getVersion(napi_env env, napi_callback_info info) {
    debug("getVersion");
    napi_status status;
    napi_value value = nullptr;

    HandlerContext *hc = nullptr;
    status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, (void**)&hc);

    const char *version = ela_get_version();
    napi_create_string_utf8(env, version, NAPI_AUTO_LENGTH, &value);
    
    return value;
}

void registerInstanceMethods(napi_env env, napi_value carrier, HandlerContext *hc) {
    debug("registerInstanceMethods");
    napi_status status;
    const size_t methodCount = 22;
    napi_value fn[methodCount];
    memset(fn, 0, sizeof(napi_value) * methodCount);

    napi_create_function(env, nullptr, 0, start, (void*)hc, &fn[0]);
    napi_create_function(env, nullptr, 0, kill, (void*)hc, &fn[1]);
    napi_create_function(env, nullptr, 0, getAddress, (void*)hc, &fn[2]);
    napi_create_function(env, nullptr, 0, getNodeId, (void*)hc, &fn[3]);
    napi_create_function(env, nullptr, 0, getUserId, (void*)hc, &fn[4]);
    napi_create_function(env, nullptr, 0, setNospam, (void*)hc, &fn[5]);
    napi_create_function(env, nullptr, 0, getNospam, (void*)hc, &fn[6]);
    napi_create_function(env, nullptr, 0, setSelfInfo, (void*)hc, &fn[7]);
    napi_create_function(env, nullptr, 0, getSelfInfo, (void*)hc, &fn[8]);
    napi_create_function(env, nullptr, 0, setPresence, (void*)hc, &fn[9]);
    napi_create_function(env, nullptr, 0, getPresence, (void*)hc, &fn[10]);
    napi_create_function(env, nullptr, 0, isReady, (void*)hc, &fn[11]);
    napi_create_function(env, nullptr, 0, getFriends, (void*)hc, &fn[12]);
    napi_create_function(env, nullptr, 0, getFriend, (void*)hc, &fn[13]);
    napi_create_function(env, nullptr, 0, labelFriend, (void*)hc, &fn[14]);
    napi_create_function(env, nullptr, 0, isFriend, (void*)hc, &fn[15]);
    napi_create_function(env, nullptr, 0, addFriend, (void*)hc, &fn[16]);
    napi_create_function(env, nullptr, 0, acceptFriend, (void*)hc, &fn[17]);
    napi_create_function(env, nullptr, 0, removeFriend, (void*)hc, &fn[18]);
    napi_create_function(env, nullptr, 0, sendFriendMessage, (void*)hc, &fn[19]);
    napi_create_function(env, nullptr, 0, isValidAddress, (void*)hc, &fn[20]);
    napi_create_function(env, nullptr, 0, getVersion, (void*)hc, &fn[21]);

    napi_property_descriptor properties[] = {
        {"start", NULL, 0, 0, 0, fn[0], napi_default, 0},
        {"kill", NULL, 0, 0, 0, fn[1], napi_default, 0},
        {"getAddress", NULL, 0, 0, 0, fn[2], napi_default, 0},
        {"getNodeId", NULL, 0, 0, 0, fn[3], napi_default, 0},
        {"getUserId", NULL, 0, 0, 0, fn[4], napi_default, 0},
        {"setNospam", NULL, 0, 0, 0, fn[5], napi_default, 0},
        {"getNospam", NULL, 0, 0, 0, fn[6], napi_default, 0},
        {"setSelfInfo", NULL, 0, 0, 0, fn[7], napi_default, 0},
        {"getSelfInfo", NULL, 0, 0, 0, fn[8], napi_default, 0},
        {"setPresence", NULL, 0, 0, 0, fn[9], napi_default, 0},
        {"getPresence", NULL, 0, 0, 0, fn[10], napi_default, 0},
        {"isReady", NULL, 0, 0, 0, fn[11], napi_default, 0},
        {"getFriends", NULL, 0, 0, 0, fn[12], napi_default, 0},
        {"getFriend", NULL, 0, 0, 0, fn[13], napi_default, 0},
        {"labelFriend", NULL, 0, 0, 0, fn[14], napi_default, 0},
        {"isFriend", NULL, 0, 0, 0, fn[15], napi_default, 0},
        {"addFriend", NULL, 0, 0, 0, fn[16], napi_default, 0},
        {"acceptFriend", NULL, 0, 0, 0, fn[17], napi_default, 0},
        {"removeFriend", NULL, 0, 0, 0, fn[18], napi_default, 0},
        {"sendFriendMessage", NULL, 0, 0, 0, fn[19], napi_default, 0},
        {"isValidAddress", NULL, 0, 0, 0, fn[20], napi_default, 0},
        {"getVersion", NULL, 0, 0, 0, fn[21], napi_default, 0}
    };

    status = napi_define_properties(env, carrier, methodCount, properties);
    statusCheck(status, "napi_define_properties");
}

napi_value createCarrierInstance(napi_env env, napi_callback_info info) {
    debug("createCarrierInstance");

    napi_status status;
    napi_value object;

    OptionsHelper helper;
    ElaCarrier *carrier;
    HandlerContext *hc;

    memset(&helper, 0, sizeof(helper));
    hc = (HandlerContext*)malloc(sizeof(HandlerContext));
    memset(hc, 0, sizeof(*hc));

    size_t argc = 2;
    napi_value argv[2];
    status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    napi_value jsOptions = argv[0];
    napi_value jsCallbacks = argv[1];

    getOptionsHelper(env, info, jsOptions, &helper);
    
    ElaOptions *opts = new ElaOptions();
    opts->udp_enabled = helper.udp_enabled;
    opts->persistent_location = helper.persistent_location;
    opts->secret_key = helper.secret_key;
    opts->bootstraps_size = helper.bootstraps_size;
    opts->bootstraps = (BootstrapNode *)helper.bootstraps;
    opts->express_nodes_size = helper.express_nodes_size;
    opts->express_nodes = (ExpressNode *)helper.express_nodes;
    opts->log_level = ElaLogLevel_Info;

    ElaCallbacks callbacks;
    setCarrierCallbacks(env, jsCallbacks, &callbacks, hc);

    carrier = ela_new(opts, &callbacks, hc);
    
    status = napi_create_object(env, &object);
    statusCheck(status, "napi_create_object");

    hc->nativeCarrier = carrier;
    hc->env = env;

    status = napi_create_reference(env, object, 1, &hc->object);
    statusCheck(status, "napi_create_reference");

    registerInstanceMethods(env, object, hc);

    return object;
}

int registerCarrierMethods(napi_env env, napi_value *exports) {
    debug("registerCarrierMethods");
    napi_status status;
    napi_property_descriptor property = DECLARE_NAPI_METHOD("createCarrierInstance", createCarrierInstance);
    status = napi_define_properties(env, *exports, 1, &property);
    statusCheck(status, "napi_define_properties");
    return 0;
}