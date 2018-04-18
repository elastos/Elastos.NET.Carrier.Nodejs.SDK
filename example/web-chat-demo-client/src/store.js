import Vue from 'vue';
import Vuex from 'vuex';
import _ from 'lodash';

const storeOpts = {
  state: {
    server_log : []
  },
  mutations: {
    add_log(state, log){
      state.server_log.unshift({
        type : 'log',
        data : log
      });

    },
    clear_log(state){
      state.server_log = [];
    },
    add_error(state, error){
      state.server_log.unshift({
        type : 'error',
        data : error
      })
    }
  }
};

let store = null;
export default {
  init(){
    Vue.use(Vuex);
    store = new Vuex.Store(storeOpts);

    return store;
  },
  getStore(){
    if(!store){
      this.init();
    }

    return store;
  }
}
