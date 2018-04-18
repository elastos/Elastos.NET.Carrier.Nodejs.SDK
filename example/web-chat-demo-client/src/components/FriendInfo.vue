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
  </div>
</template>

<script>
  import api from '../utils/request';
  import utility from '../utils/utility';
  import _ from 'lodash';
  import VUE from 'vue';

  export default {
    data(){
      return {
        flag : true,
        list : [],

        target : null,
        msg : '',
        ela : utility.getElaId(this.$route)
      };
    },

    created(){

    },

    mounted(){

    },
    sockets: {
      elastos_data(data){
        const type = data.elastos_type;
        data = _.omit(data, ['elastos_type']);
        if(type === 'friend_list'){
          if(this.flag){
            this.list = [data];
            this.flag = false;
          }
          else{
            this.list.push(data);
          }
        }
        else if(type === 'friend_list_end'){
          this.flag = true;
        }
        else if(type === 'friend_status'){

          const n = _.findIndex(this.list, (item)=>item.userId===data.userId);
          if(n !== -1){

            this.$set(this.list, n, _.extend({}, this.list[n], data));
          }
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

        api.go({
          path : `/api/message/send?userid=${this.target.userId}&msg=${encodeURIComponent(msg)}`,
          ela : this.ela,
          success : (rs)=>{
            if(rs.code > 0){
              this.msg = '';
              this.$root.log(`send message to ${this.target.userId} : ${msg}`);
            }
            else{
              this.$root.error(rs.error);
            }
          }
        });
      },
      removeFriend(item, e){
        e.stopPropagation();
        api.go({
          path : `/api/friend/remove?userid=${item.userId}`,
          ela : this.ela,
          success : (rs)=>{
            console.log(rs);
            if(rs.code > 0){
              const list = _.clone(this.list);
              _.remove(list, (d)=>d.userId===item.userId);
              this.list = list;

              // this.$store.commit('add_log', `Friend ${item.userId} has been removed`);
            }
            else{
              this.$root.error(rs.error);
            }
          }
        });
      },
      refresh(){
        api.go({
          path: '/api/friend/list',
          ela: this.ela,
          success: (rs)=>{
            // console.log(rs)
          }
        });
      }
    }
  }
</script>
