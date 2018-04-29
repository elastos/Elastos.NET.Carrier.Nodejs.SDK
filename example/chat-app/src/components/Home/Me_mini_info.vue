<template>
  <div class="cc-box c_me_mini_info">
    <p class="d_text">
      <el-dropdown @command="handleCommand" trigger="click">
        <span class="el-dropdown-link">
          <span class="d_text">{{info.name || 'NA'}}</span>
          <i v-bind:class="presence_class()" class="d_st"></i>
        </span>
        <el-dropdown-menu slot="dropdown">
          <el-dropdown-item command="0">Free</el-dropdown-item>
          <el-dropdown-item command="1" divided>Away</el-dropdown-item>
          <el-dropdown-item command="2">Busy</el-dropdown-item>
        </el-dropdown-menu>
      </el-dropdown>
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
  import {presence_class} from '../../utility';
  import _ from 'lodash';
  export default {

    computed : {
      info(){
        return this.$store.state.me.info;
      },
      address(){
        return this.$store.state.me.address;
      },
      online(){
        return this.$store.state.me.online;
      }
    },

    methods : {
      goSetting(){
        this.$router.push('setting');
      },
      handleCommand(cmd){
        try{
          cmd = _.toNumber(cmd);
          this.$root.getCarrier().execute('setSelfPresence', cmd);
          this.$root.syncData('me/presence', cmd);
        }catch(e){
          console.error(e)
        }

      },
      presence_class(){
        return presence_class(this.online, this.$store.state.me.presence, 'd_')
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

      &.d_none{
        background: lime;
        border: none;
      }
      &.d_away{
        background: #ff0;
        border: none;
      }
      &.d_busy{
        background: #f00;
        border: none;
      }
    }
  }
</style>
