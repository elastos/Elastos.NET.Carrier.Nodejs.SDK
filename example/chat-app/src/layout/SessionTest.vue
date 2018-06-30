<template>
  <el-container class="cc-box c_home">
    <el-aside class="d_left_one" width="70px">
      <Left_One />
    </el-aside>
    <el-aside class="d_left_two" width="240px">
      <Left_Two />
    </el-aside>

    <el-container>
      <el-header style="padding:0;" height="50px">
        <Main_Header />
      </el-header>
      <el-main class="cc-box" style="padding: 0;">

        <el-button @click.native="createSession()">create session</el-button>
        <el-button @click.native="sendRequest()">send session request</el-button>
        <input type="file" ref="upload" @change="upload()" />

        <br />
        <el-button v-if="file" @click.native="sendTestStream()">Send aaa</el-button>
        <el-button v-if="file" @click.native="sendFileStream()">Send file</el-button>
        <hr />

        <el-button @click.native="receiveRequest()">receive request</el-button>

      </el-main>
    </el-container>

  </el-container>

</template>

<script>
  import Left_Two from '../components/Home/Left_Two';
  import Left_One from '../components/Home/Left_One';
  import Main_Header from '../components/Home/Main_Header';
  import Main_Friend_List from '../components/Home/Main_Friend_List';
  import Message_List from '../components/Home/Message_List';

  import Loading from '../components/common/Loading';
  import CarrierModel from '../service/CarrierModel';

  import {File} from '@/utility';
  import _ from 'lodash';

  export default {
    data(){
      return {
        stream : null,
        file : null
      }
    },
    components : {
      Left_Two,
      Left_One,
      Main_Header,
      Main_Friend_List,


      Loading
    },

    computed : {
      current(){
        return this.$store.state.friend.currentSelect;
      }
    },

    created(){

    },

    mounted(){



    },
    methods: {
      createSession(){
        if(!this.current.online){
          this.$root.errorMessage('This friend is not online, couldn\'t send message');
          return false;
        }

        const carrier = this.$root.getCarrier();
        carrier.execute('session_newSession', this.current.userId);
        this.stream = carrier.execute('session_addStream', ['portforwarding', 'multiplexing', 'reliable'], 'TEXT');


      },
      sendRequest(){
        const carrier = this.$root.getCarrier();
        carrier.execute('session_request');
      },
      upload() {
        const obj = this.$refs.upload;
        const file = obj.files[0];

        this.file = file;
        console.log(file);
      },
      sendTestStream(){
        const buffer = new Buffer('aaa');

        try {
          this.$root.getCarrier().execute('stream_write', this.stream.id, buffer);
        } catch (e) {
          console.error(e);
        }
      },
      sendFileStream(){
        const f = new File(this.file);
        const list = f.buildFileBuffer();

        try {
          _.each(list, (buffer)=>{
            // console.log(buffer);
            this.$root.getCarrier().execute('stream_write', this.stream.id, buffer);

          });
        } catch (e) {
          console.error(e);
        }
      },

      receiveRequest(){
        this.createSession();
        _.delay(()=>{
          this.$root.getCarrier().execute('session_replyRequest', true);
        }, 5000);

      }
    }
  };
</script>

<style lang="scss">
  .c_home{
    width: 100%;
    position: relative;

    .d_left_one{
      background: #42b983;
    }
    .d_left_two{
      position: relative;
    }

    .d_msg_area{
      /*padding-right: 240px;*/
      background: #ccc;
      height: 100%;
    }
    .d_friend_area{
      width: 240px;
      position: absolute;
      right: 0;
      top: 0;
      height: 100%;
    }
  }
</style>
