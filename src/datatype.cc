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
    napi_value value_n1 = nullptr;

    static char* getStringProperty(napi_env env, napi_value obj, const char* name, char* buf, size_t len) {
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

    static bool getBoolProperty(napi_env env, napi_value obj, const char* name) {
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

    static uint32_t getUint32Property(napi_env env, napi_value obj, const char* name) {
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
        if (getStringProperty(env, obj, "ipv4", bootstraps_buf->ipv4, MAX_IPV4_ADDRESS_LEN)) {
            bootstraps->ipv4 = bootstraps_buf->ipv4;
        }

        //ipv6
        if (getStringProperty(env, obj, "ipv6", bootstraps_buf->ipv6, MAX_IPV6_ADDRESS_LEN)) {
            bootstraps->ipv6 = bootstraps_buf->ipv6;
        }

        //port
        if (getStringProperty(env, obj, "port", bootstraps_buf->port, MAX_PORT_LEN)) {
            bootstraps->port = bootstraps_buf->port;
        }

        //public_key
        if (getStringProperty(env, obj, "publicKey", bootstraps_buf->public_key, MAX_PUBLIC_KEY_LEN)) {
            bootstraps->public_key = bootstraps_buf->public_key;
        }

        // printf("ipv4:%s, ipv6:%s, port:%s, public_key:%s\n",
        //         bootstraps->ipv4, bootstraps->ipv6, bootstraps->port, bootstraps->public_key);

        return bootstraps;
    }

    ElaOptions *getOptionsValue(napi_env env, napi_value obj, ElaOptions *opts, BootstrapBuf** bootstraps_buf) {
        napi_status status;
        napi_value val;
        napi_valuetype valuetype;

        status = napi_typeof(env, obj, &valuetype);
        if (status != napi_ok) return nullptr;
        if (valuetype != napi_object) return nullptr;

        //udp_enabled
        opts->udp_enabled = getBoolProperty(env, obj, "udpEnabled");

        //persistent_location
        getStringProperty(env, obj, "persistentLocation", (char *)opts->persistent_location, 1024);

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

    ElaUserInfo* createUserInfoFromJsObj(napi_env env, napi_value obj, ElaUserInfo *info) {
        if (!obj || !info) {
            return nullptr;
        }

        memset(info, 0, sizeof(ElaUserInfo));

        getStringProperty(env, obj, "userId", info->userid, ELA_MAX_ID_LEN + 1);
        getStringProperty(env, obj, "name", info->name, ELA_MAX_USER_NAME_LEN + 1);
        getStringProperty(env, obj, "description", info->description, ELA_MAX_USER_DESCRIPTION_LEN + 1);
        info->has_avatar = getBoolProperty(env, obj, "hasAvatar");
        getStringProperty(env, obj, "gender", info->gender, ELA_MAX_GENDER_LEN + 1);
        getStringProperty(env, obj, "phone", info->phone, ELA_MAX_PHONE_LEN + 1);
        getStringProperty(env, obj, "email", info->email, ELA_MAX_EMAIL_LEN + 1);
        getStringProperty(env, obj, "region", info->region, ELA_MAX_REGION_LEN + 1);

        return info;
    }

    ElaFriendInfo* getFriendInfoFromJsObj(napi_env env, napi_value obj, ElaFriendInfo *info) {
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
                createUserInfoFromJsObj(env, val, &info->user_info);
            }
        }

        getStringProperty(env, obj, "label", info->label, ELA_MAX_USER_NAME_LEN + 1);
        info->status = (ElaConnectionStatus)getUint32Property(env, obj, "status");
        info->presence = (ElaPresenceStatus)getUint32Property(env, obj, "presence");

        return info;
    }

    napi_value createUserInfoJsObj(napi_env env, const ElaUserInfo *info) {
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

    napi_value createFriendInfoJsObj(napi_env env, const ElaFriendInfo *info) {
        napi_value user_info, label, connection_status, presence;
        napi_status status;

        if (!info) {
            return value_null;
        }

        user_info = createUserInfoJsObj(env, &info->user_info);

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

    napi_value create_AddressInfoJsObj(napi_env env, const ElaAddressInfo *info) {
        napi_value type, addr, port, related_addr, related_port;
        napi_status status;

        if (!info) {
            return value_null;
        }

        status = napi_create_int32(env, info->type , &type);
        CHECK_STATUS;

        status = napi_create_string_utf8(env, info->addr, NAPI_AUTO_LENGTH , &addr);
        CHECK_STATUS;

        status = napi_create_int32(env, info->port , &port);
        CHECK_STATUS;

        status = napi_create_string_utf8(env, info->related_addr, NAPI_AUTO_LENGTH , &related_addr);
        CHECK_STATUS;

        status = napi_create_int32(env, info->related_port , &related_port);
        CHECK_STATUS;

        napi_value obj;
        status = napi_create_object(env, &obj);
        CHECK_STATUS;

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "type", NULL, 0, 0, 0, type, napi_default, 0 },
            { "address", NULL, 0, 0, 0, addr, napi_default, 0 },
            { "port", NULL, 0, 0, 0, port, napi_default, 0 },
            { "relatedAddress", NULL, 0, 0, 0, addr, napi_default, 0 },
            { "relatedPort", NULL, 0, 0, 0, port, napi_default, 0 },
        };

        status = napi_define_properties(env, obj,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;

        return obj;
    }

    napi_value createUserInfoFromJsObj(napi_env env, const ElaTransportInfo *info) {
        napi_value topology, local, remote;
        napi_status status;

        if (!info) {
            return value_null;
        }

        status = napi_create_int32(env, info->topology , &topology);
        CHECK_STATUS;

        local = create_AddressInfoJsObj(env, &info->local);
        remote = create_AddressInfoJsObj(env, &info->remote);

        napi_value obj;
        status = napi_create_object(env, &obj);
        CHECK_STATUS;

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "topology", NULL, 0, 0, 0, topology, napi_default, 0 },
            { "local", NULL, 0, 0, 0, local, napi_default, 0 },
            { "remote", NULL, 0, 0, 0, remote, napi_default, 0 }
        };

        status = napi_define_properties(env, obj,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;

        return obj;
    }

//-----------------------------------------------------------------------------
    void addConstantsElement(napi_env env, napi_value exports, const char* name,
            size_t property_count, napi_property_descriptor *descriptors) {
        napi_value object;
        napi_status status;

        status = napi_create_object(env, &object);
        CHECK_STATUS;

        status = napi_define_properties(env, object, property_count, descriptors);
        CHECK_STATUS;

        status = napi_set_named_property(env, exports, name, object);
        CHECK_STATUS;
    }


    void createConstants(napi_env env, napi_value exports) {
        napi_value value_0, value_1, value_2, value_3, value_4, value_5, value_6, value_7;
        napi_value value_8, value_0x10;

        napi_get_null(env, &value_null);
        napi_get_boolean(env, false, &value_false);
        napi_get_boolean(env, true, &value_true);
        napi_create_int32(env, -1, &value_n1);

        napi_create_uint32(env, 0, &value_0);
        napi_create_uint32(env, 1, &value_1);
        napi_create_uint32(env, 2, &value_2);
        napi_create_uint32(env, 3, &value_3);
        napi_create_uint32(env, 4, &value_4);
        napi_create_uint32(env, 5, &value_5);
        napi_create_uint32(env, 6, &value_6);
        napi_create_uint32(env, 7, &value_7);
        napi_create_uint32(env, 8, &value_8);
        napi_create_uint32(env, 0x10, &value_0x10);

        napi_property_descriptor descriptors[] = {
            { "0", NULL, 0, 0, 0, value_0, napi_default, 0 },
            { "1", NULL, 0, 0, 0, value_1, napi_default, 0 },
            { "2", NULL, 0, 0, 0, value_2, napi_default, 0 },
            { "3", NULL, 0, 0, 0, value_3, napi_default, 0 },
            { "4", NULL, 0, 0, 0, value_4, napi_default, 0 },
            { "5", NULL, 0, 0, 0, value_5, napi_default, 0 },
            { "6", NULL, 0, 0, 0, value_6, napi_default, 0 },
            { "7", NULL, 0, 0, 0, value_7, napi_default, 0 },
        };

        //PresenceStatus
        descriptors[0].utf8name = "NONE";
        descriptors[1].utf8name = "AWAY";
        descriptors[2].utf8name = "BUSY";
        addConstantsElement(env, exports, "PresenceStatus", 3, descriptors);

        //ConnectionStatus
        descriptors[0].utf8name = "CONNECTED";
        descriptors[1].utf8name = "DISCONNECTED";
        addConstantsElement(env, exports, "ConnectionStatus", 2, descriptors);

        //StreamType
        descriptors[0].utf8name = "AUDIO";
        descriptors[1].utf8name = "VIDEO";
        descriptors[2].utf8name = "TEXT";
        descriptors[3].utf8name = "APPLICATION";
        descriptors[4].utf8name = "MESSAGE";
        addConstantsElement(env, exports, "StreamType", 5, descriptors);

        //StreamState
        descriptors[0].utf8name = "RAW";
        descriptors[1].utf8name = "INITIALIZED";
        descriptors[2].utf8name = "TRANSPORT_READY";
        descriptors[3].utf8name = "CONNECTING";
        descriptors[4].utf8name = "CONNECTED";
        descriptors[5].utf8name = "DEACTIVATED";
        descriptors[6].utf8name = "CLOSED";
        descriptors[7].utf8name = "FAILED";
        addConstantsElement(env, exports, "StreamState", 7, descriptors + 1);

        //CandidateType
        descriptors[0].utf8name = "HOST";
        descriptors[1].utf8name = "SERVE_RREFLEXIVE";
        descriptors[2].utf8name = "PEER_REFLEXIVE";
        descriptors[3].utf8name = "RELAYED";
        addConstantsElement(env, exports, "CandidateType", 4, descriptors);

        //NetworkTopology
        descriptors[0].utf8name = "LAN";
        descriptors[1].utf8name = "P2P";
        descriptors[2].utf8name = "RELAYED";
        addConstantsElement(env, exports, "NetworkTopology", 3, descriptors);

        //PortForwardingProtocol
        descriptors[1].utf8name = "TCP";
        addConstantsElement(env, exports, "PortForwardingProtocol", 1, descriptors + 1);

        //CloseReason
        descriptors[0].utf8name = "NORMAL";
        descriptors[1].utf8name = "TIMEOUT";
        descriptors[2].utf8name = "ERROR";
        addConstantsElement(env, exports, "CloseReason", 3, descriptors);

        //StreamMode
        descriptors[0].utf8name = "COMPRESS";
        descriptors[0].value = value_1;
        descriptors[1].utf8name = "PLAIN";
        descriptors[1].value = value_2;
        descriptors[2].utf8name = "RELIABLE";
        descriptors[2].value = value_4;
        descriptors[3].utf8name = "MULTIPLEXING";
        descriptors[3].value = value_8;
        descriptors[4].utf8name = "PORT_FORWARDING";
        descriptors[4].value = value_0x10;
        addConstantsElement(env, exports, "StreamMode", 5, descriptors);
    }

} // namespace elca
