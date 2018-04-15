<template>
  <section class="body">
    <div class="header">
      <Info />
    </div>
    <div class="main"></div>
    <div class="footer"></div>
  </section>
</template>

<script>
  import Info from './Info';
  export default {
    data (){
      return {};
    },
    components : {
      Info
    }
  }
</script>

<style>
  .body{
    background: #eee;
    width: 980px;
    margin: 0 auto;
  }
  .header{
    min-height: 80px;

  }
</style>
