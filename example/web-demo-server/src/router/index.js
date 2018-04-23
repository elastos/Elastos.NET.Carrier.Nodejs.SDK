import api from './api';


export default (app)=>{
    app.use('/hello', (req, res)=>{
        res.send('hello world');
    });
    app.use('/api', api);
};
