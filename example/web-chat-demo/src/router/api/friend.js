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

    async add(c, param){
        const address = param.address;
        const msg = param.msg;

        const rs = c.add_friend(address, msg);

        return this.result(rs);
    }
}