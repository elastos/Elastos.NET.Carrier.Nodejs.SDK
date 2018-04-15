<template>
  <div class="c_info">
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
      <select>
        <option value="name">Name</option>
        <option value="email">Email</option>
        <option value="phone">Phone</option>
        <option value="gender">Gender</option>
      </select>
      <input value="" />
      <button>Confirm</button>
    </div>
  </div>

</template>

<script>
  import api from '../utils/request';
  export default {
    data () {

      return {
        me : {},
        address : null
      }
    },
    mounted(){
      api.go({
        path: '/api/me/get',
        success: (rs)=>{
          this.me = rs.data
        }
      });

      api.go({
        path : '/api/me/address',
        success : (rs)=>{
          this.address = rs.data;
        }
      })
    }
  }
</script>

<style>
  .c_info{
    text-align: left;
    padding: 10px 25px;
  }
</style>
