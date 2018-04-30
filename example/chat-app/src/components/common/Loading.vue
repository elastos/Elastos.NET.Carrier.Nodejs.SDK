<template>
  <div class="cc-box c_loading">

    <div>
      <i class="cc-icon el-icon-loading"></i>
      <span class="d_span">{{text}}</span>
    </div>

  </div>

</template>
<script>
  export default {
    data(){
      return {
        text : this.$attrs.text || 'Loading ...'
      };
    }
  }
</script>
<style lang="scss">
  .c_loading{
    background: rgba(0,0,0,0.7);
    position: absolute;
    top: 0;
    left: 0;
    z-index: 101;
    line-height: 100vh;

    .cc-icon{
      font-size: 3rem;
      vertical-align: middle;
    }

    .d_span{
      font-size: 1.5rem;
      color: #ffffff;
      position: relative;
      left: 10px;
      top: 5px;
    }
  }
</style>
