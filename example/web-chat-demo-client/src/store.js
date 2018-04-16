import Vue from 'vue';
import Vuex from 'vuex';
import _ from 'lodash';

const storeOpts = {
  state: {
    server_log : []
  },
  mutations: {
    add_log(state, log){
      if(_.size(state.server_log) > 10){
        state.server_log = _.slice(state.server_log, 10);
      }

      state.server_log.unshift(log);
    },
    clear_log(state){
      state.server_log = [];
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
