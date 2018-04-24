<template>
  <div class="cc-box c_me_mini_info">
    <p class="d_text">
      {{info.name || 'NA'}}
      <i v-bind:class="{online : info.online}" class="d_st"></i>
    </p>
    <p class="d_text d_t2">{{address}}</p>

    <div class="d_icon">
      <el-tooltip placement="top">
        <div slot="content">setting</div>
        <i @click="goSetting()" class="cc-icon el-icon-setting"></i>
      </el-tooltip>

    </div>
  </div>
</template>
<script>
  export default {

    computed : {
      info(){
        return this.$store.state.me.info;
      },
      address(){
        return this.$store.state.me.address;
      }
    },

    methods : {
      goSetting(){
        this.$router.push('setting');
      }
    }
  }
</script>
<style lang="scss">
  .c_me_mini_info{
    background: #2b2c31;
    height: 60px !important;
    position: absolute;
    left: 0;
    bottom: 0;
    text-align: left;
    padding-left: 15px;

    .d_text{
      font-size: 1.2rem;
      color: #fff;
      margin-top: 8px;
      max-width: 200px;
      overflow: hidden;
      text-overflow: ellipsis;
    }

    .d_t2{
      font-size: 1rem;
      margin-top: 4px;
    }

    .d_icon{
      position: absolute;
      right: 10px;
      top: 10px;

      .cc-icon{

      }
    }

    .d_st{
      display: inline-block;
      width: 10px;
      height: 10px;
      border-radius: 5px;
      background: #2c3e50;
      border: 1px solid #878787;
      position: relative;
      left: 5px;
      top: 0;

      &.online{
        background: #ff0;
        border-color: #cdcdcd;
      }
    }
  }
</style>
