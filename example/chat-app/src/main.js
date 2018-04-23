import Vue from 'vue';
import App from './App';
import router from './router';
import ElementUI from 'element-ui';
import store from './store';

Vue.config.productionTip = false;

Vue.use(ElementUI);

let carrier;
try{
  carrier = CarrierService;
}catch(e){
  carrier = {};
}
new Vue({
  el: '#app',
  router,
  store : store.getStore(),
  components: { App },
  template: '<App/>',
  carrier,

  created(){
    window.store = this.$store;
  },
  mounted(){

  },
  methods : {
    getCarrier(){
      return carrier;
    }
  }
});


