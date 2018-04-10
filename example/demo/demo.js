//
// Copyright (c) 2018 Elastos Foundation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

// process.env.UV_THREADPOOL_SIZE = 8

const carrierSdk = require('elca');
const readline = require('readline');

//Data define
var connection_name = [
    "online",
    "offline"
];

var presence_name = [
    "none",    // None;
    "away",    // Away;
    "busy",    // Busy;
];

var bootstraps = [
    { ipv4: "13.58.208.50",     port: "33445",  publicKey: "89vny8MrKdDKs7Uta9RdVmspPjnRMdwMmaiEW27pZ7gh"},
    { ipv4: "18.216.102.47",    port: "33445",  publicKey: "G5z8MqiNDFTadFUPfMdYsYtkUDbX5mNCMVHMZtsCnFeb"},
    { ipv4: "18.216.6.197",     port: "33445",  publicKey: "H8sqhRrQuJZ6iLtP2wanxt4LzdNrN2NNFnpPdq1uJ9n2"},
    { ipv4: "54.223.36.193",    port: "33445",  publicKey: "5tuHgK1Q4CYf4K5PutsEPK5E3Z7cbtEBdx7LwmdzqXHL"},
    { ipv4: "52.80.187.125",    port: "33445",  publicKey: "3khtxZo89SBScAMaHhTvD68pPHiKxgZT6hTCSZZVgNEm"}
];

var opts = {
    udpEnabled: true,
    persistentLocation: ".data",
    bootstraps: bootstraps
};

//Command Functions
const rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});

rl.setPrompt('Command:$ ');

rl.on('line', (input) => {
    do_command(`${input}`);
    rl.prompt();
});

var commands = [
    { cmd:"help",       fn:help,                   help:"help [cmd]" },

    { cmd:"address",    fn:get_address,            help:"address" },
    { cmd:"nodeid",     fn:get_nodeid,             help:"nodeid" },
    { cmd:"userid",     fn:get_userid,             help:"userid" },
    { cmd:"me",         fn:self_info,              help:"me [set] [name | description | gender | phone | email | region] [value]" },
    { cmd:"nospam",     fn:self_nospam,            help:"nospam [ value ]" },
    { cmd:"presence",   fn:self_presence,          help:"presence [ none | away | busy ]" },

    { cmd:"fadd",       fn:friend_add,             help:"fadd address hello" },
    { cmd:"faccept",    fn:friend_accept,          help:"faccept userid" },
    { cmd:"fremove",    fn:friend_remove,          help:"fremove userid" },
    { cmd:"friends",    fn:list_friends,           help:"friends" },
    { cmd:"friend",     fn:show_friend,            help:"friend userid" },
    { cmd:"label",      fn:label_friend,           help:"label userid name" },
    { cmd:"msg",        fn:send_message,           help:"msg userid message" },
    { cmd:"invite",     fn:invite,                 help:"invite userid data" },
    { cmd:"ireply",     fn:reply_invite,           help:"ireply userid [confirm message | refuse reason]" },
    { cmd:"exit",       fn:exit,                   help:"exit" }
]

function do_command(input) {
    var args = input.trim().split(' ');
    if (args[0] == "") {
        rl.prompt();
        return;
    }

    for (var i = 0; i < commands.length; i++) {
        if (commands[i].cmd == args[0]) {
            commands[i].fn(args);
            return;
        }
    }
    console.log("Unknown command:" + args[0]);
}

function help(args) {
    if (args.length > 1) {
        for (var i = 0; i < commands.length; i++) {
            if (commands[i].cmd == args[1]) {
                console.log("Usage: :" + commands[i].help);
                return;
            }
        }
        console.log("Usage: " + commands[0].help);
    }
    else {
        console.log("Available commands list:");
        for (var i = 0; i < commands.length; i++) {
            console.log("    " + commands[i].help);
        }
    }
}

function exit(args) {
    carrier.destory();
    process.exit(0);
}

