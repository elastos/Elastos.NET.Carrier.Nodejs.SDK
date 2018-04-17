<template>
  <div class="box c_log">

    <ul class="m_ul">
      <li class="m_ul_li" v-for="log in list">
        {{log}}
      </li>
    </ul>

  </div>
</template>

<script>
  import _ from 'lodash';

  export default {
    data(){
      return {
        list : []
      }
    },
    mounted(){
      const store = this.$store;
      store.commit('clear_log');
      store.commit('add_log', 'start log...');

      this.list = this.$store.state.server_log;
    },
    created(){

    },
    sockets: {
      connect(){
        const store = this.$store;
        store.commit('add_log', 'socket connected');
      },
      elastos_log(data){
        const store = this.$store;
        if(_.isString(data)){
          store.commit('add_log', data);
        }
        else{
          store.commit('add_log', JSON.stringify(data));
        }

      }
    }
  }
</script>

<style>
  .m_ul_li{
    word-break: break-all;
  }
  .m_ul{
    padding-bottom: 20px;
  }
  .c_log{
    background: #333;
    color: greenyellow;
    height: 100%;
    overflow: auto;
    box-sizing: border-box;
    margin: 0;
  }
</style>
