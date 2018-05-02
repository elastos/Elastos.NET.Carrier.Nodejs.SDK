import Message from '@/utility/Message';

test('[object Message]', ()=>{
  let rs = 0;
  const fn1 = ()=>{
    rs=1;
  };
  const fn2 = (n)=>{
    rs=n;
  };

  Message.register('test', fn1);
  Message.send('test');
  expect(rs).toBe(1);

  Message.remove('test');
  Message.register('test', fn2);
  Message.send('test', 100);
  Message.send('no_exist', 100);
  expect(rs).toBe(100);
});
