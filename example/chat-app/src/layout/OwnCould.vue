<template>
  <el-container class="cc-box c_home c_own_cloud">
    <el-aside class="d_left_one" width="70px">
      <Left_One />
    </el-aside>
    <el-container v-if="!connect" class="d_main">
      <el-button @click.native="start()">Connect to Server</el-button>
    </el-container>
    <el-container v-if="connect" class="d_main">

      <el-breadcrumb separator="/">
        <el-breadcrumb-item @click.native="clickBreadcrumb(item)" v-for="(item, index) in pathList" :key="index">{{item.key}}</el-breadcrumb-item>
      </el-breadcrumb>

      <ul class="d_ul">
        <li class="d_li" @click="goToPath(item)" :key="index" v-for="(item, index) in list">
          <i v-if="item.type==='dir'" class="el-icon-goods"></i>
          <i v-if="item.type==='file'" class="el-icon-tickets"></i>

          <span>{{item.name}}</span>
        </li>
      </ul>
    </el-container>

    <el-dialog :title="file_content.title" custom-class="file_content_dialog" height="80vh" append-to-body :visible.sync="file_dialog">
      <div v-if="file_dialog" style="height:100%;">
        <div class="e_m" v-if="file_content.type === 'text'">
          {{file_content.content}}
        </div>
        <div class="e_m" v-if="file_content.type === 'stream'">
          <video v-if="file_content.ext === 'mp4'" controls :src="file_content.stream"></video>

          <iframe style="width:100%;height:100%;border:none;" v-if="file_content.ext === 'pdf'" :src="file_content.stream"></iframe>
        </div>
        <div class="e_m" v-if="file_content.type === 'image'" style="text-align: center;">
          <img :src="file_content.stream" style="max-width:100%;margin: 0 auto;max-height:100%;" />
        </div>
      </div>
    </el-dialog>

    <Loading text="loading..." v-if="loading" />
  </el-container>
</template>
<script>
  import Left_One from '../components/Home/Left_One';
  import Loading from '../components/common/Loading';
  import _ from 'lodash';

  import OwnCloudService from '../service/OwnCouldService';

  let oc;
  export default {
    data(){
      return {
        C : {

          // local own could
          address : 'a9dgoBeSANZSwxnX5bEHc28xC9zGF5BTqc31nvUDV83Ymbe8Q8jF',
          userId : 'G5k6DJg6akoevSGmwe9YwVGBmngKS1bXjNHo6eWDUCuP',

          // tencent own cloud
          // address : 'RmymGXTYxAtFUL5FWA3R4nL4grZs1mj6TTyEwc9NqidsgfbHe6Ub',
          // userId : 'CGnVMJUit5t483mkzvAuFL6ETrVj9t1Mx5c953bC2Vpa',


          secret : '2bb80d537b1da3e38bd30361aa855686bde0eacd7162fef6a25fe97bf527a25b',
          service_name : 'owncloud',
          host : 'localhost',
          port : _.random(50000, 60000).toString()
        },

        connect : false,
        loading : false,

        list: [],
        path : '',

        file_dialog : false,
        file_content : {}
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
      },

      pathList(){
        const pl = this.path.split('/');
        const len = pl.length;
        return _.map(pl, (v, i)=>{
          const tmp = {};
          if(v === ''){
            tmp.key = 'ROOT';
            tmp.path = '';
          }
          else{
            tmp.key = v;
            tmp.path = pl.slice(0, i+1).join('/');
          }
          if(i < len-1){
            tmp.click = true;
          }

          return tmp;
        });
      }
    },

    created(){
      oc = OwnCloudService.create();
    },

    async mounted(){
      // connect to local owncloud to test
      // oc.connect(`http://localhost:8000/remote.php/dav/files/admin`, 'admin', '111111');
      // this.connect = true;
      //
      // await this.getFileList('/');
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
        oc.connect(`http://${this.C.host}:${this.C.port}/remote.php/dav/files/admin`, 'admin', '111111');
        this.connect = true;

        await this.getFileList('/');
      },


      // click event
      async goToPath(item){
        if(item.type === 'dir'){
          if(item.name){
            this.path += '/'+item.name;
          }
          return await this.getFileList('/'+this.path);
        }
        else if(item.type === 'file'){
          const path = this.path + '/' + item.name;
          this.loading = true;
          const rs = await oc.getFile(path);

          this.file_content = {
            title : item.name,
            ...rs
          };
          this.file_dialog = true;
          this.loading = false;
        }
      },
      async getFileList(path){
        this.loading = true;
        try{
          this.list = await oc.getFileList(path);
        }catch(e){
          this.$root.errorMessage(e);
        }
        this.loading = false;
      },
      async clickBreadcrumb(item){
        if(item.click){
          this.path = item.path;
          await this.getFileList(item.path);
        }
      }
    }
  }
</script>
<style lang="scss">
  .c_own_cloud{
    .d_main{
      background: #35393f;
      padding: 12px 25px;
      color: #ccc;
      display: block;
    }


    .d_ul{
      list-style: none;
      margin: 0;
      text-align: left;
      width: 100%;

      .d_li{
        padding: 8px 0;
        border-bottom: 1px solid #8c8c8c;
        cursor: pointer;

        &:hover{
          color: #ffffff;
        }
      }
    }

    .el-breadcrumb{
      padding: 12px 0;
      font-size: 18px;

      .el-breadcrumb__inner{
        cursor: pointer;
        color: #eeeeee;

        &:hover{
          font-weight: 700;
          color: #fff;
          cursor: pointer;
        }

      }
      .el-breadcrumb__item:last-child .el-breadcrumb__inner{
        cursor: default;
        color: #606266;
        &:hover{
          cursor: default;
          color: #606266;
          font-weight: normal;
        }
      }
    }
  }
  .file_content_dialog{
    margin-top:10vh !important;
    width: 80vw;
    height: 80vh;
    display: flex;
    flex-direction: column;
    .el-dialog__body{
      flex: 1;
      overflow-y: auto;
    }
    .e_m{
      height: 100%;
    }
  }
</style>
