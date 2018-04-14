// process.env.UV_THREADPOOL_SIZE = 8

const carrierSdk = require('elastos_carrier_addon');
const _ = require('lodash');
const UserClass = require('./src/model/User');

//Data define
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

const opts = {
    udpEnabled: true,
    persistentLocation: ".data",
    bootstraps: bootstraps
};

// Command Functions
const r1 = {};
//
// var commands = [
//     { cmd:"help",       fn:help,                   help:"help [cmd]" },
//
//     { cmd:"address",    fn:get_address,            help:"address" },
//     { cmd:"nodeid",     fn:get_nodeid,             help:"nodeid" },
//     { cmd:"userid",     fn:get_userid,             help:"userid" },
//     { cmd:"me",         fn:self_info,              help:"me [set] [name | description | gender | phone | email | region] [value]" },
//     { cmd:"nospam",     fn:self_nospam,            help:"nospam [ value ]" },
//     { cmd:"presence",   fn:self_presence,          help:"presence [ none | away | busy ]" },
//
//     { cmd:"fadd",       fn:friend_add,             help:"fadd address hello" },
//     { cmd:"faccept",    fn:friend_accept,          help:"faccept userid" },
//     { cmd:"fremove",    fn:friend_remove,          help:"fremove userid" },
//     { cmd:"friends",    fn:list_friends,           help:"friends" },
//     { cmd:"friend",     fn:show_friend,            help:"friend userid" },
//     { cmd:"label",      fn:label_friend,           help:"label userid name" },
//     { cmd:"msg",        fn:send_message,           help:"msg userid message" },
//     { cmd:"invite",     fn:invite,                 help:"invite userid data" },
//     { cmd:"ireply",     fn:reply_invite,           help:"ireply userid [confirm message | refuse reason]" },
//     { cmd:"exit",       fn:exit,                   help:"exit" }
// ]
//
// function do_command(input) {
//     var args = input.trim().split(' ');
//     if (args[0] == "") {
//         rl.prompt();
//         return;
//     }
//
//     for (var i = 0; i < commands.length; i++) {
//         if (commands[i].cmd == args[0]) {
//             commands[i].fn(args);
//             return;
//         }
//     }
//     console.log("Unknown command:" + args[0]);
// }

// function help(args) {
//     if (args.length > 1) {
//         for (var i = 0; i < commands.length; i++) {
//             if (commands[i].cmd == args[1]) {
//                 console.log("Usage: :" + commands[i].help);
//                 return;
//             }
//         }
//         console.log("Usage: " + commands[0].help);
//     }
//     else {
//         console.log("Available commands list:");
//         for (var i = 0; i < commands.length; i++) {
//             console.log("    " + commands[i].help);
//         }
//     }
// }

function exit(args) {
    carrier.destory();
    process.exit(0);
}

function display_user_info(info) {

}


function display_friend_info(info) {
    display_user_info(info.userInfo);
    console.log("     Presence: %s", presence_name[info.presence]);
    console.log("   Connection: %s", connection_name[info.status]);
}




function self_nospam(argv) {
    if (argv.length == 1) {
        console.log(carrier.getSelfNospam());
    }
    else if (argv.length == 2) {
        var nospam = parseInt(argv[1]);
        if (isNaN(nospam) || nospam == 0) {
            console.log("Invalid nospam value to set.");
            return;
        }

        if (carrier.setSelfNospam(nospam)) {
            console.log("User's nospam changed to be " + nospam + ".");
        }
        else {
            console.log("Set user's nospam failed (0x" + carrier.getError().toString(16) + ").");
        }
    }
    else {
        console.log("Invalid command syntax.");
    }
}

function self_presence(argv) {

}

function friend_add(argv) {

}

function friend_accept(argv) {
    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.acceptFriend(argv[1]))
        console.log("Accept friend request success.");
    else
        console.log("Accept friend request failed(0x" +  carrier.getError().toString(16) + ").");
}

function friend_remove(argv) {
    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.removeFriend(argv[1]))
        console.log("Remove friend %s success.", argv[1]);
    else
        console.log("Remove friend %s failed (0x" +  carrier.getError().toString(16) + ").");
}

function list_friends(argv) {
    if (argv.length != 1) {
        console.log("Invalid command syntax.");
        return;
    }

    carrier.getFriends(friends_list_callback, null);
}

