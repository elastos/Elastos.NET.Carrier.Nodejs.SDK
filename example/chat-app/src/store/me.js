import _ from 'lodash';
export default {
  namespace : true,
  state : {
    info : {
      name : 'NA',
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
