<template>
  <section class="body">
    <div class="header">
      <Header />
    </div>

    <div class="main">
      <Info />
      <!--<FriendInfo />-->
      <AddFriend />
    </div>
    <div class="footer"></div>
  </section>
</template>

<script>
  import Info from './Info';
  import Header from './Header';
  import FriendInfo from './FriendInfo';
  import AddFriend from './AddFriend';

  export default {
    data (){
      return {};
    },
    components : {
      Info,
      Header,
      FriendInfo,
      AddFriend
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
