
## Introduce
elastos_carrier addon is the [Node.js Add-on](https://nodeaddons.com/) for Elastos Carrier. It allows Javascript code to call [Carrier C++](https://github.com/elastos/Elastos.NET.Carrier.Native.SDK) functions.

## Installation
nstall with npm:
```shell
    npm install elastos_carrier_addon
```
## Example
```javascript
    const carrierSdk = require('elastos_carrier_addon');
    console.log(carrierSdk.getVersion());

    var carrier = carrierSdk.createObject(opts, callbacks);
    carrier.on("idle", idle_callback); //add idle event
    carrier.on("idle", null); //delete idle event

    carrier.run();
```

See [demo](https://github.com/elastos/Elastos.NET.Carrier.Nodejs.SDK/tree/master/example/demo) for more detail.
