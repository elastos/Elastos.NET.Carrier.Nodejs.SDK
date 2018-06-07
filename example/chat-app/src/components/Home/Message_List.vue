<template>
  <div class="cc-box c_message_list">

    <ul class="d_ul" ref="ul">
      <Message_Item v-for="(item, index) in list" :key="index" :item="item" />
    </ul>

    <div class="d_send">
      <el-input clearable placeholder="Input message" v-model="msg" @keyup.native="send($event)">
        <template slot="prepend" style="width: 100px;">{{min_name()}}</template>
      </el-input>

      <el-button @click.native="createSession()">create session</el-button>
    </div>
  </div>

</template>
<script>
  import Message_Item from './Message_Item';
  import _ from 'lodash';
  export default {
    data(){
      return {
        msg : ''
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

        const carrier = this.$root.getCarrier();
        carrier.execute('session_newSession', this.current.userId);
        carrier.execute('session_addStream', ['reliable']);
        _.delay(()=>{
          carrier.execute('session_request');
        }, 2000);

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

      .el-input__inner, .el-input-group__prepend{
        background: #474b53;
        color: #eeeeee;
      }
    }
  }
</style>
