#ifndef __CARRIER_UTILS_H__
#define __CARRIER_UTILS_H__

#include <node_api.h>
#include "include/ela_carrier.h"

typedef struct BootstrapHelper {
    char *ipv4;
    char *ipv6;
    char *port;
    char *public_key;
} BootstrapHelper;

typedef struct ExpressNodeHelper {
    char *ipv4;
    char *ipv6;
    char *port;
    char *public_key;
} ExpressNodeHelper;

typedef struct OptionsHelper {
    int udp_enabled;
    char* persistent_location;
    uint8_t* secret_key;
    size_t  bootstraps_size;
    BootstrapHelper *bootstraps;
    size_t express_nodes_size;
    ExpressNodeHelper *express_nodes;
} OptionsHelper;

int getOptionsHelper(napi_env env, napi_callback_info info, napi_value options, OptionsHelper *opts);

int getNativeUserInfo(napi_env env, napi_callback_info info, napi_value jUserInfo, ElaUserInfo *userInfo);

int newUserInfo(napi_env env, ElaUserInfo *userInfo, napi_value *jUserInfo);

int newFriendInfo(napi_env env, ElaFriendInfo *friendInfo, napi_value *jFriendInfo);

#endif //__CARRIER_UTILS_H__