import Base from './Base';
import SDK from 'elastos_carrier_addon';
import UserClass from '../model/User';
import FriendClass from '../model/Friend';
import _ from 'lodash';
import md5 from 'md5';
import socket from '../socket/WebSocket';

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
    { ipv4: "52.83.171.135",    port: "33445",  publicKey: "5tuHgK1Q4CYf4K5PutsEPK5E3Z7cbtEBdx7LwmdzqXHL"},
    { ipv4: "52.83.191.228",    port: "33445",  publicKey: "3khtxZo89SBScAMaHhTvD68pPHiKxgZT6hTCSZZVgNEm"}
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

        this.id = md5(id);

        const opts = {
            udpEnabled: true,
            persistentLocation: ".data/"+this.id,
            bootstraps: bootstraps
        };

        this.socket = socket.get(id);

        this.socket_message = (data)=>{
            // this._socket.send(this.id, 'elastos_log', data);
        };

        console.log('create carrier service with id = '+id);

        this.callbacks = this.buildCallback();
        this.carrier = SDK.createObject(opts, this.callbacks);
        this.carrier.run();
    }

    log(info){
        let str = '';
        if(_.isString(info)){
            str = info;
            console.log(str);
            this.socket_message(str);
        }
        else{
            // _.each(info, (value, key)=>{
            //     str = `[ ${key} ] => ${value}`;
            //     console.log(str);
            //     this.socket_message(str);
            // });
            str = JSON.stringify(info);
            console.log(str);
            this.socket_message(str);
        }

    }

    socket_data(type, data){
        // this._socket.send(this.id, 'elastos_data', {
        //     ...data,
        //     elastos_type: type
        // });

    }
    toClient(type, data){
        this.socket.send({
            ...this.socket.result(data),
            type
        })
    }

    buildCallback(){
        return {
            idle: ()=>{

            },
            connectionStatus: (carrier, status, context)=>{
                switch (status) {
                    case SDK.ConnectionStatus_Connected:
                        // this.log("Connected to carrier network.");
                        this.toClient('me/online', {
                            online : true
                        });
                        break;

                    case SDK.ConnectionStatus_Disconnected:
                        // this.log("Disconnect from carrier network.");
                        this.toClient('me/online', {
                            online : false
                        });
                        break;

                    default:
                        this.log("Error!!! Got unknown connection status :" + status);
                }
            },
            friendsList: (carrier, friend_info, context)=>{
                try{
                    if(friend_info){
                        const d = this.show_friend_info(friend_info);
                        // this.socket_data('friend_list', d);
                        this.toClient('friend/list/callback', {
                           end : 0,
                           info : d
                        });
                    }
                    else {
                        /* The list ended */
                        this.toClient('friend/list/callback', {
                            end : 1
                        });
                    }
                    return true;
                }catch(e){
                    console.error(e);
                }

            },
            friendConnection: (carrier, friendid,  status, context)=>{
                switch (status) {
                    case SDK.ConnectionStatus_Connected:
                        console.log("Friend [" + friendid +"] connection changed to be online");

                        //TOOD why call this.info_friend(friendid) here will throw an error.

                        this.toClient('friend/status/callback', {
                            userId : friendid,
                            status : 0,
                            online : true
                        });
                        break;

                    case SDK.ConnectionStatus_Disconnected:
                        console.log("Friend [" + friendid +"] connection changed to be offline.");
                        this.toClient('friend/status/callback', {
                            userId : friendid,
                            status : 1,
                            online : false
                        });
                        break;

                    default:
                        this.log("Error!!! Got unknown connection status:" + status);
                }
                return true;
            },
            friendInfo: (carrier, friendId, info, context)=>{
                console.log("Friend information changed: "+friendId);

                const fi = new FriendClass(info);
                this.toClient('friend/info/callback', fi.getData());
            },
            friendPresence: (carrier, friendid,  status, context)=>{
                if (status >= SDK.PresenceStatus_None &&
                    status <= SDK.PresenceStatus_Busy) {
                    this.log("Friend[" + friendid + "] change presence to " + presence_name[status]);
                }
                else {
                    this.log("Error!!! Got unknown presence status %d.", status);
                }
            },
            friendRequest: (carrier, userid, info, hello, context)=>{
                console.log("Friend request from user[" + info.userId + "] with : " + hello + ".");
                // this.log("Reply use following commands:");
                // this.log("  faccept " + userid);

                const u = new UserClass(info);

                this.toClient('friend/apply/callback', {
                    msg : hello,
                    ...u.getData()
                });
            },
            friendAdded: (carrier, info, context)=>{
                //this.log("New friend added. The friend information:");
                const f_info = new FriendClass(info);
                this.toClient('friend/add/callback', f_info.getData());
            },
            friendRemoved: (carrier, friend_id, context)=>{
                console.log("Friend " + friend_id +  " removed!");

                this.toClient('friend/remove/callback', friend_id);
            },
            friendMessage: (carrier, from, msg, context)=>{
                // console.log("Message from friend[" + from + "]: " + msg);

                this.toClient('friend/message/callback', {
                    userId : from,
                    msg
                });
            },
            friendInvite: (carrier, from, msg, context)=>{
                this.log("Message from friend[" + from + "]: " + msg);
            }
        };
    }

    show_friend_info(info){
        const ff = new FriendClass(info);

        // this.log(ff.getData());

        return ff.getData();
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
                throw ("Set user's presence failed (0x" + SDK.getError().toString(16) + ").");
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
            // TODO fix getError is not defined
            // if added, return ela_add_friend error:0x8100000C error
            throw new Error(address + ' was added, please wait for response');
        }
    }

    info_friend(userid){
        if(!userid){
           throw new Error('invalid userid for show_friend');
        }

        const info = this.carrier.getFriendInfo(userid);
        if (!info) {
            throw ("Get friend information failed(0x" +  SDK.getError().toString(16) + ").");
        }

        const fi = new FriendClass(info);
        return fi.getData();
    }

    list_friend(){
        this.carrier.getFriends(this.callbacks.friendsList, null);
    }

    send_message(userid, msg){
        const rs = this.carrier.sendFriendMessage(userid, msg);
        if(rs){
            return true;
        }
        else
            throw ("Send message failed(0x" +  SDK.getError().toString(16) + ").");
    }

    accept_friend(userid){
        const rs = this.carrier.acceptFriend(userid);
        if(rs){
            return true;
        }
        else{
            throw ("Accept friend request failed(0x" +  SDK.getError().toString(16) + ").");
        }
    }

    remove_friend(userid) {
        const rs = this.carrier.removeFriend(userid);
        if(rs){
            return true;
        }
        else{
            throw ("Remove friend %s failed (0x" + SDK.getError().toString(16) + ").");
        }
    }

    close(){
        this.carrier.destory();
    }
}