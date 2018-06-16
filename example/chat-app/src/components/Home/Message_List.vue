<template>
  <div class="cc-box c_message_list">

    <ul class="d_ul" ref="ul">
      <Message_Item v-for="(item, index) in list" :key="index" :item="item" />
    </ul>

    <div class="d_send">
      <el-input clearable placeholder="Input message" v-model="msg" @keyup.native="send($event)">
        <template slot="prepend" style="width: 100px;">{{min_name()}}</template>

        <el-button class="" @click="sendFile()" slot="append" icon="el-icon-plus"></el-button>
      </el-input>

      <!--<el-button @click.native="createSession()">create session</el-button>-->
      <input type="file" hidden ref="upload" @change="upload()" />
    </div>

    <el-dialog title="Upload Dialog" :visible.sync="uploadDialog">
      <p>stream status : {{session_state}}</p>
      <el-button @click="transformFile()" v-if="session_state==='connected'">Send File</el-button>
    </el-dialog>
  </div>

</template>
<script>
  import Message_Item from './Message_Item';
  import {File, Media, OutMediaStream, InputMediaStream} from '@/utility';
  import _ from 'lodash';
  export default {
    data(){
      return {
        msg : '',
        file : null,
        uploadDialog : false
      };
    },
    components : {
      Message_Item
    },
    computed : {
      current(){
        return this.$store.state.friend.currentSelect;
      },
      list(){
        return this.$store.getters.getFriendMessageList();
      },
      session(){
        if(this.current){
          return this.$store.state.stream.session[this.current.userId];
        }

        return null;
      },
      session_state(){
        const carrier = this.$root.getCarrier();
        if(this.session && !_.isUndefined(this.session.state)){
          return carrier.getStaticVar('session_state')[this.session.state];
        }

        return 'NA';
      }
    },
    watch : {
      list(){
        this.scrollToBottom();
      }
    },
    mounted(){
      try{
        this.scrollToBottom();
      }catch(e){}
    },
    methods : {
      min_name(){
        if(!this.current.userId){
          return 'NA';
        }
        const name = this.current.name || this.current.userId;
        return name;
      },
      send(e){
        if(e.keyCode !== 13){
          return false;
        }

        if(!this.msg){
          return false;
        }

        if(!this.current.userId){
          this.$root.errorMessage('Please select a friend to chat');
          return false;
        }

        if(!this.current.online){
          this.$root.errorMessage('This friend is not online, couldn\'t send message');
          return false;
        }

        try{
          this.$root.getCarrier().execute('sendFriendMessage', this.current.userId, this.msg);
          this.$store.commit('friend.message.add', {
            key : this.current.userId,
            from : this.$store.state.me.info,
            msg : this.msg
          });

          this.msg = '';


        }catch(e){

        }


      },
      scrollToBottom(){
        this.$nextTick(()=>{
          const div = this.$refs.ul;
          div.scrollTop = div.scrollHeight;
        });

      },
      createSession(){
        if(!this.current.online){
          this.$root.errorMessage('This friend is not online, couldn\'t send message');
          return false;
        }

        if(this.session_state === 'connected'){
          return false;
        }

        const carrier = this.$root.getCarrier();
        carrier.execute('session_newSession', this.current.userId);
        const stream = carrier.execute('session_addStream', this.current.userId, ['plain'], 'TEXT');
        this.$root.syncData('stream/session_state', {
          userId : this.current.userId,
          streamId : stream.id
        });


      },
      upload(){
        const obj = this.$refs.upload;
        const file = obj.files[0];

        console.log(this.session.state, file);

        if(this.session && this.session.state !== 4){
          this.$root.getCarrier().execute('session_request', this.current.userId);
        }

        this.file = file;
        this.uploadDialog = true;
        obj.value = '';
      },

      sendFile(){
        if(!this.current.userId){
          this.$root.errorMessage('Please select a friend to chat');
          return false;
        }

        this.createSession();

        this.$refs.upload.click();
      },
      transformFile(){
        const f = new File(this.file);
        const list = f.buildFileBuffer();

        try {
          _.each(list, (buffer)=>{
            // console.log(buffer);
            this.$root.getCarrier().execute('stream_write', this.session.id, buffer);

          });
        } catch (e) {
          console.error(e);
        }
      }
    }
  }
</script>

<style lang="scss">
  .c_message_list{
    background: rgb(53, 57, 63);
    padding: 0 25px 85px 25px;

    .d_ul{
      height: 100%;
      padding: 0 0 20px 0;
      border-bottom: 1px solid rgb(65, 69, 73);
      list-style: none;
      overflow-y: auto;
    }

    .d_send{
      position: absolute;
      bottom: 20px;
      left: 25px;
      right: 25px;

      .el-input-group__prepend{
        max-width: 100px;
        overflow: hidden;
        text-overflow: ellipsis;
        padding: 0 8px;
      }

      .el-input__inner, .el-input-group__prepend, .el-input-group__append{
        background: #474b53;
        color: #eeeeee;
      }


    }
  }
</style>
