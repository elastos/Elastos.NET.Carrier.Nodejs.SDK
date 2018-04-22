<template>
  <div class="box" v-if="!!current.userId">
    <h2>ME & {{current.userId}}</h2>
    <hr />

    <ul>
      <li v-for="(item) in list">
        {{item.from}} : {{item.msg}}
      </li>
    </ul>

    <!--<hr />-->
    <!--{{JSON.stringify($store.state.friend.message)}}-->
  </div>
</template>

<script>
  export default {
    computed : {
      current(){
        return this.$store.state.friend.currentSelect;
      },
      list(){
        return this.$store.getters.getFriendMessageList();
      }
    }
  }
</script>
