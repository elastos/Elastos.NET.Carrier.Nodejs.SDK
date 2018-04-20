<template>
  <div class="box">
    <h2>Friend Info</h2>
    <button class="ela_button" @click="refresh()" style="position:absolute;right:25px;top:10px;">Refresh</button>
    <p v-for="item in list" @click="showInfo(item)">
      {{item.userId}} - {{item.online ? 'Online' : 'Offline'}}
       ----
      <button @click="removeFriend(item, $event)">Remove</button>
    </p>

    <hr />

    <span>Send To {{target ? target.userId : 'NA'}}</span>
    <br />
    <input style="width: 400px;margin-left:20px;" title="" type="text" class="ela_text" v-model="msg" />
    <button @click="sendMessage()" class="ela_button">Send</button>
    <hr />
    <ul>
      <li v-for="item in message_list">
        {{item.from}} : {{item.msg}}
      </li>
    </ul>
  </div>
</template>

<script>
  import api from '../utils/request';

  export default {
    data(){
      return {
        target : null,
        msg : '',
      };
    },

    computed : {
      list(){
        return this.$store.state.friend.list;
      },
      message_list(){
        if(this.target){
          return this.$store.state.friend.message[this.target.userId] || [];
        }
        else{
          return [];
        }
      }
    },
    methods: {
      showInfo(item){
        this.$store.commit('add_log', JSON.stringify(item));
        this.target = item;
      },
      sendMessage(){
        const msg = this.msg;
        if(!msg){
          alert('invalid message');
          return false;
        }

        if(!this.target){
          alert('please select a friend');
          return false;
        }
        if(!this.target.online){
          alert('This friend is not online, please wait call him online firstly.');
          return false;
        }


        this.$root.getSocket().send('friend', {
          method : 'message',
          param : {
            userid : this.target.userId,
            msg : this.msg
          }
        }, (rs)=>{
          if(rs.code > 0){
            this.msg = '';
            this.$root.log(`send message to ${this.target.userId} : ${msg}`);
            this.$store.commit('friend.message.add', {
              key : this.target.userId,
              from : 'ME',
              msg : msg
            })
          }
          else{
            this.$root.error(rs.error);
          }
        });
      },
      removeFriend(item, e){
        e.stopPropagation();
        this.$root.getSocket().send('friend', {
          method : 'remove',
          param : {
            userid : item.userId
          }
        });
      },
      refresh(){
        this.$root.getSocket().send('friend', {
          method : 'list'
        });
      }
    }
  }
</script>
