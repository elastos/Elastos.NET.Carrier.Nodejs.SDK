import Log from './Log';

export {
  Log
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


