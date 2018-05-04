const F = {
  init(){
    window.Notification.requestPermission().then((per)=>{
      console.log('notification permission => ' + per);
    });
  },

  show(body, opts={}){
    const obj = new window.Notification('Notification', {
      body : body
    });

    if(opts.onclick){
      obj.onclick = ()=>{
        opts.onclick(obj);
        obj.close();
      };
    }
  }
};

export default F;
