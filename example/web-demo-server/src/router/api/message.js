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

    async send(param){
        const carrier = this.buildCarrierService();
        const userid = param.userid;
        const msg = param.msg;

        const rs = carrier.send_message(userid, msg);

        return this.result(rs);
    }
}