<template>
  <div class="cc-box c_left_one">

    <el-tooltip placement="bottom">
      <div slot="content">Add friend</div>
      <i @click="addFriend()" class="cc-icon el-icon-circle-plus-outline"></i>
    </el-tooltip>
  </div>
</template>

<script>
  export default {
    methods : {
      addFriend(){

      }
    }
  }
</script>

<style lang="scss">
  .c_left_one{
    padding-top: 50px;

    .cc-icon{
      font-size: 3.2rem;
      cursor: pointer;
    }
  }
</style>

