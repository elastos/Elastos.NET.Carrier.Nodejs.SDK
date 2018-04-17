import express from 'express';
import router from './router';
import cors from 'cors';
import io from 'socket.io';
import http from 'http';
import mysocket from './socket';

const app = express();
const server = http.Server(app);
const socket = io(server);

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


server.listen(3000, ()=>{
    console.log('server start => http://localhost:3000')
});


/* --------------- socket ------------------ */
socket.use((st, next) => {
    let ela = st.handshake.query.ela;
    if(ela){
        return next();
    }

    // st.to(st.id).emit('global', new Error('authentication error'));
    return next(new Error('authentication error'));
});
socket.on('connection', function(st){
    // console.log('00000000000000000000');
    mysocket.init(st, socket);
});
