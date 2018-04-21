<template>
  <form class="box">
    <input type="text" title="account name" class="ela_text" v-model="name" />
    <button @click="create()" class="ela_button"> Create Account </button>
  </form>
</template>

<script>
  export default {
    data(){
      return {
        name : ''
      }
    },
    methods: {
      create(){
        const url = '?'+encodeURIComponent(this.name);
        window.open(url);
      }
    }
  }
</script>

<style>

</style>
