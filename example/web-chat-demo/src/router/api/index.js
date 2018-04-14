import express from 'express';
import Base from '../Base';

import me from './me';
import friend from './friend';

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
    }
]);


export default router;