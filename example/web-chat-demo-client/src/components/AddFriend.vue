<template>
  <div class="box">
    <h2>Add Friend</h2>

    <input style="width: 400px;" title="" placeholder="target address" class="ela_text" type="text" v-model="address" />
    <input title="" placeholder="message" class="ela_text" type="text" v-model="msg" />
    <button @click="add()" class="ela_button">Add</button>

    <hr />

    <p v-for="item in apply_list">
      [{{item.userId}}] apply to be your friend with [{{item.msg}}] =>
      <button @click="accept(item)">Accept</button>
    </p>
  </div>
</template>

<script>
  import api from '../utils/request';
  import utility from '../utils/utility';
  import _ from 'lodash';

  export default{
    data(){
      return {
        address : '',
        msg : ''
      }
    },

    computed : {
      apply_list(){
        return this.$store.state.friend.apply_list;
      }
    },

    methods: {
      add(){
        if(!this.address){
          alert('invalid address');
          return false;
        }
        this.$root.getSocket().send('friend', {
          method : 'add',
          param : {
            address : this.address,
            msg : this.msg
          }
        }, (rs)=>{
          if(rs.code>0){
            this.refreshFriendList();
          }

          this.address = '';
          this.msg = '';
        });

      },
      accept(item){
        const ela = utility.getElaId(this.$route);
        api.go({
          path : `/api/friend/accept?userid=${item.userId}`,
          ela,
          success : (rs)=>{
            if(rs.code > 0){
              this.refreshFriendList();

              const list = _.clone(this.apply_list);
              _.remove(list, (d)=>d.userId===item.userId);
              this.apply_list = list;
            }
            else{
              this.$root.error(rs.error);
            }
          }
        });

        this.$root.getSocket().send('friend', {
          method : 'accept',
          param : {
            userid : item.userId
          }
        }, (rs)=>{
          if(rs.code > 0){
            this.refreshFriendList();
            this.$store.commit('friend.apply_list.remove', item);
          }
        });
      },

      refreshFriendList(){
        this.$root.getSocket().send('friend', {
          method : 'list'
        });
      }
    },

    sockets: {
      elastos_data(data){
        const type = data.elastos_type;
        data = _.omit(data, ['elastos_type']);

        if(type === 'friend_request'){
          this.apply_list.push(data);
        }
      }
    }
  }
</script>
