#include <node_api.h>
#include "carrierUtils.h"
#include "debug.h"
#include "include/ela_carrier.h"

char * getPropertyStringValue(napi_env env, napi_value element, char *propertyName) {
    napi_value value = nullptr;
    napi_get_named_property(env, element, propertyName, &value);
    char * stringValue = new char[ELA_MAX_APP_MESSAGE_LEN];
    napi_get_value_string_utf8(env, value, stringValue, ELA_MAX_APP_MESSAGE_LEN, NULL);
    return stringValue;
}

char * getPropertyStringValueByLen(napi_env env, napi_value element, char *propertyName, char *buf, size_t len) {
    napi_status status;
    napi_value val;
    size_t size;

    status = napi_get_named_property(env, element, propertyName, &val);
    if (status == napi_ok) {
        status = napi_get_value_string_utf8(env, val, buf, len, &size);
        if (status == napi_ok) {
            return buf;
        }
    }
    return nullptr;
}

bool getPropertyBoolValue(napi_env env, napi_value element, char *propertyName) {
    napi_value value = nullptr;
    napi_get_named_property(env, element, propertyName, &value);
    bool boolValue;
    napi_get_value_bool(env, value, &boolValue);
    return boolValue;
}

napi_value getPropertyArray(napi_env env, napi_value element, char *propertyName) {
    napi_value value = nullptr;
    napi_get_named_property(env, element, propertyName, &value);
    return value;
}

uint32_t getPropertyArraySize(napi_env env, napi_value element) {
    uint32_t value;
    napi_get_array_length(env, element, &value);
    return value;
}

napi_value getPropertyElement(napi_env env, napi_value element, uint32_t index) {
    napi_value value = nullptr;
    napi_get_element(env, element, index, &value);
    return value;
}

int getOptionsHelper(napi_env env, napi_callback_info info, napi_value options, OptionsHelper *opts) {
    debug("getOptionsHelper");

    opts->udp_enabled = getPropertyBoolValue(env, options, "udpEnabled");
    opts->persistent_location = getPropertyStringValue(env, options, "persistentLocation");
    opts->secret_key = nullptr;

    napi_value bootstraps = getPropertyArray(env, options, "bootstraps");
    uint32_t bootstrapsSize = getPropertyArraySize(env, bootstraps);
    opts->bootstraps_size = (size_t)bootstrapsSize;
    opts->bootstraps = (BootstrapHelper *)calloc(1, sizeof(BootstrapHelper) * bootstrapsSize);
    for (int i = 0; i < (int)bootstrapsSize; i++) {
        napi_value element = getPropertyElement(env, bootstraps, i);
        opts->bootstraps[i].ipv4 = getPropertyStringValue(env, element, "ipv4");
        opts->bootstraps[i].port = getPropertyStringValue(env, element, "port");
        opts->bootstraps[i].public_key = getPropertyStringValue(env, element, "publicKey");
    }

    napi_value expressNodes = getPropertyArray(env, options, "expressNodes");
    uint32_t expressNodesSize = getPropertyArraySize(env, expressNodes);
    opts->express_nodes_size = (size_t)expressNodesSize;
    opts->express_nodes = (ExpressNodeHelper *)calloc(1, sizeof(ExpressNodeHelper) * expressNodesSize);
    for (int i = 0; i < (int)expressNodesSize; i++) {
        napi_value element = getPropertyElement(env, expressNodes, i);
        opts->express_nodes[i].ipv4 = getPropertyStringValue(env, element, "ipv4");
        opts->express_nodes[i].port = getPropertyStringValue(env, element, "port");
        opts->express_nodes[i].public_key = getPropertyStringValue(env, element, "publicKey");
    }

    return 1;
}

int getNativeUserInfo(napi_env env, napi_callback_info info, napi_value jUserInfo, ElaUserInfo *userInfo) {
    debug("getNativeUserInfo");

    userInfo->has_avatar = getPropertyBoolValue(env, jUserInfo, "hasAvatar");
    getPropertyStringValueByLen(env, jUserInfo, "userId", userInfo->userid, ELA_MAX_ID_LEN + 1);
    getPropertyStringValueByLen(env, jUserInfo, "name", userInfo->name, ELA_MAX_USER_NAME_LEN + 1);
    getPropertyStringValueByLen(env, jUserInfo, "description", userInfo->description, ELA_MAX_USER_DESCRIPTION_LEN + 1);
    getPropertyStringValueByLen(env, jUserInfo, "gender", userInfo->gender, ELA_MAX_GENDER_LEN + 1);
    getPropertyStringValueByLen(env, jUserInfo, "phone", userInfo->phone, ELA_MAX_PHONE_LEN + 1);
    getPropertyStringValueByLen(env, jUserInfo, "email", userInfo->email, ELA_MAX_EMAIL_LEN + 1);
    getPropertyStringValueByLen(env, jUserInfo, "region", userInfo->region, ELA_MAX_REGION_LEN + 1);

    return 1;
}

