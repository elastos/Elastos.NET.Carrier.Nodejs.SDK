import Log from './Log';
import _ from 'lodash';
import Message from './Message';

// nw
const getNW = ()=>{
  return require('nw') || null;
};
export const nwBuild = (callback)=>{
  return {
    exec : ()=>{
      const nw = getNW();
      if(nw){
        callback(nw);
      }
      else{
        console.error('nw only available in nwjs desktop env')
      }
    }
  };
};

export const hash = (str)=>{
  return str;
};

export const presence_class = (online, presence, prefix='')=>{
  let rs = '';
  if(online){
    if(presence === 0){
      rs = 'none';
    }
    else if(presence === 1){
      rs = 'away';
    }
    else if(presence === 2){
      rs = 'busy';
    }
    else{
      throw new Error('invalid presence : '+presence);
    }
  }

  return prefix+rs;
};

const _cache = {};
export const cache = (key, value)=>{
  if(!_.isUndefined(value)){
    _cache[key] = value;
    return value;
  }
  else{
    return _.isUndefined(_cache[key]) ? null : _cache[key];
  }
};


export {
  Log,
  Message
};

