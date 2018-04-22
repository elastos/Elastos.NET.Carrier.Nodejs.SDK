import request from 'request';
import config from '../config';
import _ from 'lodash';

const F = {
  go(opts){
    opts = _.extend({
      method : 'get',
      path : '',
      ela : '',
      error : ()=>{},
      success : ()=>{}
    }, opts);

    if(!opts.ela){
      throw new Error('invalid ela id');
    }

    // request({
    //   url : config.SERVER_URL+opts.path,
    //   method : opts.method,
    //   headers : {
    //     ela_id : opts.ela
    //   }
    // }, (err, res, body)=>{
    //   if(err){
    //     opts.error(err);
    //   }
    //   else{
    //     opts.success(JSON.parse(body));
    //   }
    // })
  }
};

export default F;
