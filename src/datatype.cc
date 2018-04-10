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

    napi_value value_null = nullptr;
    napi_value value_false = nullptr;
    napi_value value_true = nullptr;

    static char* get_StringProperty(napi_env env, napi_value obj, const char* name, char* buf, size_t len) {
        napi_value val;
        napi_valuetype valuetype;
        size_t size;

        napi_status status = napi_get_named_property(env, obj, name, &val);
        if (status == napi_ok) {
            status = napi_typeof(env, obj, &valuetype);
            if (status == napi_ok || valuetype == napi_string) {
                status = napi_get_value_string_utf8(env, val, buf, len, &size);
                if (status == napi_ok) {
                    return buf;
                }
            }
        }
        return nullptr;
    }

    static bool get_BoolProperty(napi_env env, napi_value obj, const char* name) {
        napi_value val;
        napi_valuetype valuetype;
        bool ret = false;

        napi_status status = napi_get_named_property(env, obj, name, &val);
        if (status == napi_ok) {
            status = napi_typeof(env, obj, &valuetype);
            if (status == napi_ok || valuetype == napi_boolean) {
                napi_get_value_bool(env, val, &ret);
            }
        }
        return ret;
    }

    static uint32_t get_Uint32Property(napi_env env, napi_value obj, const char* name) {
        napi_value val;
        napi_valuetype valuetype;
        uint32_t ret = 0;

        napi_status status = napi_get_named_property(env, obj, name, &val);
        if (status == napi_ok) {
            status = napi_typeof(env, obj, &valuetype);
            if (status == napi_ok || valuetype == napi_number) {
                napi_get_value_uint32(env, val, &ret);
            }
        }
        return ret;
    }


    BootstrapNode *set_BootstrapNode(napi_env env, napi_value obj, BootstrapNode* bootstraps, BootstrapBuf* bootstraps_buf) {
        napi_status status;
        napi_valuetype valuetype;

        status = napi_typeof(env, obj, &valuetype);
        if (status != napi_ok) return nullptr;
        if (valuetype != napi_object) return nullptr;

        //ipv4
        if (get_StringProperty(env, obj, "ipv4", bootstraps_buf->ipv4, MAX_IPV4_ADDRESS_LEN)) {
            bootstraps->ipv4 = bootstraps_buf->ipv4;
        }

        //ipv6
        if (get_StringProperty(env, obj, "ipv6", bootstraps_buf->ipv6, MAX_IPV6_ADDRESS_LEN)) {
            bootstraps->ipv6 = bootstraps_buf->ipv6;
        }

        //port
        if (get_StringProperty(env, obj, "port", bootstraps_buf->port, MAX_PORT_LEN)) {
            bootstraps->port = bootstraps_buf->port;
        }

        //public_key
        if (get_StringProperty(env, obj, "publicKey", bootstraps_buf->public_key, MAX_PUBLIC_KEY_LEN)) {
            bootstraps->public_key = bootstraps_buf->public_key;
        }

        // printf("ipv4:%s, ipv6:%s, port:%s, public_key:%s\n",
        //         bootstraps->ipv4, bootstraps->ipv6, bootstraps->port, bootstraps->public_key);

        return bootstraps;
    }

    ElaOptions *get_OptionsValue(napi_env env, napi_value obj, ElaOptions *opts, BootstrapBuf** bootstraps_buf) {
        napi_status status;
        napi_value val;
        napi_valuetype valuetype;

        status = napi_typeof(env, obj, &valuetype);
        if (status != napi_ok) return nullptr;
        if (valuetype != napi_object) return nullptr;

        //udp_enabled
        opts->udp_enabled = get_BoolProperty(env, obj, "udpEnabled");

        //persistent_location
        get_StringProperty(env, obj, "persistentLocation", (char *)opts->persistent_location, 1024);

        //bootstraps
        status = napi_get_named_property(env, obj, "bootstraps", &val);
        CHECK_STATUS;

        bool is_array;
        status = napi_is_array(env, val, &is_array);
        if (status != napi_ok || !is_array) {
            return nullptr;
        }

        uint32_t length = 0;
        status = napi_get_array_length(env, val, &length);
        if (status != napi_ok || length < 1) {
            return nullptr;
        }

        opts->bootstraps_size = length;
        HEAP_ALLOC(opts->bootstraps, BootstrapNode, length, nullptr);
        HEAP_ALLOC(*bootstraps_buf, BootstrapBuf, length, nullptr);

        napi_value element;
        for (uint32_t i = 0; i < length; i++) {
            // printf("index:%d-- ", i);
            status = napi_get_element(env, val, i, &element);
            if (status != napi_ok) {
                return nullptr;
            }

            if (!set_BootstrapNode(env, element, &opts->bootstraps[i], &(*bootstraps_buf)[i])) {
                return nullptr;
            }
        }

        return opts;
    }

    ElaUserInfo* get_UserInfoFromJsObj(napi_env env, napi_value obj, ElaUserInfo *info) {
        if (!obj || !info) {
            return nullptr;
        }

        memset(info, 0, sizeof(ElaUserInfo));

        get_StringProperty(env, obj, "userId", info->userid, ELA_MAX_ID_LEN + 1);
        get_StringProperty(env, obj, "name", info->name, ELA_MAX_USER_NAME_LEN + 1);
        get_StringProperty(env, obj, "description", info->description, ELA_MAX_USER_DESCRIPTION_LEN + 1);
        info->has_avatar = get_BoolProperty(env, obj, "hasAvatar");
        get_StringProperty(env, obj, "gender", info->gender, ELA_MAX_GENDER_LEN + 1);
        get_StringProperty(env, obj, "phone", info->phone, ELA_MAX_PHONE_LEN + 1);
        get_StringProperty(env, obj, "email", info->email, ELA_MAX_EMAIL_LEN + 1);
        get_StringProperty(env, obj, "region", info->region, ELA_MAX_REGION_LEN + 1);

        return info;
    }

    ElaFriendInfo* get_FriendInfoFromJsObj(napi_env env, napi_value obj, ElaFriendInfo *info) {
        napi_status status;
        napi_value val;
        napi_valuetype valuetype;

        if (!obj) {
            return nullptr;
        }

        status = napi_get_named_property(env, obj, "userInfo", &val);
        if (status == napi_ok) {
            status = napi_typeof(env, obj, &valuetype);
            if (status == napi_ok || valuetype == napi_object) {
                get_UserInfoFromJsObj(env, val, &info->user_info);
            }
        }

        get_StringProperty(env, obj, "label", info->label, ELA_MAX_USER_NAME_LEN + 1);
        info->status = (ElaConnectionStatus)get_Uint32Property(env, obj, "status");
        info->presence = (ElaPresenceStatus)get_Uint32Property(env, obj, "presence");

        return info;
    }

    napi_value create_UserInfoJsObj(napi_env env, const ElaUserInfo *info) {
        napi_value userid, name, description, has_avatar, gender, phone, email, region;
        napi_status status;

        if (!info) {
            return value_null;
        }

        status = napi_create_string_utf8(env, info->userid, NAPI_AUTO_LENGTH , &userid);
        CHECK_STATUS;

        status = napi_create_string_utf8(env, info->name, NAPI_AUTO_LENGTH , &name);
        CHECK_STATUS;

        status = napi_create_string_utf8(env, info->description, NAPI_AUTO_LENGTH , &description);
        CHECK_STATUS;

        status = napi_create_int32(env, info->has_avatar , &has_avatar);
        CHECK_STATUS;

        status = napi_create_string_utf8(env, info->gender, NAPI_AUTO_LENGTH , &gender);
        CHECK_STATUS;

        status = napi_create_string_utf8(env, info->phone, NAPI_AUTO_LENGTH , &phone);
        CHECK_STATUS;

        status = napi_create_string_utf8(env, info->email, NAPI_AUTO_LENGTH , &email);
        CHECK_STATUS;

        status = napi_create_string_utf8(env, info->region, NAPI_AUTO_LENGTH , &region);
        CHECK_STATUS;

        napi_value obj;
        status = napi_create_object(env, &obj);
        CHECK_STATUS;

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "userId", NULL, 0, 0, 0, userid, napi_default, 0 },
            { "name", NULL, 0, 0, 0, name, napi_writable, 0 },
            { "description", NULL, 0, 0, 0, description, napi_writable, 0 },
            { "hasAvatar", NULL, 0, 0, 0, has_avatar, napi_default, 0 },
            { "gender", NULL, 0, 0, 0, gender, napi_writable, 0 },
            { "phone", NULL, 0, 0, 0, phone, napi_writable, 0 },
            { "email", NULL, 0, 0, 0, email, napi_writable, 0 },
            { "region", NULL, 0, 0, 0, region, napi_writable, 0 }
        };

        status = napi_define_properties(env, obj,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;

        return obj;
    }

    napi_value create_FriendInfoJsObj(napi_env env, const ElaFriendInfo *info) {
        napi_value user_info, label, connection_status, presence;
        napi_status status;

        if (!info) {
            return value_null;
        }

        user_info = create_UserInfoJsObj(env, &info->user_info);

        status = napi_create_string_utf8(env, info->label, NAPI_AUTO_LENGTH , &label);
        CHECK_STATUS;

        status = napi_create_int32(env, info->status , &connection_status);
        CHECK_STATUS;

        status = napi_create_int32(env, info->presence , &presence);
        CHECK_STATUS;

        napi_value obj;
        status = napi_create_object(env, &obj);
        CHECK_STATUS;

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "userInfo", NULL, 0, 0, 0, user_info, napi_default, 0 },
            { "label", NULL, 0, 0, 0, label, napi_writable, 0 },
            { "status", NULL, 0, 0, 0, connection_status, napi_default, 0 },
            { "presence", NULL, 0, 0, 0, presence, napi_default, 0 }
        };

        status = napi_define_properties(env, obj,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;

        return obj;
    }

//-----------------------------------------------------------------------------

    void create_Constants(napi_env env, napi_value exports) {
        napi_value value_0, value_1, value_2, value_3;

        napi_get_null(env, &value_null);
        napi_get_boolean(env, false, &value_false);
        napi_get_boolean(env, true, &value_true);

        napi_create_uint32(env, 0, &value_0);
        napi_create_uint32(env, 1, &value_1);
        napi_create_uint32(env, 2, &value_2);
        napi_create_uint32(env, 3, &value_3);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            //PresenceStatus
            { "PresenceStatus_None", NULL, 0, 0, 0, value_0, napi_default, 0 },
            { "PresenceStatus_Away", NULL, 0, 0, 0, value_1, napi_default, 0 },
            { "PresenceStatus_Busy", NULL, 0, 0, 0, value_2, napi_default, 0 },

            //ConnectionStatus
            { "ConnectionStatus_Connected", NULL, 0, 0, 0, value_0, napi_default, 0 },
            { "ConnectionStatus_Disconnected", NULL, 0, 0, 0, value_1, napi_default, 0 },

        };

        napi_define_properties(env, exports,
                                    sizeof(descriptors) / sizeof(descriptors[0]),
                                    descriptors);
    }

} // namespace elca
