import _ from 'lodash';


const default_state = ()=>({
  data_buffer : []
});

export default {
  state : default_state(),

  mutations : {
    'stream/carrier_data'(state, {data, context}){
      console.log(data);

      context.commit('add_log', '[stream/data] : '+data.data);
    },


    reset(state){
      _.assign(state, default_state());
    }
  },

  actions : {

  }
};
