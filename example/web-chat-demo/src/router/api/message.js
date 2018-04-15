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

    async send(c, param){
        const userid = param.userid;
        const msg = param.msg;

        const rs = c.send_message(userid, msg);

        return this.result(rs);
    }
}