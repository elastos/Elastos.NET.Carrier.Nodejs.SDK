
export default class {
  constructor(type, key, name){
    this.type = type;
    this.name = name;
    this.key = key;

    this.pos = this.buildPath();
    console.log(this.pos);
  }

  buildPath(){
    const pwd = nw.process.cwd();
    return pwd + `/.${this.type}_storage/${this.key}/${this.name}.json`;
  }

  save(data){

  }

  set(query, data){}

};
