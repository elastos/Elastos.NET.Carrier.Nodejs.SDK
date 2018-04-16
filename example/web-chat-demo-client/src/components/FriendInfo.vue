<template>
  <div class="box">
    <h2>Friend Info</h2>
    <p v-for="item in list" @click="showInfo(item)">
      {{item.userId}}
    </p>
  </div>
</template>

<script>
  import api from '../utils/request';
  import utility from '../utils/utility';
  export default {
    data(){
      return {
        flag : true,
        list : []
      };
    },

    created(){

    },

    mounted(){
      const ela = utility.getElaId(this.$route);
      api.go({
        path: '/api/friend/list',
        ela,
        success: (rs)=>{
          console.log(rs)
        }
      });
    },
    sockets: {
      elastos_data(data){
        if(data.elastos_type === 'friend_list'){
          if(this.flag){
            this.list = [data];
            this.flag = false;
          }
          else{
            this.list.push(data);
          }
        }
        else if(data.elastos_type === 'friend_list_end'){
          this.flag = true;
        }
      }
    },
    methods: {
      showInfo(item){
        this.$store.commit('add_log', JSON.stringify(item));
      }
    }
  }
</script>
