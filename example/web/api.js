const express = require('express');
const _ = require('lodash');
const sdk = require('./carrier');

const router = express.Router();

const F = {
    getParam(req, key){
        const param = _.extend({}, req.query, req.params);
        return key ? _.get(param, key, null) : param;
    }
}

router.get('/', (req, res)=>{
    res.send('hell elastos');
});

router.get('/me', (req, res)=>{
    const info = sdk.self_info();
    res.json(info);
})
router.get('/me/set', (req, res)=>{
    const key = F.getParam(req, 'key');
    const value = F.getParam(req, 'value');

    const rs = sdk.set_self_info(key, value);
    res.json(rs);
});
router.get('/me/address', (req, res)=>{
    res.send(sdk.self_address());
});
router.get('/me/nodeid', (req, res)=>{
    res.send(sdk.self_nodeid());
});
router.get('/me/userid', (req, res)=>{
    res.send(sdk.self_userid());
});
router.get('/friend/add', (req, res)=>{
    const param = F.getParam(req);
    const rs = sdk.add_friend(param.address, param.msg);
    res.send(rs);
});


module.exports = router;
sdk.init();