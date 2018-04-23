<template>
  <div class="c_home">
    <div class="d_left_one">

    </div>
    <div class="d_left_two">
      <Left_Two />
    </div>

    <div class="d_main">

    </div>
  </div>
</template>

<script>
  import Left_Two from '../components/Home/Left_Two';
  export default {
    components : {
      Left_Two
    }
  };
</script>

<style lang="scss">
  @import "../css/index.scss";
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
