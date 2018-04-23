<template>
  <div class="c_login">
    <h2>Login App</h2>
    <br />
    <el-form :model="ruleForm" status-icon :rules="rules" ref="ruleForm" label-width="0" class="demo-ruleForm">
      <el-form-item label="" prop="username">
        <el-input type="text" placeholder="username" v-model="ruleForm.username" auto-complete="off"></el-input>
      </el-form-item>
      <el-form-item label="" prop="password">
        <el-input type="password" placeholder="password" v-model="ruleForm.password" auto-complete="off"></el-input>
      </el-form-item>

      <el-form-item>
        <el-button type="primary" @click="submitForm('ruleForm')">提交</el-button>
        <el-button @click="resetForm('ruleForm')">重置</el-button>
      </el-form-item>
    </el-form>
  </div>
</template>
<script>
  export default {
    data() {

      return {
        ruleForm: {
          username: '',
          password: ''
        },
        rules: {
          pass: [
            { validator: ()=>{}, trigger: 'blur' }
          ],
          checkPass: [
            { validator: ()=>{}, trigger: 'blur' }
          ],

        }
      };
    },
    methods: {
      submitForm(formName) {
        this.$refs[formName].validate((valid) => {
          if(valid) {
            this.$router.replace('/home');
          }
          else{
            console.log('error submit!!');
            return false;
          }
        });
      },
      resetForm(formName) {
        this.$refs[formName].resetFields();
      }
    }
  }
</script>
<style lang="scss">
  @import '../css/index.scss';
  .c_login{
    width: 500px;
    margin: 200px auto;
    padding: 40px 25px;
    border: 1px solid #321;
    background: #fff;
  }
</style>
