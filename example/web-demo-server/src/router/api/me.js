import Base from '../Base';

export default class extends Base{
    async action(){
        const param = this.getParam();
        if(this[param.action]){
            return await this[param.action](param);
        }
        else{
            throw new Error('wrong action');
        }

    }

    async get(){
        const carrier = this.buildCarrierService();
        return this.result(carrier.self_info());
    }

    async set(param){
        const carrier = this.buildCarrierService();
        const rs = carrier.set_self_info(param.key, param.value);
        return this.result(rs);
    }

    async address(){
        const carrier = this.buildCarrierService();
        return this.result(carrier.self_address());
    }

    async nodeid(){
        const carrier = this.buildCarrierService();
        return this.result(carrier.self_nodeid());
    }

    async userid(){
        const carrier = this.buildCarrierService();
        return this.result(carrier.self_userid());
    }


}