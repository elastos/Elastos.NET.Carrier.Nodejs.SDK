## Elca(Elastos Carrier Addon) API

### Constants

### PresenceStatus:
    PresenceStatus_None = 0
    PresenceStatus_Away = 1
    PresenceStatus_Busy = 2

### ConnectionStatus:
    ConnectionStatus_Connected = 0
    ConnectionStatus_Disconnected = 1

### Data types
    object bootstrap {
        string ipv4,
        string ipv6,
        string port,
        string publicKey
    }

    object options {
        boolean udpEnabled,
        string persistentLocation,
        bootstrap bootstraps[]
    }

    object callbacks {
        idle(object carrier, [var context]),
        connectionStatus(object carrier, ConnectionStatus status, [var context]),
        friendsList(object carrier, friendInfo friend_info, [var context]),
        friendConnection(object carrier, string friend_id, ConnectionStatus status, [var context]),
        friendInfo(objet carrier, string friend_id, friendInfo info, [var context]),
        friendPresence(object carrier, sting friend_id,  PresenceStatus status, [var context]),
        friendRequest(object carrier, sting user_id, userInfo info, string hello, [var context]),
        friendAdded(object carrier, friendInfo info, [var context]),
        friendRemoved(object carrier, string friend_id, [var context]),
        friendMessage(object carrier, string from, string msg, [var context]) ,
        friendInvite(object carrier, string from, string data, [var context])
    }

    friendsIterate(object carrier, friendInfo info ,[var context])
    friendInviteResponse(object carrier, string from, int status, string reason, string data, [var context])

    object userInfo {
        string userId,
        string name,
        string description
        boolean hasAvatar,
        string gender,
        string phone,
        string email,
        string region
    }

    object friendInfo {
        object userInfo,
        string label,
        ConnectionStatus status,
        PresenceStatus presence
    }

### Functions

    Object createObject(options opts, callbacks cbs, [var context]) = ela_new

    object carrier {
        run()                                                       = ela_run
        destory()                                                   = ela_kill
        boolean isReady()                                           = ela_is_ready

        string getAddress()                                         = ela_get_address
        string getNodeId()                                          = ela_get_nodeid
        string getUserId()                                          = ela_get_userid
        string getIdByAddress(string address)                       = ela_get_id_by_address
        boolean setSelfNospam()                                     = ela_set_self_nospam
        int getSelfNospam()                                         = ela_get_self_nospam
        boolean setSelfInfo(userInfo info)                          = ela_set_self_info
        userInfo getSelfInfo()                                      = ela_get_self_info
        boolean setSelfPresence(PresenceStatus presence)            = ela_set_self_presence
        PresenceStatus getSelfPresence()                            = ela_get_self_presence

        getFriends(friendsIterate callback, [var context])          = ela_get_friends
        friendInfo getFriendInfo(string friend_id)                  = ela_get_friend_info
        boolean setFriendLabel(string friend_id, string label)      = ela_set_friend_lable
        boolean isFriend(string id)                                 = ela_is_friend
        boolean addFriend(string address, string hello)             = ela_add_friend
        boolean acceptFriend(string id)                             = ela_accept_friend
        boolean removeFriend(string id)                             = ela_remove_friend
        boolean sendFriendMessage(string id, string msg)            = ela_send_friend_message
        boolean inviteFriend(string id, string data,
                    friendInviteResponse callback)                  = ela_invite_friend
        boolean replyFriendInvite(string id, int status,
                    string reason, string msg)                      = ela_reply_friend_invite
    }

    string getVersion()                                             = ela_get_version
    logInit(int level, sting log_file)                              = ela_log_init
    boolean addressIsValid(string address)                          = ela_address_is_valid
    boolean idIsValid(string id)                                    = ela_is_valid
    int getError()                                                  = ela_get_error
    clearError()                                                    = ela_clear_error





