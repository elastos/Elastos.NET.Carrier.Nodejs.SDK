import UserClass from './User';
import _ from 'lodash';

export default class {
    constructor(friendInfo){
        const u = new UserClass(friendInfo.userInfo);

        this.data = _.extend({}, u.getData(), {
            presence : friendInfo.presence,
            status : friendInfo.status
        });
    }

    getData(){
        return this.data;
    }
}