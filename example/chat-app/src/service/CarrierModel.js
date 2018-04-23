import _ from 'lodash';

const User = class {
  constructor(userInfo){
    this.data = {
      name : userInfo.name,
      userId : userInfo.userId,
      description : userInfo.description,
      gender : userInfo.gender,
      phone : userInfo.phone,
      email : userInfo.email,
      region : userInfo.region
    };
  }

  getData(){
    return this.data;
  }
};

const Friend = class {
  constructor(friendInfo){
    const u = new User(friendInfo.userInfo);

    this.data = _.extend({}, u.getData(), {
      presence : friendInfo.presence,
      status : friendInfo.status,
      online : !friendInfo.status
    });
  }

  getData(){
    return this.data;
  }
};

export default {
  create(type, ...args){
    switch(type){
      case 'User':
        return new User(...args);

      case 'Friend':
        return new Friend(...args);

      default:
        throw new Error('invalid type');
    }
  }
};
