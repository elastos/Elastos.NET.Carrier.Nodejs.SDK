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

const carrierSdk = require('elastos_carrier_addon');
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
    { ipv4: "52.83.171.135",    port: "33445",  publicKey: "5tuHgK1Q4CYf4K5PutsEPK5E3Z7cbtEBdx7LwmdzqXHL"},
    { ipv4: "52.83.191.228",    port: "33445",  publicKey: "3khtxZo89SBScAMaHhTvD68pPHiKxgZT6hTCSZZVgNEm"}
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
});

var commands = [
    { cmd:"help",           fn:help,                   help:"help [cmd]" },

    { cmd:"address",        fn:get_address,            help:"address" },
    { cmd:"nodeid",         fn:get_nodeid,             help:"nodeid" },
    { cmd:"userid",         fn:get_userid,             help:"userid" },
    { cmd:"me",             fn:self_info,              help:"me [set] [name | description | gender | phone | email | region] [value]" },
    { cmd:"nospam",         fn:self_nospam,            help:"nospam [ value ]" },
    { cmd:"presence",       fn:self_presence,          help:"presence [ none | away | busy ]" },

    { cmd:"fadd",           fn:friend_add,             help:"fadd address hello" },
    { cmd:"faccept",        fn:friend_accept,          help:"faccept userid" },
    { cmd:"fremove",        fn:friend_remove,          help:"fremove userid" },
    { cmd:"friends",        fn:list_friends,           help:"friends" },
    { cmd:"friend",         fn:show_friend,            help:"friend userid" },
    { cmd:"label",          fn:label_friend,           help:"label userid name" },
    { cmd:"msg",            fn:send_message,           help:"msg userid message" },
    { cmd:"invite",         fn:invite,                 help:"invite userid data" },
    { cmd:"ireply",         fn:reply_invite,           help:"ireply userid [confirm message | refuse reason]" },

    { cmd:"sinit",          fn:session_init,           help:"sinit" },
    { cmd:"snew",           fn:session_new,            help:"snew userid" },
    { cmd:"speer",          fn:session_peer,           help:"speer" },
    { cmd:"srequest",       fn:session_request,        help:"srequest" },
    { cmd:"sreply",         fn:session_reply_request,  help:"sreply ok/sreply refuse [reason]"},
    { cmd:"sclose",         fn:session_close,          help:"sclose" },
    { cmd:"scleanup",       fn:session_cleanup,        help:"scleanup" },

    { cmd:"sadd",           fn:stream_add,             help:"sadd [plain] [reliable] [multiplexing] [portforwarding]"},
    { cmd:"sremove",        fn:stream_remove,          help:"sremove id" },
    { cmd:"swrite",         fn:stream_write,           help:"swrite streamid string" },
    { cmd:"sinfo",          fn:stream_get_info,        help:"sinfo id"},
    { cmd:"stype",          fn:stream_get_type,        help:"stype id"},
    { cmd:"sstate",         fn:stream_get_state,       help:"sstate id"},

    { cmd:"scopen",         fn:stream_open_channel,    help:"scopen stream [cookie]" },
    { cmd:"scclose",        fn:stream_close_channel,   help:"scclose stream channel" },
    { cmd:"scwrite",        fn:stream_write_channel,   help:"scwrite stream channel string" },
    { cmd:"scpend",         fn:stream_pend_channel,    help:"scpend stream channel" },
    { cmd:"scresume",       fn:stream_resume_channel,  help:"scresume stream channel" },

    { cmd:"spfadd",         fn:session_add_service,    help:"spfadd name tcp|udp host port" },
    { cmd:"spfremove",      fn:session_remove_service, help:"spfremove name" },
    { cmd:"spfopen",        fn:portforwarding_open,    help:"spfopen stream service tcp|udp host port" },
    { cmd:"spfclose",       fn:portforwarding_close,   help:"spfclose stream pfid" },

//    { cmd:"test",           fn:test,                   help:"test" },

    { cmd:"exit",           fn:exit,                   help:"exit" }
]

