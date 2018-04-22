
import _ from 'lodash';
export default {
  state : {
    list : []
  },
  mutations: {
    add_log(state, log){
      state.list.unshift({
        type : 'log',
        data : log
      });

    },
    clear_log(state){
      state.list = [];
    },
    add_error(state, error){
      state.list.unshift({
        type : 'error',
        data : error
      })
    }
  }
};

