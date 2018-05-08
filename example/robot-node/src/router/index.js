
export default (app, carrier)=>{
    app.use('/address', (req, res)=>{
        res.send(carrier._data.address);
    });

    app.use('/friends', (req, res)=>{
       res.json(carrier._data.list);
    });
};