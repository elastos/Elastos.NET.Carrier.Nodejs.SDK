<template>
  <div class="box c_log">

    <ul class="m_ul">
      <li class="m_ul_li" v-for="log in list">
        <span v-if="log.type==='log'">{{log.data}}</span>
        <span v-else style="color:#f00;">{{log.data}}</span>
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
      this.$root.log('start log...');

      this.list = this.$store.state.server_log;
    },
    created(){

    },
    sockets: {
      connect(){
        this.$root.log('socket connected')
      },
      elastos_log(data){
        if(_.isString(data)){
          this.$root.log(data);
        }
        else{
          this.$root.log(JSON.stringify(data));
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
