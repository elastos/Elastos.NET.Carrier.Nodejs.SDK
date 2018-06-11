import _ from "lodash";

const stream = nw.require('stream');

export const OutMediaStream = class extends stream.Writable {

  constructor(options, write_fn=_.noop()){
    super(options);

    this.write_fn = write_fn;
  }

  setWriteFunction(func){
    this.write_fn = func;
  }

  _write(chunk, enc, done) {


    this.write_fn(chunk, done);
  }
};

export const InputMediaStream = class extends stream.Readable {
  constructor(options, dataSource){
    super(options);
    this.dataSource = dataSource;
  }
  // 文档提出必须通过_read方法调用push来实现对底层数据的读取
  _read() {
    console.log('阈值规定大小：', arguments['0'] + ' bytes')
    const data = this.dataSource.makeData()
    let result = this.push(data)
    if(data) console.log('添加数据大小：', data.toString().length + ' bytes')
    console.log('已缓存数据大小: ', subReadable._readableState.length + ' bytes')
    console.log('超过阈值限制或数据推送完毕：', !result)
    console.log('====================================');
  }
}
