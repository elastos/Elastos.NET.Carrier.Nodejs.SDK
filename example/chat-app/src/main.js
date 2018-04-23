import Vue from 'vue';
import App from './App';
import router from './router';
import ElementUI from 'element-ui';
import store from './store';

Vue.config.productionTip = false;

Vue.use(ElementUI);

new Vue({
  el: '#app',
  router,
  store : store.getStore(),
  components: { App },
  template: '<App/>',

  created(){
    window.store = this.$store;
  },
  mounted(){
    // const Carrier = require('./service/Carrier')
    // const default_id = 'elastos';
    // // Carrier.init(default_id);
    // CarrierService.init(default_id);
  }
});


