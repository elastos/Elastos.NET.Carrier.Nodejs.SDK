<template>
  <el-container class="cc-box c_home">
    <el-aside class="d_left_one" width="70px">
      <Left_One />
    </el-aside>
    <el-container>
      <ul class="d_ul" ref="ul">
        <li v-for="(item, index) in list" :key="index">
          {{item.name}} - {{item.size}} - {{item.type}}
        </li>
      </ul>
    </el-container>
  </el-container>
</template>
<script>
  import Left_One from '../components/Home/Left_One';
  import _ from 'lodash';

  import OwnCloudService from '../service/OwnCouldService';
  export default {
    data(){
      return {
        list: []
      }
    },
    components: {
      Left_One
    },

    async mounted(){
      const oc = new OwnCloudService();
      oc.connect('http://localhost:8000/remote.php/dav/files/admin', 'admin', '111111');
      const list = await oc.getFileList('/aaa/');

      this.list = _.map(list, (item)=>{
        item.type = item.isFile() ? 'file' : 'dir';

        return item;
      });
      console.log(this.list);
    }
  }
</script>
