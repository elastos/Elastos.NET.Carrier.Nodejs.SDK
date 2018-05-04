<template>
  <el-dialog custom-class="c_log" title="System log" @close="close_callback()" append-to-body :visible.sync="$attrs.show" width="90vw">
    <ul class="d_ul">
      <li class="d_li" v-for="item in list">
        [{{formatDate(item.date)}}] - {{item.data}}
      </li>
    </ul>

  </el-dialog>
</template>
<script>
  import moment from 'moment';
  export default {
    methods : {
      close_callback(){
        this.$emit('change', false);
      },
      formatDate(date){
        return moment(date).format('YYYY-MM-DD hh:mm:ss');
      }
    },
    computed : {
      list(){
        return this.$store.state.log.list;
      }
    }
  }
</script>
<style lang="scss">
  .c_log{
    background: #eee;

    .d_ul{
      border-top: 1px solid #0074D9;
      padding: 10px 0;
      list-style: none;
      overflow: auto;
      max-height: 500px;

      .d_li{
        color: #0074D9;
        font-size: 1rem;
      }
    }
  }
</style>
