<template>
  <div class="box c_info">
    <h2>Personal Info</h2>
    <ul>
      <li>Name : {{me.name}}</li>
      <li>User ID : {{me.userId}}</li>
      <li>Email : {{me.email}}</li>
      <li>Region : {{me.region}}</li>
      <li>Phone : {{me.phone}}</li>
      <li>Gender : {{me.gender}}</li>
      <li>Address : {{address}}</li>
    </ul>
    <hr/>
    <div>
      <select ref="key">
        <option value="name">Name</option>
        <option value="email">Email</option>
        <option value="phone">Phone</option>
        <option value="gender">Gender</option>
        <option value="region">Region</option>
      </select>
      <input type="text" ref="value" />
      <button @click="changeValue()">Confirm</button>
    </div>
  </div>

</template>

<script>
  import api from '../utils/request';
  import utility from '../utils/utility';
  export default {
    data () {

      return {
        me : {},
        address : null
      }
    },
    mounted(){
      const ela = utility.getElaId(this.$route);
      api.go({
        path: '/api/me/get',
        ela,
        success: (rs)=>{
          this.me = rs.data
        }
      });

      api.go({
        path : '/api/me/address',
        ela,
        success : (rs)=>{
          this.address = rs.data;
        }
      })
    },
    methods: {
      changeValue(){
        const key = this.$refs.key.value;
        const value = this.$refs.value.value || '';
        console.log(key, value);

        api.go({
          path : `/api/me/set?key=${key}&value=${value}`,
          ela : utility.getElaId(this.$route),
          success : (rs)=>{
            console.log(rs);
            if(rs.code > 0){
              this.me = rs.data;
              this.$refs.value.value = '';
            }
          }
        })
      }
    }
  }
</script>

<style>
  .c_info{
    text-align: left;
  }
</style>
