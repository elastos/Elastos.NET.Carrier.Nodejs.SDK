<template>
  <div class="cc-box c_left_two">
    <div class="d_top">

      <i @click="refreshFriendList()" class="cc-icon el-icon-refresh"></i>
    </div>

    <Main_Friend_list />

    <Me_mini_info />
  </div>
</template>
<script>
  import Me_mini_info from './Me_mini_info';
  import Main_Friend_list from './Main_Friend_List';
  export default{
    components : {
      Me_mini_info,
      Main_Friend_list
    },
    methods : {
      refreshFriendList(){
        this.$root.getCarrier().execute('getFriends');
      }
    }
  }
</script>
<style lang="scss">
  .c_left_two{
    padding-top: 50px;

    .d_top{
      background: #353940;
      border-bottom: 1px solid #292b39;
      box-shadow: 0 1px 0 #222;
      z-index: 1;
      border-right: 1px solid #292b39;
      position: absolute;
      top: 0;
      left: 0;
      width: 100%;
      height: 50px;

      .cc-icon{
        color: #d9d9d9;
        position: absolute;
        right: 12px;
        font-size: 2rem;
        top: 12px;
        cursor: pointer;
      }
    }
  }
</style>
