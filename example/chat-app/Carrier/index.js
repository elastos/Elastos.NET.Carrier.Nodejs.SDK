
(function(){
  const SDK = require('elastos_carrier_addon');
  const _ = require('lodash');
  const _log = {
    debug : console.log,
    error : console.error
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

              break;

            case SDK.ConnectionStatus_Disconnected:
              _log.debug("Disconnect from carrier network.");

              break;
            default:
              _log.debug("Error!!! Got unknown connection status :" + status);
          }
        },
        friendsList: (carrier, friend_info, context)=>{
          try{
            if(friend_info){

            }
            else {

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


              break;

            case SDK.ConnectionStatus_Disconnected:
              _log.debug("Friend [" + friendid +"] connection changed to be offline.");

              break;

            default:
              _log.error("Error!!! Got unknown connection status:" + status);
          }
          return true;
        },
        friendInfo: (carrier, friendId, info, context)=>{
          _log.debug("Friend information changed: "+friendId);


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

        },
        friendAdded: (carrier, info, context)=>{
          _log.debug("New friend added. The friend information:");

        },
        friendRemoved: (carrier, friend_id, context)=>{
          _log.debug("Friend " + friend_id +  " removed!");

        },
        friendMessage: (carrier, from, msg, context)=>{
          _log.debug("Message from friend[" + from + "]: " + msg);


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
      const me_data = F.carrier.getSelfInfo();
      const me = F.model.create('User', me_data);
      F.syncData('me/info', me.getData());

      //get address
      const address = F.carrier.getAddress();
      F.syncData('me/address', address);

      F.ready = true;
    }
  };


  this.CarrierService = F;
})();

