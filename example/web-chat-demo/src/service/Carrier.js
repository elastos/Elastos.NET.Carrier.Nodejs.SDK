import Base from './Base';
import SDK from 'elastos_carrier_addon';
import UserClass from '../model/User';
import FriendClass from '../model/Friend';

const connection_name = [
    "online",
    "offline"
];

const presence_name = [
    "none",    // None;
    "away",    // Away;
    "busy",    // Busy;
];

const bootstraps = [
    { ipv4: "13.58.208.50",     port: "33445",  publicKey: "89vny8MrKdDKs7Uta9RdVmspPjnRMdwMmaiEW27pZ7gh"},
    { ipv4: "18.216.102.47",    port: "33445",  publicKey: "G5z8MqiNDFTadFUPfMdYsYtkUDbX5mNCMVHMZtsCnFeb"},
    { ipv4: "18.216.6.197",     port: "33445",  publicKey: "H8sqhRrQuJZ6iLtP2wanxt4LzdNrN2NNFnpPdq1uJ9n2"},
    { ipv4: "54.223.36.193",    port: "33445",  publicKey: "5tuHgK1Q4CYf4K5PutsEPK5E3Z7cbtEBdx7LwmdzqXHL"},
    { ipv4: "52.80.187.125",    port: "33445",  publicKey: "3khtxZo89SBScAMaHhTvD68pPHiKxgZT6hTCSZZVgNEm"}
];

const F = {
    fmt_str_blank(string, length){
        for(let i = string.length; i < length; i++) {
            string += " "
        }
        return string;
    }
};

export default class extends Base{
    init(id){
        if(!id){
            throw new Error('invalid carrier id');
        }

        this.id = id;

        const opts = {
            udpEnabled: true,
            persistentLocation: ".data/"+this.id,
            bootstraps: bootstraps
        };

        this.carrier = SDK.createObject(opts, this.buildCallback());
        this.carrier.run();
    }

    buildCallback(){
        return {
            idle: ()=>{

            },
            connectionStatus: (carrier, status, context)=>{
                console.log('[connectionStatus]', status, context);
                switch (status) {
                    case SDK.ConnectionStatus_Connected:
                        console.log("Connected to carrier network.");
                        break;

                    case SDK.ConnectionStatus_Disconnected:
                        console.log("Disconnect from carrier network.");
                        break;

                    default:
                        console.log("Error!!! Got unknown connection status :" + status);
                }
            },
            friendsList: (carrier, friend_info, context)=>{
                console.log("Friends list from carrier network:");
                if(friend_info){
                    this.show_friend_info(friend_info);
                }
                else {
                    /* The list ended */
                    console.log("  ----------------");
                    console.log(context);
                    console.log("  ----------------");
                }
                return true;
            },
            friendConnection: (carrier, friendid,  status, context)=>{
                switch (status) {
                    case SDK.ConnectionStatus_Connected:
                        console.log("Friend[" + friendid +"] connection changed to be online");
                        break;

                    case SDK.ConnectionStatus_Disconnected:
                        console.log("Friend[" + friendid +"] connection changed to be offline.");
                        break;

                    default:
                        console.log("Error!!! Got unknown connection status:" + status);
                }
                return true;
            },
            friendInfo: (carrier, friendId, info, context)=>{
                console.log("Friend information changed: "+friendId);
                this.show_friend_info(info);
            },
            friendPresence: (carrier, friendid,  status, context)=>{
                if (status >= SDK.PresenceStatus_None &&
                    status <= SDK.PresenceStatus_Busy) {
                    console.log("Friend[" + friendid + "] change presence to " + presence_name[status]);
                }
                else {
                    console.log("Error!!! Got unknown presence status %d.", status);
                }
            },
            friendRequest: (carrier, userid, info, hello, context)=>{
                console.log("Friend request from user[" + info.name ? info.name : userid + "] with HELLO: " + hello + ".");
                console.log("Reply use following commands:");
                console.log("  faccept " + userid);
            },
            friendAdded: (carrier, info, context)=>{
                console.log("New friend added. The friend information:");
                this.show_friend_info(info);
            },
            friendRemoved: (carrier, friend_id, context)=>{
                console.log("Friend " + friend_id +  " removed!");
            },
            friendMessage: (carrier, from, msg, context)=>{
                console.log("Message from friend[" + from + "]: " + msg);
            },
            friendInvite: (carrier, from, msg, context)=>{
                console.log("Message from friend[" + from + "]: " + msg);
            }
        };
    }

    show_friend_info(info){
        const ff = new UserClass(info.userInfo);
        console.log("  friend info: ", ff.getData());
        console.log("     Presence: %s", presence_name[info.presence]);
        console.log("   Connection: %s", connection_name[info.status]);

    }

    self_info(){
        const me = new UserClass(this.carrier.getSelfInfo());

        return me.getData();
    }
    set_self_info(key, value=''){
        let info = this.self_info();
        info[key] = value;
        this.carrier.setSelfInfo(info);
        return info;
    }
    self_presence(status=''){
        let presence;
        if(!status) {
            presence = this.carrier.getSelfPresence();
            return presence_name[presence];
        }
        else if (status) {
            if (status === "none") {
                presence = SDK.PresenceStatus_None;
            }
            else if (status === "away") {
                presence = SDK.PresenceStatus_Away;
            }
            else if (status === "busy") {
                presence = SDK.PresenceStatus_Busy;
            }
            else {
                throw ("Invalid command syntax.");
            }

            if (this.carrier.setSelfPresence(presence)) {
                return true;
            }
            else {
                throw ("Set user's presence failed (0x" + this.carrier.getError().toString(16) + ").");
            }
        }
        else {
            throw("Invalid command syntax.");
        }
    }
    self_address(){
        return this.carrier.getAddress();
    }

    self_nodeid(){
        return this.carrier.getNodeId();
    }

    self_userid(){
        return this.carrier.getUserId();
    }
    add_friend(address, msg){
        if(this.carrier.addFriend(address, msg)){
            return true;
        }
        else{
            throw ("Request to add a new friend failed(0x" + this.carrier.getError().toString(16) + ").");
        }
    }

    show_friend(userid){
        if(!userid){
           throw new Error('invalid userid for show_friend');
        }

        const info = carrier.getFriendInfo(userid);
        if (!info) {
            throw ("Get friend information failed(0x" +  carrier.getError().toString(16) + ").");
        }

        const fi = new FriendClass(info);
        return fi.getData();
    }

    list_frined(){
        //carrier.getFriends(friends_list_callback, null);
    }

    send_message(userid, msg){
        const rs = this.carrier.sendFriendMessage(userid, msg);
        if(rs){
            return true;
        }
        else
            throw ("Send message failed(0x" +  carrier.getError().toString(16) + ").");
    }

    accept_friend(userid){
        const rs = this.carrier.acceptFriend(userid);
        if(rs){
            return true;
        }
        else{
            throw ("Accept friend request failed(0x" +  carrier.getError().toString(16) + ").");
        }
    }

    remove_friend(userid) {
        const rs = this.carrier.removeFriend(userid);
        if(rs){
            return true;
        }
        else{
            throw ("Remove friend %s failed (0x" +  carrier.getError().toString(16) + ").");
        }
    }
}