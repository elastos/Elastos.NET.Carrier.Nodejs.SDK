import express from 'express';
import Base from '../Base';

import me from './me';
import friend from './friend';
import message from './message';

const router = express.Router();

Base.setRouter(router, [
    {
        path : '/me/:action',
        method : 'get',
        action : me
    },
    {
        path : '/friend/:action',
        method : 'get',
        action : friend
    },
    {
        path : '/message/:action',
        method : 'get',
        action : message
    }
]);


export default router;