
(function(){
  const SDK = require('elastos_carrier_addon');
  const _ = require('lodash');
  const _log = {
    debug(str){
      console.log(str);
      F.store.commit('add_log', str);
    },
    error(err){
      console.error(err);
      F.store.commit('add_error', err.toString());
    }
  };
  const hash = (str)=>{
    return str;
  };

  const F = {
    connection_name : [
      "online",
      "offline"
    ],
    presence_name : [
      "none",    // None;
      "away",    // Away;
      "busy",    // Busy;
    ],
    bootstraps : [
      { ipv4: "13.58.208.50",     port: "33445",  publicKey: "89vny8MrKdDKs7Uta9RdVmspPjnRMdwMmaiEW27pZ7gh"},
      { ipv4: "18.216.102.47",    port: "33445",  publicKey: "G5z8MqiNDFTadFUPfMdYsYtkUDbX5mNCMVHMZtsCnFeb"},
      { ipv4: "18.216.6.197",     port: "33445",  publicKey: "H8sqhRrQuJZ6iLtP2wanxt4LzdNrN2NNFnpPdq1uJ9n2"},
      { ipv4: "52.83.171.135",    port: "33445",  publicKey: "5tuHgK1Q4CYf4K5PutsEPK5E3Z7cbtEBdx7LwmdzqXHL"},
      { ipv4: "52.83.191.228",    port: "33445",  publicKey: "3khtxZo89SBScAMaHhTvD68pPHiKxgZT6hTCSZZVgNEm"}
    ],

    ready : false,

    session : null,
    session_ctx : {
      remote_sdp: "",
      unchanged_streams: 0,
      need_start: false,
    },

    init(id, store, CarrierModel, config={}){
      if(!id){
        throw new Error('invalid carrier id');
      }

      F.init_args = [id, store, CarrierModel, config];
      F.id = hash(id);
      F.store = store;
      F.model = CarrierModel;

      const opts = {
        udpEnabled: true,
        persistentLocation: ".ela_data/"+F.id,
        bootstraps: F.bootstraps
      };

      _log.debug('create carrier service with id = '+id);

      F.callbacks = F.buildCallback();
      F.carrier = SDK.createObject(opts, F.callbacks);
      F.carrier.run();

      F.initData();

      // init session
      F.session_initSession();
    },

    buildCallback(){
      return {
        idle: ()=>{

        },
        connectionStatus: (carrier, status, context)=>{
          switch (status) {
            case SDK.ConnectionStatus.CONNECTED:
              _log.debug("Connected to carrier network.");
              F.syncData('me/online', {
                online : true
              });
              break;

            case SDK.ConnectionStatus.DISCONNECTED:
              _log.debug("Disconnect from carrier network.");
              F.syncData('me/online', {
                online : false
              });

              // reconnect

              break;
            default:
              _log.debug("Error!!! Got unknown connection status :" + status);
          }
          _log.debug('carrier node is ready : '+F.carrier.isReady());
          F.ready = F.carrier.isReady();
        },
        friendsList: (carrier, friend_info, context)=>{
          try{
            if(friend_info){
              const d = F.model.create('Friend', friend_info);
              F.syncData('friend/list/callback', {
                end : 0,
                info : d.getData()
              });
            }
            else {
              F.syncData('friend/list/callback', {
                end : 1
              });
            }
            return true;
          }catch(e){
            _log.error(e);
          }

        },
        friendConnection: (carrier, friendid,  status, context)=>{
          switch (status) {
            case SDK.ConnectionStatus.CONNECTED:
              _log.debug("Friend [" + friendid +"] connection changed to be online");
              F.syncData('friend/status/callback', {
                userId : friendid,
                status : 0,
                online : true
              });

              break;

            case SDK.ConnectionStatus.DISCONNECTED:
              _log.debug("Friend [" + friendid +"] connection changed to be offline.");
              F.syncData('friend/status/callback', {
                userId : friendid,
                status : 1,
                online : false
              });
              break;

            default:
              _log.error("Error!!! Got unknown connection status:" + status);
          }
          return true;
        },
        friendInfo: (carrier, friendId, info, context)=>{
          _log.debug("Friend information changed: "+friendId);

          const fi = F.model.create('Friend', info);
          F.syncData('friend/info/callback', fi.getData());
        },
        friendPresence: (carrier, friendid,  status, context)=>{
          if(status >= SDK.PresenceStatus_None && status <= SDK.PresenceStatus_Busy){
            _log.debug("Friend[" + friendid + "] change presence to " + F.presence_name[status]);
            F.syncData('friend/status/callback', {
              userId : friendid,
              presence : status
            });
          }
          else{
            _log.debug("Error!!! Got unknown presence status %d.", status);
          }
        },
        friendRequest: (carrier, userid, info, hello, context)=>{
          _log.debug("Friend request from user[" + info.userId + "] with : " + hello + ".");
          // this.log("Reply use following commands:");
          // this.log("  faccept " + userid);
          const u = F.model.create('User', info);

          F.syncData('friend/apply/callback', {
            msg : hello,
            ...u.getData()
          });
        },
        friendAdded: (carrier, info, context)=>{
          const f_info = F.model.create('Friend', info);
          // _log.debug("New friend added.");

          F.syncData('friend/add/callback', f_info.getData());
        },
        friendRemoved: (carrier, friend_id, context)=>{
          _log.debug("Friend " + friend_id +  " removed!");
          F.syncData('friend/remove/callback', friend_id);
        },
        friendMessage: (carrier, from, msg, context)=>{
          _log.debug("Message from friend[" + from + "]: " + msg);
          F.syncData('friend/message/callback', {
            user : F.getFriendInfo(from),
            msg
          });

        },
        friendInvite: (carrier, from, msg, context)=>{
          _log.debug("Friend invited from friend[" + from + "] with : " + msg);
        },
        friendsIterate: (...args)=>{
          console.log('[friendsIterate]', ...args);
        },

        friendInviteResponse : (carrier, friendid, status, reason, data, context)=>{
          _log.debug("Got invite response from " + friendid + ".");
          if (status === 0) {
            _log.debug("message within response: " + data);
          }
          else {
            _log.debug("refused: " + reason);
          }
        },

        session_request_callback : (carrier, from, sdp)=>{
          F.session_ctx.remote_sdp = sdp;

          const log = ` \n
            Session request from[${from}] with SDP:  \n
            ${sdp} \n
            
            Reply use following commands: \n
              1. snew ${from} \n
              2. sreply refuse [reason] \n
            OR:
              1. snew ${from} \n
              2. sadd [plain] [reliable] [multiplexing] [portforwarding] \n
              3. sreply ok \n
          `;

          _log.debug(log);

        },
        session_request_complete_callback : (session, status, reason, sdp)=>{
          if(status !== 0){
            _log.debug("Session complete, status: " + status + ", reason:" + reason);
          }
          else{
            F.session_ctx.remote_sdp = sdp;
            const ret = F.session.start(sdp);
            _log.debug("Session start " + (ret ? "success" : "failed"));
          }
        },
        stream_on_state_changed : (stream, state)=>{
          const state_name = [
            "raw",
            "initialized",
            "transport_ready",
            "connecting",
            "connected",
            "deactivated",
            "closed",
            "failed"
          ];
          _log.debug("Stream [" + stream.id + "] state changed to: " + state_name[state]);

          if(state == SDK.StreamState.TRANSPORT_READY){
            --F.session_ctx.unchanged_streams;
            if((F.session_ctx.unchanged_streams == 0) && (F.session_ctx.need_start)){
              const ret = F.session.start(F.session_ctx.remote_sdp);
              F.session_ctx.need_start = false;
              _log.debug("Session start " + (ret ? "success." : "failed."));
            }
          }
        },
        stream_on_data : (stream, data)=>{
          _log.debug("Stream [" + stream.id + "] received data ["+ data.toString() + "]");

          // TODO sync data
        },

        on_channel_open : (stream, channel_id, cookie)=>{
          _log.debug("Stream request open new channel:" + channel_id + " cookie:" + cookie);
        },
        on_channel_opened : (stream, channel_id)=>{
          _log.debug("Channel " + stream.id + ":" + channel_id + " opened.");
        },
        on_channel_close : (stream, channel_id, reason)=>{
          _log.debug("Channel " + stream.id + ":" + channel_id + " closed.");
        },
        on_channel_data : (stream, channel_id, data)=>{
          _log.debug("Channel " + stream.id + ":" + channel_id + " received data [" + data.toString() + "]");
        },
        on_channel_pending : (stream, channel_id)=>{
          _log.debug("Channel " + stream.id + ":" + channel_id + " is pending.");
        },
        on_channel_resume : (stream, channel_id)=>{
          _log.debug("Channel " + stream.id + ":" + channel_id + " resumed.");
        }
      };
    },

    syncData(type, data){
      F.store.dispatch('carrier_data', {
        data,
        type
      });
    },

    initData(){
      //get self info

      const me = F.getSelfInfo();
      F.syncData('me/info', me);

      //get address
      const address = F.carrier.getAddress();
      F.syncData('me/address', address);

      //get self presence
      const presence = F.getSelfPresence();
      F.syncData('me/presence', presence);

      F.ready = true;
    },

    execute(method, ...args){
      if(!method){
        throw new Error('invalid method name');
      }

      const fn = F[method] || F.carrier[method];
      try{
        return fn(...args);
      }catch(e){
        console.error(e, SDK.getError().toString(16));
        throw new Error(e, SDK.getError().toString(16));
      }

    },



    // method list
    getSelfInfo(){
      const me_data = F.carrier.getSelfInfo();
      const me = F.model.create('User', me_data);
      return me.getData();
    },
    setSelfInfo(data){
      const info = F.getSelfInfo();
      F.carrier.setSelfInfo(_.extend(info, data));

      return info;
    },
    addFriend(address, msg){
      return F.carrier.addFriend(address, msg);
    },
    getFriendInfo(userId){
      let info = F.carrier.getFriendInfo(userId);
      info = F.model.create('Friend', info);
      return info.getData();
    },
    getAddress(){
      return F.carrier.getAdaptor();
    },
    getFriends(){
      return F.carrier.getFriends(F.callbacks.friendsList, null);
    },
    sendFriendMessage(userId, msg){
      const rs = F.carrier.sendFriendMessage(userId, msg);
      return !!rs;
    },

    acceptFriend(userId){
      const rs = F.carrier.acceptFriend(userId);
      return !!rs;
    },

    removeFriend(userId){
      const rs = F.carrier.removeFriend(userId);
      return !!rs;
    },
    setFriendLabel(userId, label){
      const rs = F.carrier.setFriendLabel(userId, label);
      return !!rs;
    },

    inviteFriend(userId, msg){

      const rs = F.carrier.inviteFriend(userId, msg, F.callbacks.friendInviteResponse, null);
      return !!rs;
    },

    getSelfPresence(){
      const rs = F.carrier.getSelfPresence();
      return rs;
    },

    setSelfPresence(status){
      let presence = null;
      if(status === 0){
        presence = SDK.PresenceStatus_None;
      }
      else if(status === 1) {
        presence = SDK.PresenceStatus_Away;
      }
      else if(status === 2) {
        presence = SDK.PresenceStatus_Busy;
      }
      else{
        throw new Error('invalid status : '+status);
      }

      return !!F.carrier.setSelfPresence(presence);
    },

    session_initSession(){
      F.session_ctx.remote_sdp = '';
      const ret = F.carrier.initSession(F.callbacks.session_request_callback);
      if(!ret){
        throw 'Session initialized failed.';
      }

      _log.debug('Session initialized successfully.');
      return !!ret;
    },
    session_newSession(friendId){
      if(!friendId){
        throw 'invalid friend id';
      }
      F.session = F.carrier.newSession(friendId);

      _log.debug("Create session successfully.");
      F.session.stream = [];
      F.session_ctx.need_start = false;
      F.session_ctx.unchanged_streams = 0;
      return true;
    },
    session_getPeer(){
      const peer = F.session.getPeer();
      if(!peer){
        throw ("Get peer failed.");
      }
      _log.debug("Get peer: " + peer);
      return peer;
    },
    session_request(){
      const ret = F.session.request(F.callbacks.session_request_complete_callback);
      if(!ret){
        throw ("session request failed.");
      }
      _log.debug("session request successfully.");
      return true;
    },
    session_replyRequest(accept, reason=''){
      let ret;
      if(accept){
        ret = F.session.replyRequest(0, null);
        if(!ret){
          throw '[accept] response invite failed.';
        }
        _log.debug("[accept] response invite successfully.");
        if(F.session_ctx.unchanged_streams > 0) {
          F.session_ctx.need_start = true;
        }
        else{
          ret = F.session.start(F.session_ctx.remote_sdp);
          _log.debug("Session start " + (ret ? "success." : "failed."));
        }
      }
      else{
        ret = F.session.replyRequest(1, reason);
        if(!ret){
          throw '[reject] response invite failed.';
        }
        _log.debug("[reject] response invite successfully.");
      }

      return true;
    },
    session_close(){
      F.session.close();
      F.session = null;
      _log.debug("Session closed.");
    },
    session_cleanupSession(){
      F.carrier.cleanupSession();
      _log.debug("Session cleaned up.");
    },

    /*
    * types =>
    * [plain] [reliable] [multiplexing] [portforwarding]
    *
    * streamType =>
    * AUDIO = 0
    * VIDEO = 1
    * TEXT = 2
    * APPLICATION = 3
    * MESSAGE = 4
    *
    * */
    session_addStream(types=['reliable'], streamType='TEXT'){
      const argv = types;
      let param = 0;

      const callbacks = {
        stateChanged : F.callbacks.stream_on_state_changed,
        streamData : F.callbacks.stream_on_data
      };

      if(argv.length > 0){
        for(let i = 0; i < argv.length; i++){
          const key = argv[i];
          if(key === 'reliable'){
            param |= SDK.StreamMode.RELIABLE;
          }
          else if(key === 'plain') {
            param |= SDK.StreamMode.PLAIN;
          }
          else if(key === 'multiplexing'){
            param |= SDK.StreamMode.MULTIPLEXING;
          }
          else if(key === 'portforwarding'){
            param |= SDK.StreamMode.PORT_FORWARDING;
          }
          else{
            throw ('invalid session_add type => '+key);
          }
        }
      }

      if ((param & SDK.StreamMode.MULTIPLEXING) || (param & SDK.StreamMode.PORT_FORWARDING)){
        callbacks.channelOpen = F.callbacks.on_channel_open;
        callbacks.channelOpened = F.callbacks.on_channel_opened;
        callbacks.channelData = F.callbacks.on_channel_data;
        callbacks.channelPending = F.callbacks.on_channel_pending;
        callbacks.channelResume = F.callbacks.on_channel_resume;
        callbacks.channelClose = F.callbacks.on_channel_close;
      }

      const stream = F.session.addStream(SDK.StreamType[streamType], param, callbacks);
      if(!stream){
        throw ("Add stream failed.");
      }
      F.session_ctx.unchanged_streams++;
      stream.channel = [];
      F.session.stream[stream.id] = stream;
      _log.debug("Add stream successfully and stream id " + stream.id);

      return stream;
    },

    stream_remove(streamId){
      const stream = F.session.stream[streamId];
      if(!stream){
        throw ("stream " + streamId + " is invalid.");
      }
      const ret = stream.remove();
      if(!ret){
        throw("Remove stream " + stream.id + " failed.");
      }
      _log.debug("Remove stream " + stream.id + " success.");
      return true;
    },
    stream_write(streamId, buffer){
      const stream = F.session.stream[streamId];
      if(!stream){
        throw ("stream " + streamId + " is invalid.");
      }

      const rc = stream.write(buffer);
      if(rc < 0){
        throw ("write data failed.");
      }
      _log.debug("write data successfully.");

      return true;
    },
    stream_getTransportInfo(streamId){
      const topology_name = [
        "LAN",
        "P2P",
        "RELAYED"
      ];

      const addr_type = [
        "HOST   ",
        "SREFLEX",
        "PREFLEX",
        "RELAY  "
      ];

      const stream = F.session.stream[streamId];
      if(!stream){
        throw ("stream " + streamId + " is invalid.");
      }
      const info = stream.getTransportInfo();
      if(!info){
        throw ("get remote addr failed.");
      }

      console.log(info);

      const log = `
        Stream transport information:
          Network: ${topology_name[info.topology]}
          Local: ${addr_type[info.local.type]} ${info.local.address} : ${info.local.port}
          related ${info.local.relatedAddress} : ${info.local.relatedPort}
          Remote: ${addr_type[info.remote.type]} ${info.remote.address} : ${info.remote.port}
          related ${info.remote.relatedAddress} : ${info.remote.relatedPort}
      `;

      _log.debug(log);
      return true;
    },
    stream_getType(streamId){
      const type_name = [
        "audio",
        "video",
        "text",
        "application",
        "message"
      ];

      const stream = F.session.stream[streamId];
      if(!stream){
        throw ("stream " + streamId + " is invalid.");
      }
      const type = stream.getType();
      if(!type){
        throw ("get type failed.");
      }

      _log.debug("Stream type: " + type_name[type]);

      return type_name[type];
    },

    stream_getState(streamId){
      const state_name = [
        "raw",
        "initialized",
        "transport_ready",
        "connecting",
        "connected",
        "deactivated",
        "closed",
        "failed"
      ];

      const stream = F.session.stream[streamId];
      if(!stream){
        throw ("stream " + streamId + " is invalid.");
      }
      const state = stream.getState();
      if(!state){
        throw ("get state failed.");
      }

      _log.debug("Stream state: " + state_name[state]);
      return state_name[state];
    },

    close(){
      console.log('kill carrier node');
      try{
        F.session_close();
        F.carrier.destroy();
      }catch(e){
        console.error(e);
      }
      F.ready = false;
    },



    restart(){
      F.close();

      _log.debug('restart carrier node');
      setTimeout(()=>{
        F.init.apply(F, F.init_args);
      }, 2000);
    }
  };


  this.CarrierService = F;
})();

