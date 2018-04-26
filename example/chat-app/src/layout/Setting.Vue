<template>
  <el-container class="cc-box c_setting">

    <el-tabs @tab-click="tabClick($event)" tab-position="left" class="d_tabs" :value="select">
      <el-tab-pane disabled label="User Setting"></el-tab-pane>
      <el-tab-pane name="profile" label="Profile">
        <Profile />
      </el-tab-pane>

      <el-tab-pane class="d_gap" disabled label="Application Setting"></el-tab-pane>

      <el-tab-pane disabled label="Account Setting"></el-tab-pane>
      <!--<el-tab-pane name="password" label="Password"></el-tab-pane>-->
      <el-tab-pane name="logout" label="Logout"></el-tab-pane>
    </el-tabs>

    <i class="cc-icon el-icon-circle-close-outline d_close" @click="$router.back()"></i>
  </el-container>

</template>
<script>
  import Profile from '../components/Setting/Profile';
  export default {
    data(){
      return {
        select : 'profile'
      };
    },
    components : {
      Profile
    },
    methods : {
      tabClick(tab){
        if(tab.name === 'logout'){
          this.select = 'profile'
        }

      }
    }
  }
</script>

<style lang="scss">

  .c_setting{
    background: rgb(53, 57, 64);

    .d_tabs{

      width: 100vw;

      .el-tabs__header{
        width: 240px;
        padding-left: 80px;
        background: rgb(47, 49, 54);
      }

      .el-tabs__nav{
        top: 80px;

      }
      .el-tabs__item{
        color: #e9e9e9;
        font-weight: 700;
        font-size: 1.2rem;

        &:nth-child(2){
          margin-top: 0 !important;
        }

        &:last-child{
          color: #f00 !important;
        }

        &.is-active{
          color: #ff0;
        }

        &:hover{
          text-decoration: underline;
        }

        &.is-disabled{
          font-size: 0.9rem;
          color: #a2a2a2;
          margin-top: 80px;
          border-bottom: 1px solid #999;


          &:hover{
            text-decoration: none;
          }
        }

      }

      .el-tabs__active-bar{
        background: none;
      }
    }

    .d_close{
      position:absolute;
      right: 40px;top: 25px;
      font-size: 3rem;
      color: #d9d9d9;
      cursor: pointer;
    }
  }

  .el-tab-pane{
    width: 100%;
    height: 100vh;

  }
</style>
