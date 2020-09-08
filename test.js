var addon = require('bindings')('carrier_addon');

var options = {
    udpEnabled: true,
    /*persistentLocation: "D:\\elastOS\\n-api_test\\carrier_node_sdk",*/
    persistentLocation: "D:\\elastOS\\electron_build\\dev_trinity\\Runtime",
    bootstraps: [
        {
            ipv4: "13.58.208.50",
            port: "33445",
            publicKey: "89vny8MrKdDKs7Uta9RdVmspPjnRMdwMmaiEW27pZ7gh"
        },
        {
            ipv4: "18.216.102.47",
            port: "33445",
            publicKey: "G5z8MqiNDFTadFUPfMdYsYtkUDbX5mNCMVHMZtsCnFeb"
        },
        {
            ipv4: "18.216.6.197",
            port: "33445",
            publicKey: "H8sqhRrQuJZ6iLtP2wanxt4LzdNrN2NNFnpPdq1uJ9n2"
        },
        {
            ipv4: "52.83.171.135",
            port: "33445",
            publicKey: "5tuHgK1Q4CYf4K5PutsEPK5E3Z7cbtEBdx7LwmdzqXHL"
        },
        {
            ipv4: "52.83.191.228",
            port: "33445",
            publicKey: "3khtxZo89SBScAMaHhTvD68pPHiKxgZT6hTCSZZVgNEm"
        }
    ],
    expressNodes: [
        {
            ipv4: "ece00.trinity-tech.io",
            port: "443",
            publicKey: "FyTt6cgnoN1eAMfmTRJCaX2UoN6ojAgCimQEbv1bruy9"
        },
        {
            ipv4: "ece01.trinity-tech.io",
            port: "443",
            publicKey: "FyTt6cgnoN1eAMfmTRJCaX2UoN6ojAgCimQEbv1bruy9"
        },
        {
            ipv4: "ece01.trinity-tech.cn",
            port: "443",
            publicKey: "FyTt6cgnoN1eAMfmTRJCaX2UoN6ojAgCimQEbv1bruy9"
        }
    ]
};


function onConnection(carrier, status) {
    console.log("javascript - onConnection");
    console.log("status: "+status);
    console.log("address: "+carrier.getAddress());
    console.log("userId: "+carrier.getUserId());
    //carrier.acceptFriend("4DcFsUnZRK2vqnFZgjQnMrTnHUdijaPtSGoRLkSzvaq3");
    //carrier.removeFriend("4DcFsUnZRK2vqnFZgjQnMrTnHUdijaPtSGoRLkSzvaq3");

    /*let isValid = carrier.isValidAddress("sdfsdfsdfsf");
    if (isValid) {
        console.log("address valid");
    }
    else {
        console.log("address not valid");
    }

    isValid = carrier.isValidAddress("859sPNjiYCGGeTFXjziSDfRZvvPzewifr315fXSAZkHR1U9i4mKU");
    if (isValid) {
        console.log("address valid");
    }
    else {
        console.log("address not valid");
    }*/

    /*let presence = carrier.getPresence();
    console.log("presence: "+presence);*/

    /*let isFriend = carrier.isFriend("4DcFsUnZRK2vqnFZgjQnMrTnHUdijaPtSGoRLkSzvaq2");
    console.log("isFriend: "+isFriend);*/

    /*let friend = carrier.getFriend("4DcFsUnZRK2vqnFZgjQnMrTnHUdijaPtSGoRLkSzvaq3");
    console.log("friendId: "+friend.userId);
    console.log("friendName: "+friend.name);*/
}

function onReady(carrier) {
    console.log("javascript - onReady");
}

function onSelfInfoChanged(carrier, userInfo) {
    console.log("javascript - onSelfInfoChanged");
    console.log("name: "+userInfo.name);
}

function onFriendConnection(carrier, friendId, status) {
    console.log("javascript - onFriendConnection");
    console.log("friendId: "+friendId);
    console.log("status: "+status);

    //carrier.kill();
}

function onFriendInfoChanged(carrier, friendId, friendInfo) {
    console.log("javascript - onFriendInfoChanged");
    console.log("friendId: "+friendId);
    console.log("name: "+friendInfo.name);
    console.log("presence: "+friendInfo.presence);
    console.log("connectionStatus: "+friendInfo.connectionStatus);
}

function onFriendPresence(carrier, friendId, presence) {
    console.log("javascript - onFriendPresence");
    console.log("friendId: "+friendId);
    console.log("presence: "+presence);
}

function onFriendRequest(carrier, userId, userInfo, hello) {
    console.log("javascript - onFriendRequest");
    console.log("userId: "+userId);
    console.log("name: "+userInfo.name);
    console.log("hello: "+hello);
    carrier.acceptFriend(userId);
}

function onFriendAdded(carrier, friendInfo) {
    console.log("javascript - onFriendAdded");
    console.log("name: "+friendInfo.name);
}

function onFriendRemoved(carrier, friendId) {
    console.log("javascript - onFriendRemoved");
    console.log("friendId: "+friendId);
}

function onFriendMessage(carrier, from, msg, timestamp, isOffline) {
    console.log("javascript - onFriendMessage");
    console.log("from: "+from);
    console.log("msg: "+msg);
    console.log("timestamp: "+timestamp);
    console.log("isOffline: "+isOffline);
    carrier.kill();
}

var callbacks = {
    onConnection: onConnection,
    onReady: onReady,
    onSelfInfoChanged: onSelfInfoChanged,
    onFriendConnection: onFriendConnection,
    onFriendInfoChanged: onFriendInfoChanged,
    onFriendPresence: onFriendPresence,
    onFriendRequest: onFriendRequest,
    onFriendAdded: onFriendAdded,
    onFriendRemoved: onFriendRemoved,
    onFriendMessage: onFriendMessage
};

var carrier = addon.createCarrierInstance(options, callbacks);
//carrier.start(1000);

console.log("version: "+carrier.getVersion());