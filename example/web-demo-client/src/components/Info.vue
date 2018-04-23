<template>
  <div class="box c_info">
    <h2>Personal Info -- {{$root.getId()}}</h2>
    <ul>
      <li>Name : {{me.name}}</li>
      <li>User ID : {{me.userId}}</li>
      <li>Email : {{me.email}}</li>
      <li>Region : {{me.region}}</li>
      <li>Phone : {{me.phone}}</li>
      <li>Gender : {{me.gender}}</li>
      <li>Address : {{address}}</li>
      <li>Connect network : {{online}}</li>
    </ul>
    <hr />
    <div>
      <select title="" ref="key">
        <option value="name">Name</option>
        <option value="email">Email</option>
        <option value="phone">Phone</option>
        <option value="gender">Gender</option>
        <option value="region">Region</option>
      </select>
      <input title="" type="text" ref="value" />
      <button @click="changeValue()">Confirm</button>
    </div>
  </div>

</template>

<script>
  import api from '../utils/request';
  import utility from '../utils/utility';
  export default {
    data(){
      return {}
    },
    computed: {
      me(){
        return this.$store.state.me.info;
      },
      address(){
        return this.$store.state.me.address;
      },
      online(){
        return this.$store.state.me.online;
      }
    },

    methods: {
      changeValue(){
        const key = this.$refs.key.value;
        const value = this.$refs.value.value || '';
        console.log(key, value);

        this.$root.getSocket().send('me', {
          method : 'set',
          param : {
            key,
            value
          }
        });
      }
    },
    sockets: {
      elastos_data(data){
        const type = data.elastos_type;
        if(type === 'network_connect'){
          this.$root.log((data.connect?'connect':'disconnect') +' to carrier network');
          this.network = data.connect;
        }
      }
    }
  }
</script>

<style>
  .c_info{
    text-align: left;
  }
</style>
