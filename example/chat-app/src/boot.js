import Store from './store';
import storage from './storage';

const initForNW = (store)=>{
  const app = nw.Window.get();

  const ac = storage.create('Account', 'abc');
  ac.save({a:'aaa'})

  app.on('close', ()=>{
    // TODO close stuff


    ac.save({a:'bbb'})
    app.close(true);
  });
};

export default ()=>{
  const store = Store.getStore();

  if(nw){
    // only available under nw env
    initForNW(store);
  }

};