function do_command(input) {
    var args = input.trim().match(/[^\s"]+|"([^"]*)"/g);
    if (!args || args[0] == "") {
        rl.prompt();
        return;
    }

    for (var i = 0; i < commands.length; i++) {
        if (commands[i].cmd == args[0]) {
            commands[i].fn(args);
            rl.prompt();
            return;
        }
    }
    console.log("Unknown command:" + args[0]);
    rl.prompt();
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
    carrier.destroy();
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
            console.log(str +  " failed(0x" + carrierSdk.getError().toString(16) + ").");
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
            console.log("Set user's nospam failed (0x" + carrierSdk.getError().toString(16) + ").");
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
            presence = carrierSdk.PresenceStatus.NONE;
        }
        else if (argv[1] == "away") {
            presence = carrierSdk.PresenceStatus.AWAY;
        }
        else if (argv[1] == "busy") {
            presence = carrierSdk.PresenceStatus.BUSY;
        }
        else {
            console.log("Invalid command syntax.");
            return;
        }

        if (carrier.setSelfPresence(presence)) {
            console.log("User's presence changed to be " + argv[1] + ".");
        }
        else {
            console.log("Set user's presence failed (0x" + carrierSdk.getError().toString(16) + ").");
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
        console.log("Request to add a new friend failed(0x" + carrierSdk.getError().toString(16) + ").");
}

function friend_accept(argv) {
    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.acceptFriend(argv[1]))
        console.log("Accept friend request success.");
    else
        console.log("Accept friend request failed(0x" +  carrierSdk.getError().toString(16) + ").");
}

function friend_remove(argv) {
    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.removeFriend(argv[1]))
        console.log("Remove friend %s success.", argv[1]);
    else
        console.log("Remove friend %s failed (0x" +  carrierSdk.getError().toString(16) + ").");
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
        console.log("Get friend information failed(0x" +  carrierSdk.getError().toString(16) + ").");
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
        console.log("Update friend label failed(0x" +  carrierSdk.getError().toString(16) + ").");
}

function send_message(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.sendFriendMessage(argv[1], argv[2]))
        console.log("Send message success.");
    else
        console.log("Send message failed(0x" +  carrierSdk.getError().toString(16) + ").");
}

function invite(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (carrier.inviteFriend(argv[1], argv[2], invite_response_callback, null))
        console.log("Send invite request success.");
    else
        console.log("Send invite request failed(0x" +  carrierSdk.getError().toString(16) + ").");
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
        console.log("Send invite reply to inviter failed(0x" +  carrierSdk.getError().toString(16) + ").");
}

//-----------------------------------------------------------------------------
var session;
var session_ctx = {
    remote_sdp: "",
    unchanged_streams: 0,
    need_start: false,
}

function session_request_callback(carrier, from, sdp) {
    console.log("");
    session_ctx.remote_sdp = sdp;

    console.log("Session request from[" + from + "] with SDP:" + sdp);
    console.log("Reply use following commands:");
    console.log("  1. snew " + from);
    console.log("  2. sreply refuse [reason]");
    console.log("OR:");
    console.log("  1. snew " + from);
    console.log("  2. sadd [plain] [reliable] [multiplexing] [portforwarding]");
    console.log("  3. sreply ok");

    rl.prompt();
}

function session_request_complete_callback(session, status, reason, sdp) {
    console.log("");

    if (status != 0) {
        console.log("Session complete, status: " + status + ", reason:" + reason);
    }
    else {
        session_ctx.remote_sdp = sdp;
        var ret = session.start(session_ctx.remote_sdp);
        console.log("Session start " + (ret ? "success" : "failed"));
    }
    rl.prompt();
}

function stream_on_data(stream, data) {
    console.log("");
    console.log("Stream [" + stream.id + "] received data ["+ data.toString() + "]");
    rl.prompt();
}

function stream_on_state_changed(stream, state) {
    console.log("");
    var state_name = [
        "raw",
        "initialized",
        "transport_ready",
        "connecting",
        "connected",
        "deactivated",
        "closed",
        "failed"
    ];

    console.log("Stream [" + stream.id + "] state changed to: " + state_name[state]);

    if (state == carrierSdk.StreamState.TRANSPORT_READY) {
        --session_ctx.unchanged_streams;
        if ((session_ctx.unchanged_streams == 0) && (session_ctx.need_start)) {
            ret = session.start(session_ctx.remote_sdp);
            session_ctx.need_start = false;
            console.log("Session start " + (ret ? "success." : "failed."));
        }
    }


    rl.prompt();
}

