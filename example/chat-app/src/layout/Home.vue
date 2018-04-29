<template>
  <el-container class="cc-box c_home">
    <el-aside class="d_left_one" width="70px">
      <Left_One />
    </el-aside>
    <el-aside class="d_left_two" width="240px">
      <Left_Two />
    </el-aside>

    <el-container>
      <el-header style="padding:0;" height="50px">
        <Main_Header />
      </el-header>
      <el-main class="cc-box" style="padding: 0;">
        <div class="d_msg_area">
          <Message_List />
        </div>
        <!--<div class="d_friend_area">-->
          <!--<Main_Friend_List />-->
        <!--</div>-->
      </el-main>
    </el-container>

    <Loading text="Init Carrier ..." v-if="isLoading" />
  </el-container>

</template>

<script>
  import Left_Two from '../components/Home/Left_Two';
  import Left_One from '../components/Home/Left_One';
  import Main_Header from '../components/Home/Main_Header';
  import Main_Friend_List from '../components/Home/Main_Friend_List';
  import Message_List from '../components/Home/Message_List';

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
      Main_Header,
      Main_Friend_List,
      Message_List,

      Loading
    },

    created(){

    },

    mounted(){
      const username = this.$store.state.account.username;
      const Carrier = this.$root.getCarrier();
      if(!Carrier.ready){
        this.isLoading = true;
        Carrier.init(username, this.$store, CarrierModel, {});
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
    }
    .d_left_two{
      position: relative;
    }

    .d_msg_area{
      /*padding-right: 240px;*/
      background: #ccc;
      height: 100%;
    }
    .d_friend_area{
      width: 240px;
      position: absolute;
      right: 0;
      top: 0;
      height: 100%;
    }
  }
</style>
