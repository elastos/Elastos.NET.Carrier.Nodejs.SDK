import Vue from 'vue';
import Vuex from 'vuex';
import _ from 'lodash';

import me from './me';
import friend from './friend';
import log from './log';

const storeOpts = {
  modules: {
    me : me,
    log,
    friend
  },
  state: {
  },
  mutations: {

  },
  actions: {
    carrier_data(context, param){
      try{
        context.commit(param.type, {
          data: param.data,
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
