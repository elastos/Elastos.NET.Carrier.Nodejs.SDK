import _ from 'lodash';
import store from './store';

const F = {
  ws : null,
  param : {},
  callbacks : {},
  create(url){
    F.ws = new WebSocket(url);
    F.param.url = url;
    F.init();
    // console.log(F.ws);
  },

  init(){
    F.ws.addEventListener('open', (event)=>{

    });

// Listen for messages
    F.ws.addEventListener('message', (event)=>{
      try{
        const data = JSON.parse(event.data);
        console.log(111, data);
        if(data.code > 0){
          this.buildStoreData(data.type, data);
        }
        else{
          this.buildStoreData(data.type, data);
        }
      }catch(e){
        console.error(e);
        console.error('receive invalid message : '+event.data);
      }

    });

    F.ws.addEventListener('close', (...args)=>{
      // console.log(...args);
      console.log('connection closed');
      // _.delay(()=>{
      //   F.create(F.param.url);
      // }, 2000);
    });

    F.ws.addEventListener('error', (e)=>{
      console.error(e);
    })

    console.log('socket init success');
  },

  send(type, data, callback){
    if(F.ws.readyState < 1){
      _.delay(()=>{
        this.send(type, data, callback);
      }, 200);
    }
    else{
      F.ws.send(JSON.stringify({
        type,
        data
      }));

      if(callback){
        const key = `${type}/${data.method}`;
        if(!F.callbacks[key]){
          F.callbacks[key] = [];
        }

        F.callbacks[key].push(callback);
      }

    }

  },

  buildStoreData(type, {data, msg, code, error}){
    store.getStore().dispatch('socket_data', {
      type,
      data,
      msg,
      code,
      error
    });

    //call callback
    if(F.callbacks[type]){
      _.each(F.callbacks[type], (fn)=>{
        fn({data, msg, code, error})
      });

      F.callbacks[type] = [];
    }

  },



  handler(data){
    console.log(1111, data);
  }
};

export default F;
