import _ from 'lodash';

const default_state = ()=>({
  username : 'elastos'
});

export default {
  namespaced : true,
  state : default_state(),

  mutations : {
    username(state, name){
      state.username = name;
    },


    reset(state){
      _.assign(state, default_state());
    }
  }
}
