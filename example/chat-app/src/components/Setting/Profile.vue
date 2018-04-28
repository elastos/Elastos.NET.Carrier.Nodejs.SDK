<template>
  <div class="cc-box c_profile">
    <h2 class="h2 h-center">Profile</h2>

    <el-form class="d_form" label-position="top" label-width="100vw" :model="info">
      <el-form-item label="Name">
        <el-input v-model="info.name"></el-input>
      </el-form-item>
      <el-form-item label="Email">
        <el-input v-model="info.email"></el-input>
      </el-form-item>
      <el-form-item label="Description">
        <el-input v-model="info.description"></el-input>
      </el-form-item>
      <el-form-item label="Phone">
        <el-input v-model="info.phone"></el-input>
      </el-form-item>
      <el-form-item label="Gender">
        <el-input v-model="info.gender"></el-input>
      </el-form-item>
      <el-form-item label="Region">
        <el-input v-model="info.region"></el-input>
      </el-form-item>
      <!--<el-form-item label="User ID">-->
        <!--<el-input readonly v-model="info.userId"></el-input>-->
      <!--</el-form-item>-->
      <!--<el-form-item label="User Address">-->
        <!--<el-input readonly v-model="address"></el-input>-->
      <!--</el-form-item>-->

      <el-form-item size="large" style="margin-top: 50px;text-align: right;">
        <el-button type="primary" @click="confirm()">Confirm</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>
<script>
  export default {
    data(){
      return {
      };
    },
    computed : {
      info(){
        return this.$store.state.me.info;
      },
      address(){
        return this.$store.state.me.address;
      }
    },
    methods : {
      confirm(){
        console.log(this.info);

        try{
          const rs = this.$root.getCarrier().execute('setSelfInfo', this.info);
          this.$root.syncData('me/info', rs);

          this.$root.successMessage('success');
        }catch(e){
          console.error(e);
        }

      }
    }
  }
</script>

<style lang="scss">
  .c_profile{
    padding: 50px 40px;
    text-align: left;

    .d_form{
      padding: 20px 60px;
      display: block;

      .el-form-item{
        margin-bottom: 10px;

        .el-form-item__label{
          font-size: 1.2rem;
          color: #eee;
          padding: 0;
        }
      }
    }

  }
</style>
