<template>
  <el-container class="cc-box c_home">
    <el-aside class="d_left_one" width="70px">
      <Left_One />
    </el-aside>
    <el-aside class="d_left_two" width="240px">
      <Left_Two />
    </el-aside>

    <el-container>
      <el-header style="padding:0;" height="50px">
        <Main_Header />
      </el-header>
      <el-main class="cc-box" style="padding: 0;">

        <el-button @click.native="createSession()">create session</el-button>
        <el-button @click.native="sendRequest()">send session request</el-button>
        <input type="file" ref="upload" @change="upload()" />

        <br />
        <el-button v-if="file" @click.native="sendTestStream()">Send aaa</el-button>
        <el-button v-if="file" @click.native="sendFileStream()">Send file</el-button>
        <hr />

        <el-button @click.native="receiveRequest()">receive request</el-button>

        <hr />
        <el-button @click.native="openCamera()">Open camera</el-button>
        <hr />
        <video :src="video_url" ref="video_obj" autoplay id="" controls="controls">

        </video>
        <hr/>

        <div id="abjsd"></div>

      </el-main>
    </el-container>

  </el-container>

</template>

<script>
  import Left_Two from '../components/Home/Left_Two';
  import Left_One from '../components/Home/Left_One';
  import Main_Header from '../components/Home/Main_Header';
  import Main_Friend_List from '../components/Home/Main_Friend_List';
  import Message_List from '../components/Home/Message_List';

  import Loading from '../components/common/Loading';
  import CarrierModel from '../service/CarrierModel';

  import {File, Media, OutMediaStream, InputMediaStream} from '@/utility';
  import _ from 'lodash';
  // import ChimeePlayer from 'chimee-player';

  export default {
    data(){
      return {
        stream : null,
        file : null,
        video_url : null
      }
    },
    components : {
      Left_Two,
      Left_One,
      Main_Header,
      Main_Friend_List,


      Loading
    },

    computed : {
      current(){
        return this.$store.state.friend.currentSelect;
      },
      carrier_session(){
        return this.$store.state.stream.connect;
      }
    },

    created(){

    },

    mounted(){
      // const cp = nw.require('chimee-player');
      // console.log(ChimeePlayer)
      // new ChimeePlayer({
      //   wrapper: '#abjsd',  // video dom容器
      //   src: 'http://chimee.org/vod/1.mp4',
      //   autoplay: true,
      //   controls: true
      // });
      // var wjs = nw.require("wcjs-player");
      // var player = new wjs("#abjsd").addPlayer({
      //   autoplay: true,
      //   wcjs: nw.require('wcjs-prebuilt')
      // });
      // player.addPlaylist("http://chimee.org/vod/1.mp4");
      // this.video_url = 'file:///Users/jacky.li/Desktop/ab.webm'
    },
    methods: {
      createSession(){
        if(!this.current.online){
          this.$root.errorMessage('This friend is not online, couldn\'t send message');
          return false;
        }

        const carrier = this.$root.getCarrier();
        carrier.execute('session_newSession', this.current.userId);
        this.stream = carrier.execute('session_addStream', ['plain'], 'TEXT');


      },
      sendRequest(){
        const carrier = this.$root.getCarrier();
        carrier.execute('session_request');
      },
      upload() {
        const obj = this.$refs.upload;
        const file = obj.files[0];

        this.file = file;
        console.log(file);
      },
      sendTestStream(){
        const buffer = new Buffer('aaa');

        try {
          this.$root.getCarrier().execute('stream_write', this.stream.id, buffer);
        } catch (e) {
          console.error(e);
        }
      },
      sendFileStream(){
        const f = new File(this.file);
        const list = f.buildFileBuffer();

        try {
          _.each(list, (buffer)=>{
            // console.log(buffer);
            this.$root.getCarrier().execute('stream_write', this.stream.id, buffer);

          });
        } catch (e) {
          console.error(e);
        }
      },

      receiveRequest(){
        this.createSession();
        _.delay(()=>{
          this.$root.getCarrier().execute('session_replyRequest', true);
        }, 5000);

      },

      async openCamera(){
        const m = new Media();


        const ms = new MediaSource();
        const q = [];


        ms.addEventListener('sourceopen', async ()=>{
          // const sb = ms.addSourceBuffer('video/webm; codecs="vp8, vorbis"');
          const sb = ms.addSourceBuffer('video/mp4; codecs="avc1.58A01E, mp4a.40.2"');
          const out_stream = new OutMediaStream({

          }, (chunk, done)=>{

            const list = File.cutBuffer(chunk);
            try {
              _.each(list, (buffer)=>{
                // console.log(buffer);
                // this.$root.getCarrier().execute('stream_write', this.stream.id, buffer);
                // q.push(buffer);

                // q.push(buffer);
                // loop();
              });
              q.push(Buffer.from(chunk, 'binary'));
              loop();
              done();
            } catch (e) {
              console.error(e);
            }


          });

          const res = await m.openMedia(out_stream);

          sb.addEventListener('update', (e)=>{ // Note: Have tried 'updateend'
            // console.log(123, e);

          });

          sb.onerror = (e, err)=>{
            // console.error(1, e, err);
          }

          sb.addEventListener('updateend', ()=>{
            // console.log(111);
            // ms.endOfStream();
            this.$refs.video_obj.play();
          }, false);

          const loop = ()=>{
            if(!sb.updating){
              const buf = q.shift();
              if(buf){
                // console.log(buf);
                sb.appendBuffer(buf);

              }

            }


          }

//           loop();


        });
        this.video_url = nw.global.URL.createObjectURL(ms);
        console.log(nw.global.URL.createObjectURL(ms))
        // const input_stream = new InputMediaStream({}, ds);
        //
        //
        // // const obj = this.$refs.video_obj;
        // // obj.onloadedmetadata = (e)=>{
        // //   // console.log(x, stream, stream.getVideoTracks())
        // // };
        // input_stream.on('readable', () => {
        //   console.log('缓存剩余数据大小: ', input_stream._readableState.length + ' byte')
        //   console.log('------------------------------------')
        //   input_stream.read(1);
        // })
        //
        // const fs = nw.require('fs');
        // const ss = fs.createReadStream('/Users/jacky.li/Desktop/test.mp4');



        // const res = await m.openMedia(true, true);
        // this.video_url = nw.global.URL.createObjectURL(res);
        // this.$refs.video_obj.onloadedmetadata = ()=>{
        //   this.$refs.video_obj.play();
        // };

      }
    }
  };
</script>

<style lang="scss">
  .c_home{
    width: 100%;
    position: relative;

    .d_left_one{
      background: #42b983;
    }
    .d_left_two{
      position: relative;
    }

    .d_msg_area{
      /*padding-right: 240px;*/
      background: #ccc;
      height: 100%;
    }
    .d_friend_area{
      width: 240px;
      position: absolute;
      right: 0;
      top: 0;
      height: 100%;
    }
  }
</style>
