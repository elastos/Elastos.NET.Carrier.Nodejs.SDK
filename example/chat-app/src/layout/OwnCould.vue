<template>
  <el-container class="cc-box c_home">
    <el-aside class="d_left_one" width="70px">
      <Left_One />
    </el-aside>
    <el-container>
      <div>
        <el-button @click.native="start()" v-if="!connect">Connect to Server</el-button>
      </div>
    </el-container>

    <!--<el-dialog title="Add OwnCloud server" custom-class="d_dialog" width="70vw" append-to-body :visible.sync="dialogFormVisible">-->
      <!--<el-form :model="form" :rules="formRules" ref="form">-->
        <!--<el-form-item label="Address" prop="address">-->
          <!--<el-input v-model="form.address" auto-complete="off"></el-input>-->
        <!--</el-form-item>-->
        <!--<el-form-item label="Message" prop="msg">-->
          <!--<el-input v-model="form.msg" auto-complete="off"></el-input>-->
        <!--</el-form-item>-->

      <!--</el-form>-->
      <!--<div slot="footer" class="dialog-footer">-->
        <!--<el-button @click="dialogFormVisible = false">Cancel</el-button>-->
        <!--<el-button type="primary" @click="confirmAddFriend()">Confirm</el-button>-->
      <!--</div>-->
    <!--</el-dialog>-->

    <Loading text="loading..." v-if="loading" />
  </el-container>
</template>
<script>
  import Left_One from '../components/Home/Left_One';
  import Loading from '../components/common/Loading';
  import _ from 'lodash';

  import OwnCloudService from '../service/OwnCouldService';
  export default {
    data(){
      return {
        C : {
          // address : 'a9dgoBeSANZSwxnX5bEHc28xC9zGF5BTqc31nvUDV83Ymbe8Q8jF',
          address : '5jejnpwXj49tScjLrp7zJu77qRgWZsPQYnpoHavbDqgaasgmnvs8',
          //userId : 'G5k6DJg6akoevSGmwe9YwVGBmngKS1bXjNHo6eWDUCuP',
          userId : '39xaQwJ8fHByWoMyaLUGca8ZT4hc24XrumDCogAH7w2C',
          secret : '2bb80d537b1da3e38bd30361aa855686bde0eacd7162fef6a25fe97bf527a25b',
          service_name : 'owncloud',
          host : 'localhost',
          port : _.random(50000, 60000).toString()
        },

        connect : false,
        loading : false,
        // dialogFormVisible : false,
        // form : {
        //   address : '',
        //   msg : ''
        // },
        // formRules : {
        //   address : [
        //     { required: true, message: 'please input friend node address', trigger: 'blur' }
        //   ],
        //   msg : [
        //     { required: true, message: 'please input message', trigger: 'blur' }
        //   ]
        // },
        list: []
      }
    },
    components: {
      Left_One,
      Loading
    },
    computed : {
      current(){
        return this.$store.state.friend.currentSelect;
      },
      me(){
        return this.$store.state.me;
      }
    },

    async mounted(){

    },
    methods: {
      async start(){
        if(!this.me.online){
          this.$root.errorMessage('please wait carrier online');
          return false;
        }

        this.loading = true;
        try{
          this.$root.getCarrier().execute('addFriend', this.C.address, this.C.secret);
          this.$root.getCarrier().execute('getFriends');
        }catch(e){

        }

        const carrier = this.$root.getCarrier();
        carrier.execute('session_newSession', this.C.userId);
        this.stream = carrier.execute('session_addStream', ['portforwarding', 'multiplexing', 'reliable'], 'TEXT');

        console.log(111);
        _.delay(this.sendRequest, 2000);

      },

      sendRequest(){
        try{
          const carrier = this.$root.getCarrier();
          carrier.execute('session_request');

          console.log(222);
          _.delay(this.openPortForward, 2000);
        }catch(e){
          _.delay(this.sendRequest, 2000);
        }

      },

      openPortForward(){
        try{
          const carrier = this.$root.getCarrier();
          console.log(3, this.stream.id, this.C.service_name, 'tcp', this.C.host, this.C.port);
          carrier.execute('stream_openPortForwarding', this.stream.id, this.C.service_name, 'tcp', this.C.host, this.C.port);

          console.log(333);
          _.delay(this.connectToServer, 2000);
        }catch(e){
          _.delay(this.openPortForward, 2000);
        }

      },

      async connectToServer(){
        const oc = new OwnCloudService();
        oc.connect(`http://${this.C.host}:${this.C.port}/remote.php/dav/files/admin`, 'admin', '111111');
        const list = await oc.getFileList('/');

        this.list = _.map(list, (item)=>{
          item.type = item.isFile() ? 'file' : 'dir';

          return item;
        });
        console.log(`http://${this.C.host}:${this.C.port}/remote.php/dav/files/admin`, this.list);

        this.loading = false;
      }
    }
  }
</script>
