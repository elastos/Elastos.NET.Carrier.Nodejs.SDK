import request from 'request';
import config from '../config';
import _ from 'lodash';

const F = {
  go(opts){
    opts = _.extend({
      method : 'get',
      path : '',
      error : ()=>{},
      success : ()=>{}
    }, opts);

    request({
      url : config.SERVER_URL+opts.path,
      method : opts.method
    }, (err, res, body)=>{
      console.log(err, res, body)
      if(err){
        opts.error(err);
      }
      else{
        opts.success(JSON.parse(body));
      }
    })
  }
};

export default F;
