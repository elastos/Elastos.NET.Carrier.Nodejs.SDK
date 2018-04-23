<template>
  <el-container class="cc-box c_setting">

    <el-tabs tab-position="left" class="d_tabs" value="profile">
      <el-tab-pane disabled label="User Setting"></el-tab-pane>
      <el-tab-pane name="profile" label="Profile"><h1>用户管理</h1></el-tab-pane>

      <el-tab-pane class="d_gap" disabled label="Application Setting"></el-tab-pane>
    </el-tabs>
  </el-container>

</template>
<script>
  export default {
    data(){
      console.log(this.carrier);
      return {

      };
    }
  }
</script>

<style lang="scss">
  @import '../css/index.scss';

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

        &.is-active{
          color: #ff0;
        }

        &:hover{
          text-decoration: underline;
        }

        &.is-disabled{
          font-size: 0.9rem;
          color: #a2a2a2;

          &:hover{
            text-decoration: none;
          }
        }
      }

      .gap{
        .el-tabs__item{
          margin-top: 50px;
        }
      }
      .el-tabs__active-bar{

      }
    }
  }
</style>
