const express = require('express');
const api = require('./api');

const app = express();

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

app.use(express.static('public', options))
app.use('/api', api);


app.listen(3000, ()=>{
    console.log('server start => http://localhost:3000')
});