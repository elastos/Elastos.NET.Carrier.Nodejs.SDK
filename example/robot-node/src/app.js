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

app.listen(3000, ()=>{
    console.log('server start => http://localhost:3000')
});

