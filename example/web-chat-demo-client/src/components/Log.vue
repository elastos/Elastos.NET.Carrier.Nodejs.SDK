<template>
  <div class="box">
    <h2>LOG</h2>
    <hr />

    <ul v-for="log in list">
      <li>{{log}}</li>
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
        console.log(data);
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