function show_friend(argv) {
    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    info = carrier.getFriendInfo(argv[1]);
    if (!info) {
        console.log("Get friend information failed(0x" +  carrier.getError().toString(16) + ").");
        return;
    }

    console.log("Friend %s information:", argv[1]);
    display_friend_info(info);
}

function label_friend(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.setFriendLabel(argv[1], argv[2]))
        console.log("Update friend label success.");
    else
        console.log("Update friend label failed(0x" +  carrier.getError().toString(16) + ").");
}

function send_message(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.sendFriendMessage(argv[1], argv[2]))
        console.log("Send message success.");
    else
        console.log("Send message failed(0x" +  carrier.getError().toString(16) + ").");
}

function invite(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.inviteFriend(argv[1], argv[2], invite_response_callback, null))
        console.log("Send invite request success.");
    else
        console.log("Send invite request failed(0x" +  carrier.getError().toString(16) + ").");
}

function reply_invite(argv) {
    var status = 0;
    var reason = null;
    var msg = null;

    if (argv.length != 4) {
        console.log("Invalid command syntax.");
        return;
    }

    if (argv[2] == "confirm") {
        msg = argv[3];
    }
    else if (argv[2] == "refuse") {
        status = -1; // TODO: fix to correct status code.
        reason = argv[3];
    } else {
        console.log("Unknown sub command: " + argv[2]);
        return;
    }

    if (carrier.replyFriendInvite(argv[1], status, reason, msg))
        console.log("Send invite reply to inviter success.");
    else
        console.log("Send invite reply to inviter failed(0x" +  carrier.getError().toString(16) + ").");
}





function friends_list_callback(carrier, friend_info, context) {
    if (first_friends_item) {
        count = 0;
        console.log("");
        console.log("Friends list from carrier network:");
        console.log("  ID                                              Connection   Label");
        console.log("  ----------------                                ----------   -----");
    }

    if (friend_info) {
        console.log("  " + fmt_str_blank(friend_info.userInfo.userId, 48) + fmt_str_blank(connection_name[friend_info.status], 13) + friend_info.label);
        first_friends_item = 0;
        count++;
    }
    else {
        /* The list ended */
        console.log("  ----------------");
        console.log("Total " + count + " friends.");

        first_friends_item = 1;
        rl.prompt();
    }
    // return true;
}

function friend_connection_callback(carrier, friendid,  status, context) {
    console.log("");
    switch (status) {
        case carrierSdk.ConnectionStatus_Connected:
            console.log("Friend[" + friendid +"] connection changed to be online");
            break;

        case carrierSdk.ConnectionStatus_Disconnected:
            console.log("Friend[" + friendid +"] connection changed to be offline.");
            break;

        default:
            console.log("Error!!! Got unknown connection status:" + status);
    }
    rl.prompt();
}


function friend_presence_callback(carrier, friendid,  status, context) {
    console.log("");
    if (status >= carrierSdk.PresenceStatus_None &&
        status <= carrierSdk.PresenceStatus_Busy) {
        console.log("Friend[" + friendid + "] change presence to " + presence_name[status]);
    }
    else {
        console.log("Error!!! Got unknown presence status %d.", status);
    }
    rl.prompt();
}

function friend_request_callback(carrier, userid, info, hello, context) {
    console.log("");
    console.log("Friend request from user[" + info.name ? info.name : userid + "] with HELLO: " + hello + ".");
    console.log("Reply use following commands:");
    console.log("  faccept " + userid);
    rl.prompt();
}

function friend_added_callback(carrier, info, context) {
    console.log("");
    console.log("New friend added. The friend information:");
    display_friend_info(info);
    rl.prompt();
}

function friend_removed_callback(carrier, friend_id, context) {
    console.log("");
    console.log("Friend " + friend_id +  " removed!");;
    rl.prompt();
}

function message_callback(carrier, from, msg, context) {
    console.log("");
    console.log("Message from friend[" + from + "]: " + msg);
    rl.prompt();
}

function invite_request_callback(carrier, from, data, context) {
    console.log("");
    console.log("Invite request from[" + from + "] with data: " + data);
    console.log("Reply use following commands:");
    console.log("  ireply " + from + " confirm [message]");
    console.log("  ireply " + from + " refuse [reason]");
    rl.prompt();
}

