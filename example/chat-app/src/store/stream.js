import _ from 'lodash';
import {File} from '@/utility';


const default_state = ()=>({
  data_buffer : [],
  connect : false
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


    reset(state){
      _.assign(state, default_state());
    }
  },

  actions : {

  }
};
