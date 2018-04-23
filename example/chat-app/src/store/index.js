import Vue from 'vue';
import Vuex from 'vuex';
import _ from 'lodash';

import me from './me';

const storeOpts = {
  modules: {
    me : me,
  },
  state: {
  },
  mutations: {

  },
  actions: {

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
