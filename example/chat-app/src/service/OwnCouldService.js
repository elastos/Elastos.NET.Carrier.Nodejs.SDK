const createClient = nw.require('webdav-fs');

const C = {
  storage_key : 'owncloud_data'
};

export default class {
  constructor(){
    this.client = null;

    this.remote = {
      address : '',
      username : '',
      password : '',
      pair : false
    };
    // this.initData();
  }
  initData(){
    const d = LocalStorage.getItem(C.storage_key);
    if(d){
      const json = JSON.parse(d);
      this.remote = json;
    }
  }
  setData(opts){

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


  async saveToLocal(){

  }
  async getFromLocal(){

  }
};
