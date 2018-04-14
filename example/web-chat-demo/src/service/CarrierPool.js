import Base from './Base';
import Carrier from './Carrier';

export default class extends Base{
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
}