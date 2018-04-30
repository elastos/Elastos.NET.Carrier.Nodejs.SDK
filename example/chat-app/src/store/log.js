import _ from 'lodash';

const default_state = ()=>({
  list : []
});

export default {
  state : default_state(),
  mutations: {
    add_log(state, log){
      state.list.unshift({
        type : 'log',
        data : log,
        date : Date.now()
      });

    },
    clear_log(state){
      state.list = [];
    },
    add_error(state, error){
      state.list.unshift({
        type : 'error',
        data : error,
        date : Date.now()
      })
    },

    reset(state){
      _.assign(state, default_state());
    }
  }
};

