import {presence_class, hash, cache, nwBuild} from '@/utility';

test('[function hash]', ()=>{
  const str = hash('aaa');
  expect(str).toBe('aaa');
});

test('[function presence_class]', ()=>{
  const s1 = presence_class(false, 0);
  const s2 = presence_class(true, 0);
  const s3 = presence_class(true, 1, 'd_');
  const s4 = presence_class(true, 2, 'd_');

  expect(s1).toBe('');
  expect(s2).toBe('none');
  expect(s3).toBe('d_away');
  expect(s4).toBe('d_busy');


  expect(()=>{
    presence_class(true, 3);
  }).toThrow();
});

test('[function cache]', ()=>{
  cache('key', 'value');
  expect(cache('key')).toBe('value');
  expect(cache('no_exist')).toBe(null);
});

test('[function nwBuild]', ()=>{
  let rs = 1;

  nwBuild(()=>{
    rs = 10;
  }).exec();
  expect(rs).not.toBe(10);
});

test('[function nwBuild]', ()=>{
  let rs = 1;
  // mock nw
  jest.doMock('nw', ()=>{
    return {

    };
  });
  window.nw = {
    env : 'test'
  };

  nwBuild((nw)=>{
    rs = nw.env;
  }).exec();

  expect(rs).toBe('test');
});

