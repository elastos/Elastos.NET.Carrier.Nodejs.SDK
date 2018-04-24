<template>
  <li class="c_message_item">
    <p class="d_p1">
      <span class="d_name">{{name}}</span>
      <span class="d_date">{{date()}}</span>
    </p>
    <p class="d_msg">
      {{item.msg}}
    </p>
  </li>

</template>
<script>
  import moment from 'moment';
  export default {
    props : {
      item : {
        required : true
      }
    },
    computed : {
      name(){
        const f = this.item.from;
        return f.name || f.userId;
      }
    },
    methods : {
      date(){
        const date = this.item.date;
        return moment(date).format('YYYY-MM-DD hh:mm:ss');
      }
    }
  }
</script>
<style lang="scss">
  .c_message_item{
    text-align: left;
    padding: 18px 0;
    border-top: 1px solid rgb(70, 74, 80);

    &:first-child{
      border-top: none;
    }

    .d_name{
      color: #ffffff;
      font-size: 1.2rem;
      font-weight: 700;
    }
    .d_date{
      color: rgb(86, 88, 92);
      font-size: 1rem;
      margin-left: 5px;
    }
    .d_msg{
      margin-top: 8px;
      color: rgb(195, 196, 197);
      font-size: 1.2rem;
    }
  }
</style>