function on_channel_open(stream, channel_id, cookie) {
    console.log("");
    console.log("Stream request open new channel:" + channel_id + " cookie:" + cookie);
    rl.prompt();
    return true;
}

function on_channel_opened(stream, channel_id) {
    console.log("");
    console.log("Channel " + stream.id + ":" + channel_id + " opened.");
    rl.prompt();
}

function on_channel_close(stream, channel_id, reason) {
    console.log("");
    console.log("Channel " + stream.id + ":" + channel_id + " closed.");
}

function on_channel_data(stream, channel_id, data) {
    console.log("");
    console.log("Channel " + stream.id + ":" + channel_id + " received data [" + data.toString() + "]");
    rl.prompt();
    return true;
}

function on_channel_pending(stream, channel_id) {
    console.log("");
    console.log("Channel " + stream.id + ":" + channel_id + " is pending.");
    rl.prompt();
}

function on_channel_resume(stream, channel_id) {
    console.log("");
    console.log("Channel " + stream.id + ":" + channel_id + " resumed.");
    rl.prompt();
}

function session_init(argv) {
    if (argv.length != 1) {
        console.log("Invalid command syntax.");
        return;
    }

    session_ctx.remote_sdp = "";
    var ret = carrier.initSession(session_request_callback);
    if (!ret) {
        console.log("Session initialized failed.");
    }
    else {
        console.log("Session initialized successfully.");
    }
}

function session_cleanup(argv) {
    if (argv.length != 1) {
        console.log("Invalid command syntax.");
        return;
    }

    carrier.cleanupSession();
    console.log("Session cleaned up.");
}

function session_new(argv) {
    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    session = carrier.newSession(argv[1]);
    if (!session) {
        console.log("Create session failed.");
    }
    else {
        console.log("Create session successfully.");
        session.stream = [];
        session_ctx.need_start = false;
        session_ctx.unchanged_streams = 0;
    }
}

function session_close(argv) {
    if (argv.length != 1) {
        console.log("Invalid command syntax.");
        return;
    }

    if (session) {
        session.close();
        session = null;
        console.log("Session closed.");
    } else {
        console.log("No session available.");
    }
}

function stream_add(argv) {
    var callbacks = new Object;
    var options= 0;

    callbacks.stateChanged = stream_on_state_changed;
    callbacks.streamData = stream_on_data;

    if (!session) {
        console.log("No session available.");
    }

    if (argv.length < 1) {
        console.log("Invalid command syntax.");
        return;
    }
    else if (argv.length > 1) {
        for (var i = 1; i < argv.length; i++) {
            if (argv[i] == "reliable") {
                options |= carrierSdk.StreamMode.RELIABLE;
            }
            else if (argv[i] == "plain") {
                options |= carrierSdk.StreamMode.PLAIN;
            }
            else if (argv[i] == "multiplexing") {
                options |= carrierSdk.StreamMode.MULTIPLEXING;
            }
            else if (argv[i] == "portforwarding") {
                options |= carrierSdk.StreamMode.PORT_FORWARDING;
            } else {
                console.log("Invalid command syntax.");
                return;
            }
        }
    }

    if ((options & carrierSdk.StreamMode.MULTIPLEXING) || (options & carrierSdk.StreamMode.PORT_FORWARDING)) {
        callbacks.channelOpen = on_channel_open;
        callbacks.channelOpened = on_channel_opened;
        callbacks.channelData = on_channel_data;
        callbacks.channelPending = on_channel_pending;
        callbacks.channelResume = on_channel_resume;
        callbacks.channelClose = on_channel_close;
    }

    var stream = session.addStream(carrierSdk.StreamType.TEXT, options, callbacks);
    if (!stream) {
        console.log("Add stream failed.");
    }
    else {
        session_ctx.unchanged_streams++;
        session.stream[stream.id] = stream;
        stream.channel = [];
        console.log("Add stream successfully and stream id " + stream.id);
    }
}

