import express from 'express';
import router from './router';

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

app.use(express.static('public', options));
router(app);


app.listen(3000, ()=>{
    console.log('server start => http://localhost:3000')
});