import Vue from 'vue';
import App from './App';
import router from './router';
// import Carrier from './service/Carrier';

Vue.config.productionTip = false;



new Vue({
  el: '#app',
  router,
  components: { App },
  template: '<App/>',

  mounted(){
    // const Carrier = require('./service/Carrier')
    const default_id = 'elastos';
    // Carrier.init(default_id);
    CarrierService.init(default_id);
  }
});


