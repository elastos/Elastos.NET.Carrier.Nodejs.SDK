import Base from './Base';
import Carrier from './Carrier';
import md5 from "md5";

class Pool extends Base{
    init() {
        this.pool = this.initPool();
    }

    initPool(){
        // TODO

        return {};
    }

    getInstance(id){
        if(!id){
            throw new Error('invalid carrier instance id');
        }

        if(!this.pool[id]){
            const s = new Carrier(id);

            this.pool[id] = s;
        }

        return this.pool[id];
    }

    destroyInstance(id){
        if(this.pool[id]){
            console.log('Carrier instance of '+(id)+' killed');
            this.pool[id].close();
            delete this.pool[id];
        }
    }
}

export default new Pool();