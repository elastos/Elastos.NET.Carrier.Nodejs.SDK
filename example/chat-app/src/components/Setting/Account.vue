<template>
  <div class="cc-box c_account">
    <h2 class="h2 h-center">Account</h2>
    <div class="d_box">
      <p v-for="item in list" class="d_p">
        <span class="d_s1">{{item.key}}</span>
        <span class="d_s2">{{item.value}}</span>
      </p>
      <el-button @click="logout()" class="d_btn" type="danger">Logout</el-button>
    </div>
  </div>
</template>
<script>

  export default {
    data(){
      const state = this.$store.state;
      return {
        list : [
          { key : 'Account', value : state.account.username },
          { key : 'UserId', value : state.me.info.userId },
          { key : 'Address', value : state.me.address }
        ]
      }
    },
    methods : {
      logout(){
        this.$confirm('Are you sure to Logout?')
          .then(_ => {

          })
          .catch(_ => {});
      }
    }
  }

</script>
<style lang="scss">
  .c_account{
    padding: 50px 40px;
    text-align: left;

    .d_box{
      padding: 20px 60px;

      .d_p{
        display: flex;
        padding: 12px 0;
        border-bottom: 1px solid #787878;
        justify-content: space-between;
      }
      .d_s1{
        font-size: 1.2rem;
        color: #fff;
        font-weight: 700;
      }
      .d_s2{
        font-size: 1.2rem;
        color: #c9c9c9;
      }

      .d_btn{
        float: right;
        margin-top: 40px;
        width: 100%;
      }
    }
  }
</style>
