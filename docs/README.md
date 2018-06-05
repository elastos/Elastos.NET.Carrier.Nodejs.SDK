## Elca(Elastos Carrier Addon) API

### Constants

### PresenceStatus:
    NONE = 0
    AWAY = 1
    BUSY = 2

### ConnectionStatus:
    CONNECTED = 0
    DISCONNECTED = 1

### StreamType:
    AUDIO = 0
    VIDEO = 1
    TEXT = 2
    APPLICATION = 3
    MESSAGE = 4

### StreamState:
    RAW = 0
    INITIALIZED = 1
    TRANSPORT_READY = 2
    CONNECTING = 3
    CONNECTED = 4
    DEACTIVATED = 5
    CLOSED = 6
    FAILED = 7

### CandidateType
     HOST = 0
     SERVE_RREFLEXIVE = 1
     PEER_REFLEXIVE = 2
     RELAYED = 3

### NetworkTopology
    LAN = 0
    P2P = 1
    RELAYED = 2

### PortForwardingProtocol
    TCP = 1

### CloseReason
    NORMAL = 0
    TIMEOUT = 1
    ERROR = 2

### StreamMode
    COMPRESS = 1
    PLAIN = 2
    RELIABLE = 4
    MULTIPLEXING = 8
    PORT_FORWARDING = 0x10

### Data types
    //Bootstrap
    object bootstrap {
        string ipv4,
        string ipv6,
        string port,
        string publicKey
    }

    //ElaOptions
    object options {
        boolean udpEnabled,
        string persistentLocation,
        bootstrap bootstraps[]
    }

    //ElaCallbacks
    object callbacks {
        idle(object carrier),
        connectionStatus(object carrier, ConnectionStatus status),
        friendsList(object carrier, friendInfo friend_info),
        friendConnection(object carrier, string friend_id, ConnectionStatus status),
        friendInfo(objet carrier, string friend_id, friendInfo info),
        friendPresence(object carrier, sting friend_id,  PresenceStatus status),
        friendRequest(object carrier, sting user_id, userInfo info, string hello),
        friendAdded(object carrier, friendInfo info),
        friendRemoved(object carrier, string friend_id),
        friendMessage(object carrier, string from, string msg) ,
        friendInvite(object carrier, string from, string data)
    }

    //ElaFriendsIterateCallback
    friendsIterate(object carrier, friendInfo info)

    //ElaFriendInviteResponseCallback
    friendInviteResponse(object carrier, string from, int status, string reason, string data)

    //ElaUserInfo
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

    //ElaFriendInfo
    object friendInfo {
        object userInfo,
        string label,
        ConnectionStatus status,
        PresenceStatus presence
    }

    //ElaAddressInfo
    object addressInfo {
        int32 type,
        string address,
        int32 port,
        string relatedAddress,
        int32 relatedPort
    }

    //ElaTransportInfo
    object transportInfo {
        int32 topology,
        addressInfo local,
        addressInfo remote
    }

    //ElaStreamCallbacks
    object streamCallbacks {
        stateChanged(object stream, StreamState state),
        streamData(object stream, Buffer data),
        channelOpen(object stream, int32 channel_id, string cookie),
        channelOpened(object stream, int32 channel_id),
        channelClose(objet stream, int32 channel_id, string reason),
        channelData(object stream, int32 channel_id, Buffer data),
        channelPending(object stream, int32 channel_id),
        channelResume(object stream, int32 channel_id),
    }

    //ElaSessionRequestCallback
    sessionRequest(object carrier, string from, string sdp)

    //ElaSessionRequestCompleteCallback
    sessionRequestComplete(object session, int32 status, string reason, string sdp)

### Functions

    Object createObject(options opts, callbacks cbs) = ela_new

    object carrier {
        run()                                                       = ela_run
        destroy()                                                   = ela_kill
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

        getFriends(friendsIterate callback)                         = ela_get_friends
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

        boolean initSession(sessionRequest callback)                = ela_session_init
        cleanupSession()                                            = ela_session_cleanup

        session newSession(string address)                          = ela_session_new

        on(stirng name, function callback || null)
    }

    object session {
        close()                                                     = ela_session_close
        string getPeer(string address)                              = ela_session_get_peer
        boolean request(sessionRequestComplete callback)            = ela_session_request
        boolean replyRequest(uint32 status, string reason)          = ela_session_reply_request
        boolean start(string sdp)                                   = ela_session_start

        boolean addService(string service, PortForwardingProtocol protocol, string host, string port)
                                                                    = ela_session_add_service
        removeService(string service)                               = ela_session_remove_service

        stream addStream(StreamType type, uint32 options, streamCallbacks callbacks,)
                                                                    = ela_session_add_stream

        on(stirng name, function callback || null)
    }

    object stream {
        remove()                                                    = ela_session_remove_stream
        uint32 getType()                                            = ela_stream_get_type
        uint32 getState()                                           = ela_stream_get_state
        transportInfo getTransportInfo()                            = ela_stream_get_transport_info
        int32 write(Buffer data)                                    = ela_stream_write

        int32 openChannel()                                         = ela_stream_open_channel
        boolean closeChannel(int32 id)                              = ela_stream_close_channel
        int32 writeChannel(int32 id, Buffer data)                   = elca_stream_write_channel
        boolean pendChannel(int32 id)                               = ela_stream_pend_channel
        boolean resumeChannel(int32 id)                             = ela_stream_resume_channel

        int32 openPortForwarding(string service, PortForwardingProtocol protocol, string host, string port)
                                                                    = ela_stream_open_port_forwarding
        boolean closePortForwarding(uint32 portforwarding)          = ela_stream_close_port_forwarding

        on(stirng name, function callback || null)
    }

    string getVersion()                                             = ela_get_version
    logInit(int level, sting log_file)                              = ela_log_init
    boolean addressIsValid(string address)                          = ela_address_is_valid
    boolean idIsValid(string id)                                    = ela_is_valid
    int getError()                                                  = ela_get_error
    clearError()                                                    = ela_clear_error





