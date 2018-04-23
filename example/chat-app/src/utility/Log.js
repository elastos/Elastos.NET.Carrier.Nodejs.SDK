export default class{
  constructor(key, level=1){
    this.key = key;
    this.level = level;
  }

  debug(...args){
    console.log(`[${this.key}]`, ...args);
  }

  error(...args){
    console.error(`[${this.key}]`, ...args);
  }
}
