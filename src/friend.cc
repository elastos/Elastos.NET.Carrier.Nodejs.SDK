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


#include "elca.h"

namespace elca {

    bool friends_iterate_callback(const ElaFriendInfo *info, void *context);
    void friend_invite_response_callback(ElaCarrier *carrier, const char *from,
                                    int status, const char *reason, const char *data,
                                    size_t len, void *context);

    static napi_value elca_get_friends(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_value context = nullptr;

        int ret;

        size_t argc = 2;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 1) {
            log_err(env, "elca_get_friends: Wrong number of arguments");
            return nullptr;
        }

        if (!elca || !elca->elcarrier) return nullptr;
        if (argc > 1) context = args[2];

        set_CallbackHandle(env, args[0], context, FRIENDS_ITERATE, elca);

        ret = ela_get_friends(elca->elcarrier, friends_iterate_callback, elca);
        if (ret == -1) {
            log_err(env, "Run ela_get_friends error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    static napi_value elca_get_friend_info(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char friend_id[ELA_MAX_ID_LEN + 1];
        ElaFriendInfo friend_info;
        int ret = -1;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 1) {
            log_err(env, "elca_get_friends: Wrong number of arguments");
            return nullptr;
        }

        if (!elca || !elca->elcarrier) return value_false;

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return nullptr;

        status = napi_get_value_string_utf8(env, args[0], friend_id, ELA_MAX_ID_LEN + 1, &len);
        if (status != napi_ok) return nullptr;

        ret = ela_get_friend_info(elca->elcarrier, friend_id, &friend_info);
        if (ret == -1) {
            log_err(env, "Run ela_get_friend_info error:0x%X", ela_get_error());
            return nullptr;
        }
        else {
            return create_FriendInfoJsObj(env, &friend_info);
        }
    }

    static napi_value elca_set_friend_label(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char friend_id[ELA_MAX_ID_LEN + 1];
        char label[ELA_MAX_USER_NAME_LEN + 1];
        int ret = -1;

        size_t argc = 2, len;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 2) {
            log_err(env, "elca_get_friends: Wrong number of arguments");
            return value_false;
        }

