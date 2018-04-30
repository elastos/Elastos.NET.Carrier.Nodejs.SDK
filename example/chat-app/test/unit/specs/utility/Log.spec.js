import Log from '@/utility/Log';

it('[class Log]', ()=>{
  const log = new Log('xyz');

  expect(log.key).toBe('xyz');
  expect(log.level).toBe(1);

  expect(log.debug(111)).toBeUndefined();
  expect(log.error(111)).toBeUndefined();
});
