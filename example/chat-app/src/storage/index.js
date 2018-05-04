import Account from './Account';

import _ from 'lodash';

export default {
  create(dbName, key, opts={}){
    if(_.isUndefined(nw)){
      console.error('storage layer only available for nwjs env');

      return {};
    }

    switch(dbName){
      case 'Account':
        return new Account(key);
    }
  }
};
