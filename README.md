# TL;DR
Elastos.NET.Carrier.Nodejs.SDK is the [Node.js Add-on](https://nodeaddons.com/) for Elastos Carrier. It allows Javascript code to call [Carrier C++](https://github.com/elastos/Elastos.NET.Carrier.Native.SDK) functions.

# Design concerns
## Thread safe
Carrier is multithreaded C++ module. JS is single thread. We need to make sure threads are well managed in NodeAddOn level. JS code call the addon to create new Carrier object. the addon will create new thread to handle the long running working thread(s) until it quits. JS caller can get a session ID to identify this particular carrier object. All communication between add-on and js code will have this session ID as the first arg.

## Session management
When Carrier C++ code create a new Carrier object, it returns a memory pointer to the new gneerated carrier object. But the memory pointer cannot be sent to the js code. Instead, we will hold the object in add-on memory into a table. A unique session ID can find the pointer to this Carrier object. The session ID can be sent to JS. Everytime, when JS call any function, or any callback function will need this session ID to identify which Carrier object it is talking to.

## Streaming
<!--- To Do -->

# FAQ
## Carrier.Nodejs.SDK vs Carrier.Trinity.SDK
### What is Carrier.Trinity project?
Trinity project is a modified version of Chromium browser. It embedded the Elastos.Carrier into the browser, so that there is no need for a server. The decentralized webapp can be safely running inside the Trinity browser while connect to the P2P network using Carrier. Of course, other Elastos.RT services will be provided by the trinity as well.

### Why do we need Carrier.Nodejs.SDK besides Carrier.Trinity?
Most of standard web/server application has a backend, very likely running on Node.js. Sometime the webapp need other services such as database which cannot be provided by Trinity at this moment. That's to say the backend is required. For these kind of webapp, we provide the Node.js addon which is called Carrier.Nodejs.SDK.

The addon can be used as a typical node.js modules using "import" or "require". It provide all the Carrier's functionalities to the js developers.

You can consider Carrier.Nodejs.SDK or Carrier.Trinity.SDK are just wrappers of Carrier C++ module. We try to keep both SDK using the same API so that you do not need to learn yet another sets of APIs.

## Where to learn more about Carrier module?
This project is just a wrapper. For more detail of Carrier, please go to Elastos.NET.Carrier.Native.SDK

## Are there any other SDK for other OS?
Yes, we are working on the SDK for Android, iOS, Mac, Windows, Linux, Nodejs and client-side-js. Please visit our code repo frequently to check latest updates.

# Build from source

## Ubuntu

### Install Pre-Requirements

Execute following command to install all pre-requirements.

Build carrier native sdk
Please goto https://github.com/elastos/Elastos.NET.Carrier.Native.SDK.
Clone the code and build.


Install node
```shell
$ cd $(SRC_ROOT)
$ git clone --branch v9.10.0 https://github.com/nodejs/node.git
$ cd node
$ ./configure
$ make
$ make install
```

Check node version.
```shell
$ node --version
v9.10.0
```

Install node-gype
```shell
sudo npm install -g node-gyp
```

### Build

Set the environment value about carrier native sdk header files and libraries path.
```shell
export CARRIER_NATIVE_SDK=$(SRC_CARRIER_NATIVE_SDK_ROOT)/build/_dist/Linux-x86_64/debug
```

From root directory run:

```shell
$ node-gyp configure
$ node-gyp build
```

### Demo
```shell
$ node ./example/demo/demo.js
```

# Demo App
We are going to make a demo web app to clone the Carrier Native SDK shell demo app. The shell demo app is a command line app. You can run two shell app as two different users. They can talk to each other, adding/removing friends, steaming etc.

We are going to make the same app but in browser using this NodeJs SDK.