function invite_response_callback(carrier, friendid, status, reason, data, context) {
    console.log("");
    console.log("Got invite response from " + friendid + ".");
    if (status == 0) {
        console.log("message within response: " + data);
    }
    else {
        console.log("refused: " + reason);
    }
    rl.prompt();
}


// -----------

let count = 0;
let first_friends_item = 1;
const F = {
    fmt_str_blank(string, length){
        for(var i = string.length; i < length; i++) {
            string += " "
        }
        return string;
    }
};


const callbacks = {
    idle: ()=>{

    },
    connectionStatus: (carrier, status, context)=>{
        console.log('[connectionStatus]', status, context);
        switch (status) {
            case carrierSdk.ConnectionStatus_Connected:
                console.log("Connected to carrier network.");
                break;

            case carrierSdk.ConnectionStatus_Disconnected:
                console.log("Disconnect from carrier network.");
                break;

            default:
                console.log("Error!!! Got unknown connection status :" + status);
        }
    },
    friendsList: (carrier, friend_info, context)=>{
        if (first_friends_item) {
            count = 0;
            console.log("");
            console.log("Friends list from carrier network:");
            console.log("  ID                                              Connection   Label");
            console.log("  ----------------                                ----------   -----");
        }

        if (friend_info) {
            console.log("  " + F.fmt_str_blank(friend_info.userInfo.userId, 48) + fmt_str_blank(connection_name[friend_info.status], 13) + friend_info.label);
            first_friends_item = 0;
            count++;
        }
        else {
            /* The list ended */
            console.log("  ----------------");
            console.log("Total " + count + " friends.");

            first_friends_item = 1;
        }
        return true;
    },
    friendConnection: friend_connection_callback,
    friendInfo: (carrier, friendId, info, context)=>{
        console.log("");
        console.log("Friend information changed:");
        display_friend_info(info);
    },
    friendPresence: friend_presence_callback,
    friendRequest: friend_request_callback,
    friendAdded: friend_added_callback,
    friendRemoved: friend_removed_callback,
    friendMessage: message_callback,
    friendInvite: invite_request_callback
}

// function printConstants() {
//     console.log(carrierSdk.PresenceStatus_None);
//     console.log(carrierSdk.PresenceStatus_Away);
//     console.log(carrierSdk.PresenceStatus_Busy);
//     console.log(carrierSdk.ConnectionStatus_Connected);
//     console.log(carrierSdk.ConnectionStatus_Disconnected);
// }





// carrier.destory();

let carrier = null;
const sdk = {
    init(){
        console.log(carrierSdk.getVersion());
        carrier = carrierSdk.createObject(opts, callbacks);
        // carrier.on("idle", idle_callback); //add idle event
        // carrier.on("idle", null); //delete idle event
        carrier.run();

    },
    self_info(){
        const me = new UserClass(carrier.getSelfInfo());

        return me.getData();
    },
    set_self_info(key, value=''){
        let info = sdk.self_info();
        info[key] = value;
        carrier.setSelfInfo(info);
        return info;
    },
    self_presence(status=''){
        let presence;
        if(!status) {
            presence = carrier.getSelfPresence();
            return presence_name[presence];
        }
        else if (status) {
            if (status === "none") {
                presence = carrierSdk.PresenceStatus_None;
            }
            else if (status === "away") {
                presence = carrierSdk.PresenceStatus_Away;
            }
            else if (status === "busy") {
                presence = carrierSdk.PresenceStatus_Busy;
            }
            else {
                throw ("Invalid command syntax.");
            }

            if (carrier.setSelfPresence(presence)) {
                return true;
            }
            else {
                throw ("Set user's presence failed (0x" + carrier.getError().toString(16) + ").");
            }
        }
        else {
            throw("Invalid command syntax.");
        }
    },
    self_address(){
        return carrier.getAddress();
    },

    self_nodeid(){
        return carrier.getNodeId();
    },

    self_userid(){
        return carrier.getUserId();
    },
    add_friend(address, msg){
        if(carrier.addFriend(address, msg)){
            return true;
        }
        else{
            throw ("Request to add a new friend failed(0x" + carrier.getError().toString(16) + ").");
        }
    }



};

module.exports = sdk;