function stream_remove(argv) {
    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var ret = stream.remove();
    if (!ret) {
        console.log("Remove stream " + stream.id + " failed.");
    }
    else {
        console.log("Remove stream " + stream.id + " success.");
    }
}

function session_peer(argv) {
    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var peer = session.getPeer();
    if (!peer) {
        console.log("Get peer failed.");
    }
    else {
        console.log("Get peer: " + peer);
    }
}

function session_request(argv) {
    if (argv.length != 1) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var ret = session.request(session_request_complete_callback);
    if (!ret) {
        console.log("session request failed.");
    }
    else {
        console.log("session request successfully.");
    }
}

function session_reply_request(argv) {
    var ret;
    if ((argv.length != 2) && (argv.length != 3)) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }

    if ((argv[1] == "ok") && (argv.length == 2)) {
        ret = session.replyRequest(0, null);
        if (!ret) {
            console.log("response invite failed.");
        }
        else {
            console.log("response invite successuflly.");

            if (session_ctx.unchanged_streams > 0) {
                session_ctx.need_start = true;
            }
            else {
                ret = session.start(session_ctx.remote_sdp);
                console.log("Session start " + (ret ? "success." : "failed."));
            }
        }
    }
    else if ((argv[1] == "refuse") && (argv.length == 3)) {
        ret = session.replyRequest(1, argv[2]);
        if (!ret) {
            console.log("response invite failed.");
        }
        else {
            console.log("response invite successuflly.");
        }
    }
    else {
        console.log("Unknown sub command.");
        return;
    }
}

function stream_write(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var buf = new Buffer(argv[2]);
    var rc = stream.write(buf);
    if (rc < 0) {
        console.log("write data failed.");
    }
    else {
        console.log("write data successfully.");
    }
}

function stream_get_info(argv) {
    var info;

    var topology_name = [
        "LAN",
        "P2P",
        "RELAYED"
    ];

    var addr_type = [
        "HOST   ",
        "SREFLEX",
        "PREFLEX",
        "RELAY  "
    ];

    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    info = stream.getTransportInfo();
    if (!info) {
        console.log("get remote addr failed.");
        return;
    }

    console.log("Stream transport information:");
    console.log("    Network: " + topology_name[info.topology]);

    console.log("      Local: " + addr_type[info.local.type] + " " + info.local.address + ":" + info.local.port);
    if (info.local.relatedAddress)
        console.log("       related " + info.local.relatedAddress + ":" + info.local.relatedPort);
    else
        console.log("");

    console.log("     Remote: " + addr_type[info.remote.type] + " " + info.remote.address + ":" + info.remote.port);
    if (info.remote.relatedAddress)
        console.log("       related " + info.remote.relatedAddress + ":" + info.remote.relatedPort);
    else
        console.log("");
}

function stream_get_type(argv) {
    var info;

    var type_name = [
        "audio",
        "video",
        "text",
        "application",
        "message"
    ];

    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var type = stream.getType();
    if (!type) {
        console.log("get type failed.");
        return;
    }

    console.log("Stream type: " + type_name[type]);
}

function stream_get_state(argv) {
    var info;

    var state_name = [
        "raw",
        "initialized",
        "transport_ready",
        "connecting",
        "connected",
        "deactivated",
        "closed",
        "failed"
    ];

    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var state = stream.getState();
    if (!state) {
        console.log("get state failed.");
        return;
    }

    console.log("Stream state: " + state_name[state]);
}

