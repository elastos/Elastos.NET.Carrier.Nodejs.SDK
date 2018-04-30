import {presence_class, hash} from '@/utility';

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
