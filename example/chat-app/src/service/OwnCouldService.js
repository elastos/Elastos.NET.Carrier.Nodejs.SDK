

const createClient = nw.require('webdav-fs');

export default class {
  constructor(){
    this.client = null;
  }
  connect(url, username, password){
    this.client = createClient(url, username, password);
  }

  _promise(fn, args, endArgs){
    return new Promise((resolve, reject)=>{
      this.client[fn](...args, (err, rs)=>{
        if(err){
          reject(err);
        }
        else{
          resolve(rs);
        }
      }, ...endArgs);
    });
  }

  async getFileList(path){
    return this._promise('readdir', [path], ['stat']);
  }
};
