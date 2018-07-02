import _ from 'lodash';

const blobStream = nw.require('blob-stream');
const createClient = nw.require('webdav-fs');

const C = {
  storage_key : 'owncloud_data',
  stream_type : ['mp4', 'pdf'],
  image_type : ['jpeg', 'png', 'jpg']
};

class Service {
  constructor(carrier){
    this.carrier = carrier;
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

  _promise(fn, args, endArgs=[]){
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
    try{
      const rs = await this._promise('readdir', [path], ['stat']);
      const list = _.map(rs, (item)=>{
        item.type = item.isFile() ? 'file' : 'dir';
        return item;
      });
      return _.sortBy(list, ['type']);
    }catch(e){
      console.error(e);
      throw 'invalid path : '+path;
    }
  }

  async getFile(path){
    let isStream = false;
    let tmp = _.last(path.split('/'));
    if(_.includes(tmp, '.')){
      tmp = _.last(tmp.split('.')).toLowerCase();
      if(_.includes(C.stream_type, tmp) || _.includes(C.image_type, tmp)){
        isStream = true;
      }
    }

    if(isStream){
      const sm = this.client.createReadStream(path);

      return new Promise((resolve)=>{
        sm.pipe(blobStream()).on('finish', function(){
          // const blob = this.toBlob();
          let url;
          let type = 'stream';
          if(tmp === 'pdf'){
            url = this.toBlobURL('application/pdf');
          }
          else if(_.includes(C.image_type, tmp)){
            url = this.toBlobURL('image/'+tmp);
            type = 'image';
          }
          else{
            url = this.toBlobURL();
          }

          resolve({
            path,
            stream : url,
            type,
            ext : tmp
          });
        });
      });

    }
    else{
      const rs = await this._promise('readFile', [path, 'utf8']);
      return {
        path,
        content : rs,
        type : 'text',
        encoding : 'utf8',
        ext : tmp
      };
    }

  }


  async saveToLocal(){

  }
  async getFromLocal(){

  }
};


let _oc = null;
export default {
  create(){
    if(_oc) return _oc;

    _oc = new Service(CarrierService);
    return _oc;
  }
}