function display_user_info(info) {
    console.log("           ID: %s", info.userId);
    console.log("         Name: %s", info.name);
    console.log("  Description: %s", info.description);
    console.log("       Gender: %s", info.gender);
    console.log("        Phone: %s", info.phone);
    console.log("        Email: %s", info.email);
    console.log("       Region: %s", info.region);
}


function display_friend_info(info) {
    display_user_info(info.userInfo);
    console.log("     Presence: %s", presence_name[info.presence]);
    console.log("   Connection: %s", connection_name[info.status]);
}

function get_address(args) {
    console.log(carrier.getAddress());
}

function get_nodeid(args) {
    console.log(carrier.getNodeId());
}

function get_userid(args) {
    console.log(carrier.getUserId());
}

function self_info(argv) {
    var info = carrier.getSelfInfo();
    if (argv.length == 1) {
        display_user_info(info)
    }
    else if (argv.length == 3 || argv.length == 4) {
        if (argv[1] != "set") {
            console.log("Unknown sub command: " + argv[1]);
            return;
        }

        var value = "";
        if (argv.length == 4) value = argv[3];

        if (argv[2] == "name") {
            info.name = value;
        }
        else if (argv[2] == "description") {
            info.description = value;
        }
        else if (argv[2] == "gender") {
            info.gender = value;
        }
        else if (argv[2] == "phone") {
            info.phone = value;
        }
        else if (argv[2] == "email") {
            info.email = value;
        }
        else if (argv[2] == "region") {
            info.region = value;
        }
        else {
            console.log("Invalid attribute name: " +  argv[2]);
            return;
        }

        var str = "Set " + argv[2] + ":'" + argv[3] + "'";
        if (carrier.setSelfInfo(info))
            console.log(str +  " succeess.");
        else
            console.log(str +  " failed(0x" + carrier.getError().toString(16) + ").");
    }
    else {
        console.log("Invalid command syntax.");
    }
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
    var presence;
    if (argv.length == 1) {
        presence = carrier.getSelfPresence();
        console.log("Self presence: " + presence_name[presence]);
    }
    else if (argv.length == 2) {
        if (argv[1] == "none") {
            presence = carrierSdk.PresenceStatus_None;
        }
        else if (argv[1] == "away") {
            presence = carrierSdk.PresenceStatus_Away;
        }
        else if (argv[1] == "busy") {
            presence = carrierSdk.PresenceStatus_Busy;
        }
        else {
            console.log("Invalid command syntax.");
            return;
        }

        if (carrier.setSelfPresence(presence)) {
            console.log("User's presence changed to be " + argv[1] + ".");
        }
        else {
            console.log("Set user's presence failed (0x" + carrier.getError().toString(16) + ").");
        }
    }
    else {
        console.log("Invalid command syntax.");
    }
}

function friend_add(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.addFriend(argv[1], argv[2]))
        console.log("Request to add a new friend succeess.");
    else
        console.log("Request to add a new friend failed(0x" + carrier.getError().toString(16) + ").");
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



//Callback Functions
function idle_callback(carrier, context) {
    // console.log("call idle_callback.");
}

function connection_callback(carrier, status, context) {
    console.log("");
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
    rl.prompt();
}

var count = 0;
var first_friends_item = 1;

function fmt_str_blank(string, length) {
    for(var i = string.length; i < length; i++) {
        string += " "
    }
    return string;
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

function friend_info_callback(carrier, friendid, info, context) {
    console.log("");
    console.log("Friend information changed:");
    display_friend_info(info);
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

var callbacks = {
    idle: idle_callback,
    connectionStatus: connection_callback,
    friendsList: friends_list_callback,
    friendConnection: friend_connection_callback,
    friendInfo: friend_info_callback,
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

//--------------------------------------------------------
var context = "context";

console.log(carrierSdk.getVersion());
// carrierSdk.logInit(4, null); //ElaLogLevel_Info

var carrier = carrierSdk.createObject(opts, callbacks);
carrier.on("idle", idle_callback); //add idle event
carrier.on("idle", null); //delete idle event

carrier.run();
// carrier.destory();

