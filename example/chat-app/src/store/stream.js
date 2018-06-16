import _ from 'lodash';
import vue from 'vue';
import {File} from '@/utility';


const default_state = ()=>({
  data_buffer : [],
  connect : false,

  /*
  *  state =>
        "raw",
        "initialized",
        "transport_ready",
        "connecting",
        "connected",
        "deactivated",
        "closed",
        "failed"
  *
  * */
  session : {
    // userId : {state : $state, id: $id}
  }
});

export default {
  state : default_state(),

  mutations : {
    'stream/carrier_data'(state, {data, context}){
      const {stream, buffer} = data;
      File.onStreamData(buffer, (type, d)=>{
        context.commit('add_log', '[stream/on_data] : '+type+' - '+d);
      });

    },
    'stream/connect_state'(state, {data}){
      // TODO
      console.log(111, data);
      if(data === 'connected'){
        state.connect = true;
      }
      else{
        state.connect = false;
      }
    },

    'stream/session_state'(state, {data}){
      const {sessionState, streamId, userId} = data;
      let tmp = {
        state : 0
      };
      if(state.session[userId]){
        tmp = state.session[userId];
      }

      if(!_.isUndefined(sessionState)){
        tmp.state = sessionState;
      }
      if(!_.isUndefined(streamId)){
        tmp.id = streamId;
      }
      vue.set(state.session, userId, tmp);
    },


    reset(state){
      _.assign(state, default_state());
    }
  },

  actions : {

  }
};
