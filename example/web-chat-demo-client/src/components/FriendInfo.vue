<template>
  <div class="box">
    <h2>Friend Info</h2>
  </div>
</template>

<script>
  import api from '../utils/request';
  import utility from '../utils/utility';
  export default {
    data(){
      return {};
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
    }
  }
</script>