        if (!elca || !elca->elcarrier) return value_false;

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], friend_id, ELA_MAX_ID_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[1], label, ELA_MAX_USER_NAME_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_set_friend_label(elca->elcarrier, friend_id, label);
        if (ret == -1) {
            log_err(env, "Run ela_set_friend_label error:0x%X", ela_get_error());
            return value_false;
        }
        else {
            return value_true;
        }
    }

    static napi_value elca_is_friend(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char user_id[ELA_MAX_ID_LEN + 1];
        int ret = -1;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 1) {
            log_err(env, "elca_is_friend: Wrong number of arguments");
            return value_false;
        }

        if (!elca || !elca->elcarrier) return value_false;

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], user_id, ELA_MAX_ID_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_is_friend(elca->elcarrier, user_id);
        if (ret) {
            return value_true;
        }
        else {
            return value_false;
        }
    }

    static napi_value elca_add_friend(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char address[ELA_MAX_ADDRESS_LEN + 1];
        char hello[ELA_MAX_APP_MESSAGE_LEN + 1];
        int ret = -1;

        size_t argc = 2, len;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 2) {
            log_err(env, "elca_add_friend: Wrong number of arguments");
            return value_false;
        }

        if (!elca || !elca->elcarrier) return value_false;

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], address, ELA_MAX_ADDRESS_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[1], hello, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_add_friend(elca->elcarrier, address, hello);
        if (ret == -1) {
            log_err(env, "Run ela_add_friend error:0x%X", ela_get_error());
            return value_false;
        }
        else {
            return value_true;
        }
    }

    static napi_value elca_accept_friend(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char user_id[ELA_MAX_ID_LEN + 1];
        int ret = -1;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 1) {
            log_err(env, "elca_accept_friend: Wrong number of arguments");
            return value_false;
        }

        if (!elca || !elca->elcarrier) return value_false;

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], user_id, ELA_MAX_ID_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_accept_friend(elca->elcarrier, user_id);
        if (ret == -1) {
            log_err(env, "Run ela_accept_friend error:0x%X", ela_get_error());
            return value_false;
        }
        else {
            return value_true;
        }
    }

    static napi_value elca_remove_friend(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char user_id[ELA_MAX_ID_LEN + 1];
        int ret = -1;

        size_t argc = 1, len;
        napi_value args[1];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 1) {
            log_err(env, "elca_remove_friend: Wrong number of arguments");
            return value_false;
        }

        if (!elca || !elca->elcarrier) return value_false;

        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], user_id, ELA_MAX_ID_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_remove_friend(elca->elcarrier, user_id);
        if (ret == -1) {
            log_err(env, "Run ela_remove_friend error:0x%X", ela_get_error());
            return value_false;
        }
        else {
            return value_true;
        }
    }

    static napi_value elca_send_friend_message(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char user_id[ELA_MAX_ID_LEN + 1];
        char msg[ELA_MAX_APP_MESSAGE_LEN + 1];
        int ret = -1;

        size_t argc = 2, len;
        napi_value args[2];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 2) {
            log_err(env, "elca_send_friend_message: Wrong number of arguments");
            return value_false;
        }

        if (!elca || !elca->elcarrier) return value_false;

        //get user_id
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], user_id, ELA_MAX_ADDRESS_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        //get message
        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[1], msg, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        ret = ela_send_friend_message(elca->elcarrier, user_id, msg, len);
        if (ret == -1) {
            log_err(env, "Run ela_send_friend_message error:0x%X", ela_get_error());
            return value_false;
        }
        else {
            return value_true;
        }
    }

    static napi_value elca_invite_friend(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;

        char user_id[ELA_MAX_ID_LEN + 1];
        char data[ELA_MAX_APP_MESSAGE_LEN + 1];
        int ret = -1;

        napi_status status;
        napi_valuetype valuetype;
        napi_value context = nullptr;

        size_t argc = 4, len;
        napi_value args[4];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 3) {
            log_err(env, "elca_invite_friend: Wrong number of arguments");
            return value_false;
        }

        if (!elca || !elca->elcarrier) return value_false;
        if (argc > 3) context = args[3];

        //get to
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], user_id, ELA_MAX_ADDRESS_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        //get data
        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[1], data, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        //get callback
        set_CallbackHandle(env, args[2], context, FRIEND_INVITE_RESPONSE, elca);

        ret = ela_invite_friend(elca->elcarrier, user_id, data, len, friend_invite_response_callback, elca);
        if (ret == -1) {
            log_err(env, "Run ela_invite_friend error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    static napi_value elca_reply_friend_invite(napi_env env, napi_callback_info info) {
        Elca* elca = nullptr;
        napi_status status;
        napi_valuetype valuetype;
        char user_id[ELA_MAX_ID_LEN + 1];
        char msg[ELA_MAX_APP_MESSAGE_LEN + 1];
        char reason[ELA_MAX_APP_MESSAGE_LEN + 1];
        int ret = -1;
        uint32_t reply_status;

        size_t argc = 4, len;
        napi_value args[4];

        status = napi_get_cb_info(env, info, &argc, args, nullptr, (void**)&elca);
        if(status != napi_ok || argc < 4) {
            log_err(env, "elca_reply_friend_invite: Wrong number of arguments");
            return value_false;
        }

        if (!elca || !elca->elcarrier) return value_false;

        //get user_id
        status = napi_typeof(env, args[0], &valuetype);
        if (status != napi_ok || valuetype != napi_string) return value_false;
        status = napi_get_value_string_utf8(env, args[0], user_id, ELA_MAX_ADDRESS_LEN + 1, &len);
        if (status != napi_ok) return value_false;

        //get status
        status = napi_typeof(env, args[1], &valuetype);
        if (status != napi_ok || valuetype != napi_number) return value_false;
        status = napi_get_value_uint32(env, args[1], &reply_status);
        if (status != napi_ok) return value_false;

        if (reply_status == 0) {
            //get message
            status = napi_typeof(env, args[3], &valuetype);
            if (status != napi_ok || valuetype != napi_string) return value_false;
            status = napi_get_value_string_utf8(env, args[3], msg, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
            if (status != napi_ok) return value_false;

            reason[0] ='\0';
        }
        else { //if (reply_status == -1) {
            //get reason
            status = napi_typeof(env, args[2], &valuetype);
            if (status != napi_ok || valuetype != napi_string) return value_false;
            status = napi_get_value_string_utf8(env, args[2], reason, ELA_MAX_APP_MESSAGE_LEN + 1, &len);
            if (status != napi_ok) return value_false;

            msg[0] ='\0';
        }

        ret = ela_reply_friend_invite(elca->elcarrier, user_id, reply_status, reason, msg, len);
        if (ret == -1) {
            log_err(env, "Run ela_reply_friend_invite error:0x%X", ela_get_error());
            return value_false;
        }
        else return value_true;
    }

    void create_FriendFunctions(napi_env env, napi_value carrier, Elca *elca) {
        napi_status status;
        napi_value fn[10];
        memset(fn, 0, sizeof(napi_value) * 10);

        napi_create_function(env, nullptr, 0, elca_get_friends, (void*)elca, &fn[0]);
        napi_create_function(env, nullptr, 0, elca_get_friend_info, (void*)elca, &fn[1]);
        napi_create_function(env, nullptr, 0, elca_set_friend_label, (void*)elca, &fn[2]);
        napi_create_function(env, nullptr, 0, elca_is_friend, (void*)elca, &fn[3]);
        napi_create_function(env, nullptr, 0, elca_add_friend, (void*)elca, &fn[4]);
        napi_create_function(env, nullptr, 0, elca_accept_friend, (void*)elca, &fn[5]);
        napi_create_function(env, nullptr, 0, elca_remove_friend, (void*)elca, &fn[6]);
        napi_create_function(env, nullptr, 0, elca_send_friend_message, (void*)elca, &fn[7]);
        napi_create_function(env, nullptr, 0, elca_invite_friend, (void*)elca, &fn[8]);
        napi_create_function(env, nullptr, 0, elca_reply_friend_invite, (void*)elca, &fn[9]);

        // Set the properties
        napi_property_descriptor descriptors[] = {
            { "getFriends", NULL, 0, 0, 0, fn[0], napi_default, 0 },
            { "getFriendInfo", NULL, 0, 0, 0, fn[1], napi_default, 0 },
            { "setFriendLabel", NULL, 0, 0, 0, fn[2], napi_default, 0 },
            { "isFriend", NULL, 0, 0, 0, fn[3], napi_default, 0 },
            { "addFriend", NULL, 0, 0, 0, fn[4], napi_default, 0 },
            { "acceptFriend", NULL, 0, 0, 0, fn[5], napi_default, 0 },
            { "removeFriend", NULL, 0, 0, 0, fn[6], napi_default, 0 },
            { "sendFriendMessage", NULL, 0, 0, 0, fn[7], napi_default, 0 },
            { "inviteFriend", NULL, 0, 0, 0, fn[8], napi_default, 0 },
            { "replyFriendInvite", NULL, 0, 0, 0, fn[9], napi_default, 0 },
        };

        status = napi_define_properties(env, carrier,
                                        sizeof(descriptors) / sizeof(descriptors[0]),
                                        descriptors);
        CHECK_STATUS;
    }


} // namespace elca