int newUserInfo(napi_env env, ElaUserInfo *userInfo, napi_value *jUserInfo) {
    debug("newUserInfo");
    napi_value userId, name, description, hasAvatar, gender, phone, email, region;
    napi_status status;

    status = napi_create_string_utf8(env, userInfo->userid, NAPI_AUTO_LENGTH, &userId);
    status = napi_create_string_utf8(env, userInfo->name, NAPI_AUTO_LENGTH, &name);
    status = napi_create_string_utf8(env, userInfo->description, NAPI_AUTO_LENGTH, &description);
    status = napi_create_int32(env, userInfo->has_avatar, &hasAvatar);
    status = napi_create_string_utf8(env, userInfo->gender, NAPI_AUTO_LENGTH, &gender);
    status = napi_create_string_utf8(env, userInfo->phone, NAPI_AUTO_LENGTH, &phone);
    status = napi_create_string_utf8(env, userInfo->email, NAPI_AUTO_LENGTH, &email);
    status = napi_create_string_utf8(env, userInfo->region, NAPI_AUTO_LENGTH, &region);

    status = napi_create_object(env, jUserInfo);

    napi_property_descriptor properties[] = {
        { "userId", NULL, 0, 0, 0, userId, napi_default, 0 },
        { "name", NULL, 0, 0, 0, name, napi_writable, 0 },
        { "description", NULL, 0, 0, 0, description, napi_writable, 0 },
        { "hasAvatar", NULL, 0, 0, 0, hasAvatar, napi_default, 0 },
        { "gender", NULL, 0, 0, 0, gender, napi_writable, 0 },
        { "phone", NULL, 0, 0, 0, phone, napi_writable, 0 },
        { "email", NULL, 0, 0, 0, email, napi_writable, 0 },
        { "region", NULL, 0, 0, 0, region, napi_writable, 0 }
    };

    status = napi_define_properties(env, *jUserInfo, sizeof(properties) / sizeof(*properties), properties);
    if (status != napi_ok) {
        return 0;
    }
    return 1;
}

int newFriendInfo(napi_env env, ElaFriendInfo *friendInfo, napi_value *jFriendInfo) {
    debug("newFriendInfo");

    napi_value userId, name, description, hasAvatar, gender, phone, email, region, presence, connectionStatus, label;
    napi_status status;

    status = napi_create_string_utf8(env, friendInfo->user_info.userid, NAPI_AUTO_LENGTH, &userId);
    status = napi_create_string_utf8(env, friendInfo->user_info.name, NAPI_AUTO_LENGTH, &name);
    status = napi_create_string_utf8(env, friendInfo->user_info.description, NAPI_AUTO_LENGTH, &description);
    status = napi_create_int32(env, friendInfo->user_info.has_avatar, &hasAvatar);
    status = napi_create_string_utf8(env, friendInfo->user_info.gender, NAPI_AUTO_LENGTH, &gender);
    status = napi_create_string_utf8(env, friendInfo->user_info.phone, NAPI_AUTO_LENGTH, &phone);
    status = napi_create_string_utf8(env, friendInfo->user_info.email, NAPI_AUTO_LENGTH, &email);
    status = napi_create_string_utf8(env, friendInfo->user_info.region, NAPI_AUTO_LENGTH, &region);
    status = napi_create_int32(env, friendInfo->presence, &presence);
    status = napi_create_int32(env, friendInfo->status, &connectionStatus);
    status = napi_create_string_utf8(env, friendInfo->label, NAPI_AUTO_LENGTH, &label);

    status = napi_create_object(env, jFriendInfo);

    napi_property_descriptor properties[] = {
        { "userId", NULL, 0, 0, 0, userId, napi_default, 0 },
        { "name", NULL, 0, 0, 0, name, napi_writable, 0 },
        { "description", NULL, 0, 0, 0, description, napi_writable, 0 },
        { "hasAvatar", NULL, 0, 0, 0, hasAvatar, napi_default, 0 },
        { "gender", NULL, 0, 0, 0, gender, napi_writable, 0 },
        { "phone", NULL, 0, 0, 0, phone, napi_writable, 0 },
        { "email", NULL, 0, 0, 0, email, napi_writable, 0 },
        { "region", NULL, 0, 0, 0, region, napi_writable, 0 },
        { "presence", NULL, 0, 0, 0, presence, napi_writable, 0 },
        { "connectionStatus", NULL, 0, 0, 0, connectionStatus, napi_writable, 0 },
        { "label", NULL, 0, 0, 0, label, napi_writable, 0 }
    };

    status = napi_define_properties(env, *jFriendInfo, sizeof(properties) / sizeof(*properties), properties);
    if (status != napi_ok) {
        return 0;
    }
    return 1;
}

