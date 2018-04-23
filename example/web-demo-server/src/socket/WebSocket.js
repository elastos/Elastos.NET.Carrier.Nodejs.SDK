import _ from 'lodash';
import me from '../controller/me';
import friend from '../controller/friend';
import CarrierPool from '../service/CarrierPool';

const map = {};
class Socket{
    constructor(rid, expressWs, ws, id){
        this.rid = rid;
        this.id = id;
        this.app = expressWs.app;
        this.ws = ws;
        this.obj = expressWs;

        if(!map[id]){
            map[id] = [];
        }
        map[id].push({
            id : rid,
            ws : this.ws
        });

        this.init();
    }

    init(){
        this.ws.on('message', (msg)=>{
            // console.log(_.size(this.obj.getWss().clients));
            // console.log(this.ws.readyState);
            try{
                msg = JSON.parse(msg);
                this.handler(msg.type, msg.data);

            }catch(e){
                console.error('invalid message', msg);
            }

        });

        this.ws.on('close', ()=>{
            this.close();
        });

    }
    close(){
        _.remove(map[this.id], (item)=>{
            return item.id === this.rid;
        });

        _.delay(()=>{
            if(_.size(map[this.id]) < 1){
                // kill carrier node
                CarrierPool.destroyInstance(this.id);
            }
        }, 2000);


        console.log('[current client number] : '+this.getClientNumber());
    }
    logout(){
        // this.send(this.error('another client login.', -100));
        // this.ws.close();
    }

    error(error='', code=-1){
        return ({
            code,
            error
        })
    }

    result(data, code=1, msg='ok'){
        return ({
            data,
            code,
            msg
        });
    }

    send(data){

        const list = map[this.id];
        _.each(list, (item)=>{
            item.ws.send(JSON.stringify(data))
        });
    }

    getClientNumber(){
        return _.size(this.obj.getWss().clients)
    }

    handler(type, data){
        console.log('[receive] : ', type, data);
        let c = null;
        if(type === 'me'){
            c = this.buildController(me, data.method, data.param);
        }
        else if(type === 'friend'){
            c = this.buildController(friend, data.method, data.param);
        }

        if(c){
            c.execute(type);
        }
    }

    buildController(Controller, method, param={}){
        return new Controller(this, {
            ela : this.id,
            method,
            param
        });
    }

}

const pool = {};
export default {
    create(rid, ela, expressWs, ws){
        pool[ela] = new Socket(rid, expressWs, ws, ela);
        console.log('[current client number] : '+pool[ela].getClientNumber());
        return pool[ela];
    },

    get(ela){
        return pool[ela];
    }
}

