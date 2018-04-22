import CarrierPool from '../service/CarrierPool';

let POOL = null;
export default class{
    constructor(socket, opts={}){
        this.socket = socket;
        this.ela = opts.ela;
        this.method = opts.method;
        this.param = opts.param || {};

        if(!POOL){
            POOL = CarrierPool;
        }

    }

    buildCarrierService(){
        const id = this.ela;
        return POOL.getInstance(id);
    }

    execute(type){
        if(this[this.method]){
            try{
                const rs = this[this.method].call(this, this.buildCarrierService(), this.param);
                this.socket.send({
                    ...this.socket.result(rs),
                    type : `${type}/${this.method}`
                });
            }catch(e){
                this.socket.send({
                    ...this.socket.error(e.reason || e.message || e.toString()),
                    type : `${type}/${this.method}`
                });
            }
        }
    }
}