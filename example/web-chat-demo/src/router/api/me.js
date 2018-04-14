import Base from '../Base';

export default class extends Base{
    async action(){
        const carrier = this.buildCarrierService();

        const param = this.getParam();
        if(this[param.action]){
            return await this[param.action](carrier, param);
        }
        else{
            throw new Error('wrong action');
        }

    }

    async get(carrier){
        return this.result(carrier.self_info());
    }

    async set(carrier, param){
        const rs = carrier.set_self_info(param.key, param.value);
        return this.result(rs);
    }

    async address(carrier){
        return this.result(carrier.self_address());
    }

    async nodeid(carrier){
        return this.result(carrier.self_nodeid());
    }

    async userid(c){
        return this.result(c.self_userid());
    }


}