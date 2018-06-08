<template>
  <div class="cc-box c_left_one">

    <el-tooltip placement="bottom">
      <div slot="content">Add friend</div>
      <i @click="addFriend()" class="cc-icon el-icon-circle-plus-outline"></i>
    </el-tooltip>

    <el-tooltip v-if="apply_list.length > 0" placement="bottom">
      <div slot="content">System Message</div>
      <el-badge style="margin-top: 20px;" :value="apply_list.length" class="item">
        <i style="font-size: 2.5rem;" @click="systemDialogVisible=true" class="cc-icon el-icon-message"></i>
      </el-badge>
    </el-tooltip>

    <el-button @click="goToA()">A</el-button>
    <el-button @click="goToB()">B</el-button>


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

    <el-dialog title="System Message" custom-class="d_dialog" width="70vw" append-to-body :visible.sync="systemDialogVisible">
      <ul class="d_system_message_ul">
        <li class="d_li" v-for="item in apply_list">
          <p style="padding-right: 120px;">
            {{item.userId}} apply to be your friend with "{{item.msg}}".
          </p>
          <el-button class="d_btn" type="success" size="mini" @click="accept(item)">Accept</el-button>

        </li>
      </ul>
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
        },

        systemDialogVisible : false
      };
    },
    computed : {
      apply_list(){
        return this.$store.state.friend.apply_list;
      }
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

              this.$root.getCarrier().execute('getFriends');
            }catch(e){

            }
          }
        });

      },
      accept(item){
        try{
          this.$root.getCarrier().execute('acceptFriend', item.userId);
          this.$store.commit('friend.apply_list.remove', item);

          this.$root.getCarrier().execute('getFriends');
          if(this.apply_list.length < 1){
            this.systemDialogVisible = false;
          }
        }catch(e){

        }
      },
      goToA(){
        this.$router.push('home');
      },
      goToB(){
        this.$router.push('session_test');
      }
    }
  }
</script>

<style lang="scss">
  .c_left_one{
    padding-top: 20px;

    .cc-icon{
      font-size: 3.2rem;
      cursor: pointer;
    }
  }
  .d_system_message_ul{
    list-style: none;
    padding-top: 12px;
    border-top: 1px solid #0074D9;

    .d_li{
      color: #0074D9;
      font-size: 1rem;
      border-top: 1px dashed #cdcdcd;
      padding: 8px 0;
      position: relative;

      &:first-child{
        border-top: none;
      }

      .d_btn{
        position: absolute;
        top: 0;
        right: 0;
      }
    }
  }
</style>

