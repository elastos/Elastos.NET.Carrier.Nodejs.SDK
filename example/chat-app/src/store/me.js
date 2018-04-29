import _ from 'lodash';

const PRESENCE_NAME = [
  "none",    // None;
  "away",    // Away;
  "busy",    // Busy;
];
const default_state = ()=>({
  info : {
    name : 'NA',
    userId : '',
    email : '',
    region : '',
    phone : '',
    gender : '',
  },
  address : '',
  online : false,
  presence : -1,
  presence_string : ''
});

export default {
  state : default_state(),

  mutations : {
    'me/info'(state, {data}){
      state.info = _.extend({}, state.info, data);
    },
    'me/online'(state, {data, context}){
      state.online = data.online;
    },
    'me/address'(state, {data}){
      state.address = data;
    },

    'me/presence'(state, {data}){
      state.presence = data;
      state.presence_string = PRESENCE_NAME[data];
    },


    reset(state){
      _.assign(state, default_state());
    }
  },

  actions : {

  }
};
