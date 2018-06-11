import _ from 'lodash';

const fs = nw.require('fs');
const DATA_START = '__file__start__';
const DATA_END = '__file__end__';
const DIR = nw.process.cwd()+'/.files/';

if(!fs.existsSync(DIR)){
  fs.mkdirSync(DIR);
}

const receive = {
  flag : false,
  file : {},
  buffer : []
};
export default class File {
  constructor(file){
    this.file = file;
  }

  static cutBuffer(chunk, size=2048){
    const buffer = Buffer.from(chunk, 'binary');
    const buf_list = _.chunk(buffer, size);
    return _.map(buf_list, (item)=>{
      return Buffer.from(item, 'binary');
    });
  }

  getCutBufferList(length=2048){
    const fb = fs.readFileSync(this.file.path);
    return File.cutBuffer(fb, length);
  }

  buildFileBuffer(){
    const list = this.getCutBufferList();

    return [
      new Buffer(DATA_START+JSON.stringify({
        size : this.file.size,
        name : this.file.name,
        type : this.file.type
      })),
      ...list,
      new Buffer(DATA_END)
    ];
  }

  static onStreamData(data, callback=_.noop()){
    const d = data.toString();
    if(!receive.flag){
      if((new RegExp('^'+DATA_START)).test(d)){
        const tmp = d.replace(DATA_START, '');
        receive.file = JSON.parse(tmp);
        receive.buffer = [];
        receive.flag = true;

        return callback('file_start', receive.file);
      }

      return callback('text', d);
    }

    if((new RegExp('^'+DATA_END)).test(d)){
      receive.flag = false;
      console.log(receive.buffer);
      const tt = Buffer.concat(receive.buffer, receive.file.size);
      fs.writeFileSync(DIR+receive.file.name, tt);

      return callback('file_end', DIR+receive.file.name);
    }

    // put into buffer cache
    receive.buffer.push(Buffer.from(data, 'binary'));
  }



};
