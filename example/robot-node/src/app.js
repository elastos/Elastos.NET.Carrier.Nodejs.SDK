import express from 'express';
import config from './config';
import boot from './boot';
// import router from './router';
import cors from 'cors';

const app = express();


app.use(cors());

// app.use(express.static('public', options));
// router(app);
boot(app, config);

const port = process.env.PORT;
app.listen(port, ()=>{
    console.log('server start at port : '+port);
});

