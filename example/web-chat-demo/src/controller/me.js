import Base from './Base';

export default class extends Base{


    get(carrier){
        const info = carrier.self_info();

        return info;
    }

    set(carrier, param){
        const rs = carrier.set_self_info(param.key, param.value);
        return rs;
    }

    address(carrier){
        return carrier.self_address();
    }

    nodeid(carrier){
        return carrier.self_nodeid();
    }

    userid(carrier){
        return carrier.self_userid();
    }


}