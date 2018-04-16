<template>
  <div class="box">
    <h2>Add Friend</h2>

    <input style="width: 400px;" title="" placeholder="target address" class="ela_text" type="text" v-model="address" />
    <input title="" placeholder="message" class="ela_text" type="text" v-model="msg" />
    <button @click="add()" class="ela_button">Add</button>
  </div>
</template>

<script>
  import api from '../utils/request';
  import utility from '../utils/utility';

  export default{
    data(){
      return {
        address : '',
        msg : ''
      }
    },

    methods: {
      add(){
        const ela = utility.getElaId(this.$route);
        if(!this.address){
          alert('invalid address');
          return false;
        }
        api.go({
          path : `/api/friend/add?address=${this.address}&msg=${this.msg}`,
          ela,
          success : (rs)=>{
            if(rs.code>0){

            }
            else{
              alert(rs.error);
            }

            this.address = '';
            this.msg = '';
          }
        });
      }
    }
  }
</script>
