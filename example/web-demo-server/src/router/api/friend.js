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

    async add(param){
        const carrier = this.buildCarrierService();
        const address = param.address;
        const msg = param.msg;

        const rs = carrier.add_friend(address, msg);

        return this.result(rs);
    }

    async list(){
        const carrier = this.buildCarrierService();
        const rs = carrier.list_friend();

        return this.result(rs);
    }

    async remove(param){
        const carrier = this.buildCarrierService();
        const userId = param.userid;

        const rs = carrier.remove_friend(userId);
        return this.result(rs);
    }

    async accept(param){
        const carrier = this.buildCarrierService();
        const userId = param.userid;

        const rs = carrier.accept_friend(userId);
        return this.result(rs);
    }
}