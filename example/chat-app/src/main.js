import Vue from 'vue';
import App from './App';
import router from './router';
import ElementUI from 'element-ui';
import store from './store';

Vue.config.productionTip = false;

Vue.use(ElementUI);

let _carrier;

new Vue({
  el: '#app',
  router,
  store : store.getStore(),
  components: { App },
  template: '<App/>',

  created(){
    window.store = this.$store;

    this.$store.commit('add_log', 'start application');
  },
  mounted(){

  },
  methods : {
    getCarrier(){
      if(_carrier){
        return _carrier;
      }

      const self = this;
      try{
        _carrier = CarrierService;
      }catch(e){
        _carrier = {
          init(){
            _carrier.ready = true;
          },
          ready : false,

          execute(){
            self.$message({
              showClose: true,
              message: 'Carrier is invalid in web environment',
              type: 'error',
              duration: 5000
            });

            throw new Error('Carrier is invalid in web environment');
          }
        };
      }
      return _carrier;
    },

    syncData(type, data){
      this.$store.dispatch('carrier_data', {
        data,
        type
      });
    },

    successMessage(str){
      this.$message({
        showClose: true,
        message: str,
        type: 'success',
        duration: 3000
      });
    },
    errorMessage(err){
      this.$message({
        showClose: true,
        message: err,
        type: 'error',
        duration: 5000
      });
    },
    logout(){
      this.getCarrier().close();
      this.$store.dispatch('reset');
      this.$router.replace('login');
    }
  }
});


