import Vue from 'vue'
import App from './App'
import router from './router'
import config from './config';
import store from './store';
import utility from './utils/utility';
import io from 'socket.io-client';

import VueSocketio from 'vue-socket.io';


store.init();

Vue.config.productionTip = false;

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  store: store.getStore(),
  components: { App },
  template: '<App/>',

  created(){
    const ela = utility.getElaId(this.$route);

    if(ela){
      const socket = io(config.SOCKET_URL+'?ela='+ela);
      Vue.use(VueSocketio, socket);
    }
    else{
      throw new Error('invalid ela id, socket connection created failed.');
    }
  },
  sockets : {

  },
  methods : {
    log(...args){
      console.log(...args);
      this.$store.commit('add_log', args[0].toString());
    },
    error(...args){
      console.error(...args);
      this.$store.commit('add_error', args[0].toString());
    }
  }
});

