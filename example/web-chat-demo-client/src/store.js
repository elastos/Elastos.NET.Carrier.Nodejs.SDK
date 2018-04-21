import Vue from 'vue';
import Vuex from 'vuex';
import _ from 'lodash';

import me from './store/me';
import log from './store/log';
import friend from './store/friend';

const storeOpts = {
  modules: {
    log : log,
    me : me,
    friend : friend
  },
  state: {
  },
  mutations: {

  },
  actions: {
    socket_data(context, param){
      try{
        context.commit(param.type, {
          ...param,
          context
        });

      }catch(e){
        console.error(e);
      }
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
