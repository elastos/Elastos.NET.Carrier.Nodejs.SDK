
# Before run
Make sure your node version is 9.10 or above.
Install npm package
```shell
    npm install elastos_carrier_addon
```
# Run demo
```shell
    node demo.js
```
# Demo usage

    You can input the 'help' command get the commands usage.

```shell
    Command:$ help
    Available commands list:
        help [cmd]
        address
        nodeid
        userid
        me [set] [name | description | gender | phone | email | region] [value]
        nospam [ value ]
        presence [ none | away | busy ]
        fadd address hello
        faccept userid
        fremove userid
        friends
        friend userid
        label userid name
        msg userid message
        invite userid data
        ireply userid [confirm message | refuse reason]
        exit exit
```
