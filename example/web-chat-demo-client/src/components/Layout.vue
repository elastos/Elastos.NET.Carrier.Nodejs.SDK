<template>
  <section class="body">
    <div class="header">
      <Header />
    </div>

    <div class="main">
      <Info />
    </div>
    <div class="footer"></div>
  </section>
</template>

<script>
  import Info from './Info';
  import Header from './Header';
  export default {
    data (){
      return {};
    },
    components : {
      Info,
      Header
    }
  }
</script>

<style>
  @import "../css/index.scss";
  .body{
    width: 980px;
    margin: 0 auto;
  }
  .header{


  }
</style>
