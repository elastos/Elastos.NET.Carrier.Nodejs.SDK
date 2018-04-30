import Base from './Base';

export default class extends Base {
  constructor(key, name){
    super('fs', key, name);

    this.fs = nw.global.require('fs-extra');

    if(!this.fs.existsSync(this.pos)){
      this.fs.ensureFileSync(this.pos);
    }
  }

  save(data){
    let d = ''
    try{
      d = JSON.stringify(data);
    }catch(e){
      d = data.toString();
    }
    return this.fs.writeFileSync(this.pos, d);
  }
};
