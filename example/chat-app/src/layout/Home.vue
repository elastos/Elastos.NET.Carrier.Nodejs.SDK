<template>
  <div class="cc-box c_home">
    <div class="d_left_one">
      <Left_One />
    </div>
    <div class="d_left_two">
      <Left_Two />
    </div>

    <div class="d_main">

    </div>

    <Loading text="Init Carrier ..." v-if="isLoading" />
  </div>
</template>

<script>
  import Left_Two from '../components/Home/Left_Two';
  import Left_One from '../components/Home/Left_One';

  import Loading from '../components/common/Loading';
  import CarrierModel from '../service/CarrierModel';

  export default {
    data(){
      return {
        isLoading : false
      }
    },
    components : {
      Left_Two,
      Left_One,

      Loading
    },

    created(){

    },

    mounted(){
      const default_id = 'elastos';
      const Carrier = this.$root.getCarrier();
      if(!Carrier.ready){
        this.isLoading = true;
        Carrier.init(default_id, this.$store, CarrierModel, {});
        const tm = setInterval(()=>{
          if(Carrier.ready){
            clearInterval(tm);
            this.isLoading = false;
          }
        }, 1000);
      }


    }
  };
</script>

<style lang="scss">
  .c_home{
    width: 100%;
    position: relative;

    .d_left_one{
      background: #42b983;
      position: fixed;
      left: 0;
      top: 0;
      width: 80px;
      bottom: 0;
    }
    .d_left_two{
      background: #0074D9;
      position: fixed;
      top: 0;
      left: 80px;
      bottom: 0;
      width: 300px;
    }
  }
</style>
