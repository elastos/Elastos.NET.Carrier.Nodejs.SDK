import Store from './store';
import storage from './storage';
import {Message, nwBuild, Notification, cache} from './utility';

var fs = require('fs');

const initForNW = (store)=>{
  const win = nw.Window.get();

  // const ac = storage.create('Account', 'abc');
  // ac.save({a:'aaa'})
  //
  // app.on('close', ()=>{
  //   // TODO close stuff
  //
  //
  //   ac.save({a:'bbb'})
  //   app.close(true);
  // });

  // bind nw event
  Message.register('nw_app_badge', (label=0)=>{
    if(label > 100) label = 100;
    if(label === 0) label = '';
    win.setBadgeLabel(label.toString());
  });


  win.on('focus', ()=>{
    cache('nw_focus', true);
  });
  win.on('blur', ()=>{
    cache('nw_focus', false);
  })
};





export default ()=>{
  const store = Store.getStore();

  nwBuild(()=>{
    initForNW(store);
  }).exec();

  Notification.init();

};


