import Base from './Base';

export default class extends Base{

    add(carrier, param){
        const address = param.address;
        const msg = param.msg;

        const rs = carrier.add_friend(address, msg);

        return rs;
    }

    list(carrier){
        const rs = carrier.list_friend();

        return rs;
    }

    remove(carrier, param){
        const userId = param.userid;

        const rs = carrier.remove_friend(userId);
        return rs;
    }

    accept(carrier, param){
        const userId = param.userid;

        const rs = carrier.accept_friend(userId);
        return rs;
    }

    message(carrier, param){
        const userid = param.userid;
        const msg = param.msg;

        const rs = carrier.send_message(userid, msg);

        return rs;
    }
}