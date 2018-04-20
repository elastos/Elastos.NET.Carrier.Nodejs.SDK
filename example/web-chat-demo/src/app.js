import express from 'express';
import router from './router';
import cors from 'cors';
// import io from 'socket.io';
import http from 'http';
import MySocket from './socket/WebSocket';
import uuid from 'node-uuid';

const app = express();
// const server = http.Server(app);
// const socket = io(server);

var expressWs = require('express-ws')(app);

const options = {
    dotfiles: 'ignore',
    etag: false,
    extensions: ['htm', 'html'],
    index: false,
    maxAge: '1d',
    redirect: false,
    setHeaders: function (res, path, stat) {
        res.set('x-timestamp', Date.now())
    }
}

app.use(cors());

// app.use(express.static('public', options));
router(app);


app.listen(3000, ()=>{
    console.log('server start => http://localhost:3000')
});


/* --------------- socket ------------------ */
// socket.listen(3100);
// socket.use((st, next) => {
//     let ela = st.handshake.query.ela;
//     if(ela){
//         return next();
//     }
//
//     // st.to(st.id).emit('global', new Error('authentication error'));
//     return next(new Error('authentication error'));
// });
//
// socket.origins((origin, callback) => {
//     console.log(111, origin)
//     callback(null, true);
// });
// // socket.set('origins', ['null']);
// // socket.set('transports', [            // all transports (optional if you want flashsocket)
// //     'websocket'
// //     , 'flashsocket'
// //     , 'htmlfile'
// //     , 'xhr-polling'
// //     , 'jsonp-polling'
// // ]);
//
//
// socket.on('connection', function(st){
//     // console.log('00000000000000000000');
//     mysocket.init(st, socket);
// });

app.ws('/', function(ws, req){

    const ela = req.query.ela;
    const rid = uuid.v4();
    if(ela){
        MySocket.create(rid, ela, expressWs, ws);
    }
    else{
       //TODO
    }


});