import _ from 'lodash';
export default {
  state : {
    info : {
      name : '',
      userId : '',
      email : '',
      region : '',
      phone : '',
      gender : '',
    },
    address : '',
    online : false
  },

  mutations : {
    // set(state, key, value){
    //   if(state[key]){
    //     state[key] = value;
    //   }
    // },

    'me/get'(state, {data}){
      state.info = _.extend({}, state.info, data);
    },
    'me/set'(state, {data}){
      state.info = _.extend({}, state.info, data);
    },
    'me/online'(state, {data}){
      state.online = data.online;
    },
    'me/address'(state, {data}){
      state.address = data;
    }
  },

  actions : {

  }
};
