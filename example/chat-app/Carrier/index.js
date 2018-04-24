
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
      { ipv4: "54.223.36.193",    port: "33445",  publicKey: "5tuHgK1Q4CYf4K5PutsEPK5E3Z7cbtEBdx7LwmdzqXHL"},
      { ipv4: "52.80.187.125",    port: "33445",  publicKey: "3khtxZo89SBScAMaHhTvD68pPHiKxgZT6hTCSZZVgNEm"}
    ],

    ready : false,

    init(id, store, CarrierModel, config={}){
      if(!id){
        throw new Error('invalid carrier id');
      }

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
    },

    buildCallback(){
      return {
        idle: ()=>{

        },
        connectionStatus: (carrier, status, context)=>{
          switch (status) {
            case SDK.ConnectionStatus_Connected:
              _log.debug("Connected to carrier network.");
              F.syncData('me/online', {
                online : true
              });
              break;

            case SDK.ConnectionStatus_Disconnected:
              _log.debug("Disconnect from carrier network.");
              F.syncData('me/online', {
                online : false
              });
              break;
            default:
              _log.debug("Error!!! Got unknown connection status :" + status);
          }
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
            case SDK.ConnectionStatus_Connected:
              _log.debug("Friend [" + friendid +"] connection changed to be online");
              F.syncData('friend/status/callback', {
                userId : friendid,
                status : 0,
                online : true
              });

              break;

            case SDK.ConnectionStatus_Disconnected:
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
          if (status >= SDK.PresenceStatus_None &&
            status <= SDK.PresenceStatus_Busy) {
            _log.debug("Friend[" + friendid + "] change presence to " + presence_name[status]);
          }
          else {
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
          _log.debug("New friend added.");

          F.syncData('friend/add/callback', f_info.getData());
        },
        friendRemoved: (carrier, friend_id, context)=>{
          _log.debug("Friend " + friend_id +  " removed!");
          F.syncData('friend/remove/callback', friend_id);
        },
        friendMessage: (carrier, from, msg, context)=>{
          _log.debug("Message from friend[" + from + "]: " + msg);
          F.syncData('friend/message/callback', {
            userId : from,
            msg
          });

        },
        friendInvite: (carrier, from, msg, context)=>{
          _log.debug("Message from friend[" + from + "]: " + msg);
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
        console.error(e, F.carrier.getError().toString(16));
        throw new Error(e, F.carrier.getError().toString(16));
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

    removeFriend(userId) {
      const rs = F.carrier.removeFriend(userId);
      return !!rs;
    },
    close(){
      F.carrier.destory();
    }
  };


  this.CarrierService = F;
})();

