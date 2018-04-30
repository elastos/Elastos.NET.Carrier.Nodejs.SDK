<template>
  <div class="cc-box c_main_header">
    <i class="cc-icon el-icon-tickets" style="right: 30px;" @click="showDialog=true"></i>



    <Log :show="showDialog" @change="change" />
  </div>
</template>
<script>
  import Log from '../common/Log';

  export default {
    data(){
      return {
        showDialog : false
      };
    },
    components : {
      Log
    },
    methods : {
      change(flag){
        this.showDialog = flag;
      }
    }
  };
</script>
<style lang="scss">
  .c_main_header{
    background: rgb(53, 57, 64);
    border-bottom: 1px solid rgb(41, 43, 57);
    box-shadow: 0 1px 0 #222;
    z-index: 1;

    .cc-icon{
      position: absolute;
      font-size: 2.2rem;
      color: #c9c9c9;
      top: 10px;
      cursor: pointer;

      &:hover{
        color: #ffffff;
      }
    }
  }
</style>
