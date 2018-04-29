<template>
  <div class="cc-box c_friend_info_card">
    <div class="d_list">
      <p class="d_p">
        <span class="d_s1">Name</span>
        <span class="d_s2">{{item.name}}</span>
      </p>
      <p class="d_p">
        <span class="d_s1">UserId</span>
        <span class="d_s2">{{item.userId}}</span>
      </p>
      <p class="d_p">
        <span class="d_s1">Email</span>
        <span class="d_s2">{{item.email}}</span>
      </p>
      <p class="d_p">
        <span class="d_s1">Phone</span>
        <span class="d_s2">{{item.phone}}</span>
      </p>
      <p class="d_p">
        <span class="d_s1">Gender</span>
        <span class="d_s2">{{item.gender}}</span>
      </p>

      <p class="d_p">
        <span class="d_s1">
          Label
        </span>
        <!--<span class="d_s2">{{item.label}}</span>-->
        <span class="d_s2">
          {{item.label}}
          <i @click="showChangeLabelDialog()" class="cc-icon el-icon-edit"></i>
        </span>
      </p>
    </div>
  </div>
</template>
<script>
  export default {
    data(){
      return {
      };
    },
    props : {
      item : {
        required : true
      }
    },

    methods : {
      showChangeLabelDialog(){
        this.$prompt('Please input label name', '', {
          confirmButtonText: 'Confirm',
          cancelButtonText: 'Cancel'
        }).then(({value}) => {

          this.$root.getCarrier().execute('setFriendLabel', this.item.userId, value);

          this.item.label = value;
          this.$store.commit('friend.info.update', this.item);
        }).catch((e) => {
          console.error(e);
        });
      }
    }
  }
</script>
<style lang="scss">
  .c_friend_info_card{
    .d_list{
      padding: 0 40px;
      .d_p{
        display: flex;
        justify-content: space-between;
        padding: 8px 0 2px;
        border-bottom: 1px dashed #9c9c9c;
        position: relative;

        .cc-icon{
          font-size: 1.2rem;
          position: absolute;
          top: 14px;
          right: -24px;
        }

        .d_s1{
          font-size: 1rem;
          font-weight: 700;
          color: #eee;
        }
        .d_s2{
          color: #c9c9c9;
        }
      }
    }
  }
</style>