function stream_open_channel(argv) {
    if (argv.length < 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var cookie = null;
    if (argv.length == 3) {
        cookie = argv[2];
    }
    var ch = stream.openChannel(cookie);
    if (!ch) {
        console.log("Create channel failed.");
    } else {
        console.log("Channel " + ch + " created.");
    }
}

function stream_close_channel(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var ret = stream.closeChannel(parseInt(argv[2]));
    if (!ret) {
        console.log("Close channel failed.");
    } else {
        console.log("Channel %s closed.", argv[2]);
    }
}

function stream_write_channel(argv) {
    if (argv.length != 4) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var buf = new Buffer(argv[3]);
    var rc = stream.writeChannel(parseInt(argv[2]), buf);
    if (rc < 0) {
        console.log("Write channel failed.");
    } else {
        console.log("Channel %s write successfully.", argv[2]);
    }
}

function stream_pend_channel(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var ret = stream.pendChannel(parseInt(argv[2]));
    if (!ret) {
        console.log("Pend channel failed.");
    } else {
        console.log("Channel " + argv[2] + " input is pending.", );
    }
}

function stream_resume_channel(argv) {
    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var ret = stream.resumeChannel(parseInt(argv[2]));
    if (!ret) {
        console.log("Resume channel(input) failed.");
    } else {
        console.log("Channel " + argv[2] + " input is resumed.");
    }
}

function session_add_service(argv) {
    var protocol;

    if (argv.length != 5) {
        console.log("Invalid command syntax.");
        return;
    }

    if (argv[2] == "tcp")
        protocol = carrierSdk.PortForwardingProtocol.TCP;
    else {
        console.log("Unknown protocol " + argv[2]);
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var ret = session.addService(argv[1], protocol, argv[3], argv[4]);
    console.log("Add service ", argv[1] + " " + (ret ? "success" : "failed"));
}

function session_remove_service(argv) {
    if (argv.length != 2) {
        console.log("Invalid command syntax.");
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    session.removeService(argv[1]);
    console.log("Service " + argv[1] + " removed.");
}

function portforwarding_open(argv) {
    var protocol;
    var pfid;

    if (argv.length != 6) {
        console.log("Invalid command syntax.");
        return;
    }

    if (argv[3] == "tcp")
        protocol = carrierSdk.PortForwardingProtocol.TCP;
    else {
        console.log("Unknown protocol %s.", argv[3]);
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    console.log(argv[2], protocol, argv[4], argv[5])
    var pfid = stream.openPortForwarding(argv[2], protocol, argv[4], argv[5]);

    if (pfid > 0) {
        console.log("Open portforwarding to service " + argv[2] + " <<== " + argv[4] + ":" + argv[5] + " success, id is " + pfid);
    } else {
        console.log("Open portforwarding to service " + argv[2] + " <<== " + argv[4] + ":" + argv[5] + " failed.");
    }
}

function portforwarding_close(argv)
{
    var pfid;

    if (argv.length != 3) {
        console.log("Invalid command syntax.");
        return;
    }

    var pfid = parseInt(argv[2]);
    if (pfid <= 0) {
        console.log("Invalid portforwarding id %s.", argv[2]);
        return;
    }

    if (!session) {
        console.log("session is invalid.");
        return;
    }
    var stream = session.stream[parseInt(argv[1])];
    if (!stream) {
        console.log("stream " + argv[1] + " is invalid.");
        return;
    }
    var ret = stream.closePortForwarding(pfid);
    console.log("Portforwarding " + pfid + " closed " + (ret ? "success." : "failed."));
}

//Callback Functions
function idle_callback(carrier, context) {
    pring(dgjalgj);
    // console.log("call idle_callback.");
}

function connection_callback(carrier, status, context) {
    console.log("");
    switch (status) {
        case carrierSdk.ConnectionStatus.CONNECTED:
            console.log("Connected to carrier network.");
            break;

        case carrierSdk.ConnectionStatus.DISCONNECTED:
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
        case carrierSdk.ConnectionStatus.CONNECTED:
            console.log("Friend[" + friendid +"] connection changed to be online");
            break;

        case carrierSdk.ConnectionStatus.DISCONNECTED:
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
    if (status >= carrierSdk.PresenceStatus.NONE &&
        status <= carrierSdk.PresenceStatus.BUSY) {
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

//--------------------------------------------------------
console.log(carrierSdk.getVersion());
// carrierSdk.logInit(4, null); //ElaLogLevel_Info

var session = null;
var carrier = carrierSdk.createObject(opts, callbacks);
carrier.on("idle", idle_callback); //add idle event
carrier.on("idle", null); //delete idle event

carrier.run();
// carrier.destroy();

