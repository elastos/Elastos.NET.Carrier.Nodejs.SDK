import _ from 'lodash';

const pool = {};
const F = {
  register(type, callback){
    if(!pool[type]){
      pool[type] = [];
    }

    pool[type].push(callback);
  },

  send(type, ...args){
    const list = pool[type] || [];
    _.each(list, (fn)=>{
      fn(...args);
    });
    // while(list.length > 0){
    //   const fn = list.unshift();
    //
    //   fn(...args);
    // }
  },

  remove(type){
    pool[type] = [];
  }
};


export default F;
