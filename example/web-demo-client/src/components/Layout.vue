<template>
  <section class="body">
    <div class="header">
      <Header />
    </div>

    <div class="main">
      <Info />
      <FriendInfo />
      <Message />
      <AddFriend />
    </div>
    <div class="footer">
      <Log />
    </div>
  </section>
</template>

<script>
  import Info from './Info';
  import Header from './Header';
  import FriendInfo from './FriendInfo';
  import AddFriend from './AddFriend';
  import Log from './Log';
  import Message from './Message';


  export default {
    data (){
      return {};
    },
    components : {
      Info,
      Header,
      FriendInfo,
      AddFriend,
      Log,
      Message
    },
    created(){

    }
  }
</script>

<style>
  @import "../css/index.scss";
  .body{
    width: 980px;
    margin: 0 auto;
    padding-bottom: 300px;
  }
  .header{

  }
  .footer{
    width: 100%;
    height: 280px;
    position: fixed;
    bottom: 0;
    left: 0;
  }
</style>
