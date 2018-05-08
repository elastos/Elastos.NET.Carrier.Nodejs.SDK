import express from 'express';
import config from './config';
import boot from './boot';
import router from './router';
import cors from 'cors';
import Carrier from "./service/Carrier";

const app = express();


app.use(cors());

// app.use(express.static('public', options));

// start carrier node
const carrier = Carrier.create('robot-carrier');

router(app, carrier);
boot(app, config, carrier);

const port = process.env.PORT;
app.listen(port, ()=>{
    console.log('server start at port : '+port);
});

