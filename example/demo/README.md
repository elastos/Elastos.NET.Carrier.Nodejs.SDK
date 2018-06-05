
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
        sinit
        snew userid
        speer
        srequest
        sreply ok/sreply refuse [reason]
        sclose
        scleanup
        sadd [plain] [reliable] [multiplexing] [portforwarding]
        sremove id
        swrite streamid string
        sinfo id
        stype id
        sstate id
        scopen stream [cookie]
        scclose stream channel
        scwrite stream channel string
        scpend stream channel
        scresume stream channel
        spfadd name tcp|udp host port
        spfremove name
        spfopen stream service tcp|udp host port
        spfclose stream pfid
        exit
```

```
Stream connect steps:
                A                                   B
1               sinit                               sinit
2               snew B_id
3               sadd stream_type_X
4               srequest
5                                                   Reply use following commands:
                                                    1). snew A
                                                    2). sreply refuse [reason]
                                                    OR:
                                                    1). snew A
                                                    2). sadd [plain] [reliable] [multiplexing] [portforwarding]
                                                    3). sreply ok


Note:
step 4 condition:   a. Stream [1] state changed to: initialized
                    b. B connection changed to be online
step 5 3) condition:    a. Stream [1] state changed to: initialized

Note:
spfopen conditon:  sadd reliable multiplexing portforwarding
```