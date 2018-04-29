<template>
  <div class="cc-box c_main_friend_list">
    <ul class="d_ul">
      <li @click="selectFriend(item)" class="d_li" v-bind:class="{select: item.userId===select.userId}" v-for="item in list">
        <span class="d_t1">
          {{item.name || 'NA'}} {{item.label ? `(${item.label})` : ''}}
          <i v-bind:class="{online : item.online}" class="d_st"></i>
        </span>
        <span class="d_t2">{{item.userId}}</span>
        <i @click="showInfo(item, $event)" class="cc-icon el-icon-more"></i>
      </li>
    </ul>

    <el-dialog custom-class="d_dia" :visible.sync="dialogVisible">
      <Friend_Info_Card :item="dialogInfo" />
    </el-dialog>
  </div>
</template>
<script>
  import Friend_Info_Card from './Friend_Info_Card';
  export default {
    components : {
      Friend_Info_Card
    },
    data(){
      return {
        dialogVisible : false,
        dialogInfo : null
      };
    },
    computed : {
      list(){
        return this.$store.state.friend.list;
      },
      select(){
        return this.$store.state.friend.currentSelect;
      }
    },
    methods : {
      selectFriend(item){
        this.$store.commit('friend.current.set', item);
      },
      showInfo(item, e){
        e.stopPropagation();
        this.dialogInfo = item;
        this.dialogVisible = true;
      }
    }
  }
</script>
<style lang="scss">
  .c_main_friend_list{
    background: rgb(47, 49, 54);
    padding-bottom: 60px;

    .d_st{
      display: inline-block;
      width: 10px;
      height: 10px;
      border-radius: 5px;
      background: #2c3e50;
      border: 1px solid #c9c9c9;
      position: relative;
      left: 5px;
      top: 0;

      &.online{
        background: lime;
        border-color: #cdcdcd;
      }
    }

    .d_ul{
      text-align: left;
      list-style: none;
      padding: 0;

      .d_li{
        padding: 15px 12px;
        border-top: 1px solid #c9c9c9;
        cursor: pointer;
        position: relative;

        &:hover, &.select{
          background: rgba(0, 0, 0, 0.8);
        }

        &:first-child{
          border-top: none;
        }

        span{
          color: #eee;
          font-size: 1.4rem;
          display: block;
        }

        .d_t2{
          font-size: 1rem;
          text-overflow: ellipsis;
          overflow: hidden;
        }

        .cc-icon{
          position: absolute;
          right: 12px;
          top: 15px;
          font-size: 1.8rem;
          cursor: pointer;
        }
      }
    }
    .d_dia{
      width:50vw;
      min-width: 400px;
      border: 1px solid #eee;
      background: #2c3e50;

      .el-dialog__header{
        padding: 0;
      }
    }
  }
</style>
