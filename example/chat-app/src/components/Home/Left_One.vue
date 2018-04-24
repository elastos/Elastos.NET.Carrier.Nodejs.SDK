<template>
  <div class="cc-box c_left_one">

    <el-tooltip placement="bottom">
      <div slot="content">Add friend</div>
      <i @click="addFriend()" class="cc-icon el-icon-circle-plus-outline"></i>
    </el-tooltip>


    <el-dialog title="Add friend" custom-class="d_dialog" width="70vw" append-to-body :visible.sync="dialogFormVisible">
      <el-form :model="form" :rules="formRules" ref="form">
        <el-form-item label="Address" prop="address">
          <el-input v-model="form.address" auto-complete="off"></el-input>
        </el-form-item>
        <el-form-item label="Message" prop="msg">
          <el-input v-model="form.msg" auto-complete="off"></el-input>
        </el-form-item>

      </el-form>
      <div slot="footer" class="dialog-footer">
        <el-button @click="dialogFormVisible = false">Cancel</el-button>
        <el-button type="primary" @click="confirmAddFriend()">Confirm</el-button>
      </div>
    </el-dialog>
  </div>
</template>

<script>
  export default {
    data(){
      return {
        dialogFormVisible : false,
        form : {
          address : '',
          msg : ''
        },
        formRules : {
          address : [
            { required: true, message: 'please input friend node address', trigger: 'blur' }
          ],
          msg : [
            { required: true, message: 'please input message', trigger: 'blur' }
          ]
        }
      };
    },
    methods : {
      addFriend(){
        this.dialogFormVisible = true;
      },
      confirmAddFriend(){
        this.$refs.form.validate((valid)=>{
          if(valid){
            try{
              this.$root.getCarrier().execute('addFriend', this.form.address, this.form.msg);

              this.$root.successMessage('add success');
              this.$root.syncData('friend/add', this.form);
              this.$refs.form.resetFields();
              this.dialogFormVisible = false;
            }catch(e){

            }
          }
        });

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

