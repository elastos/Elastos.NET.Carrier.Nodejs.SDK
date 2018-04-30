import CarrierModel from '@/service/CarrierModel';

test('[class User]', ()=>{
  const test_data = {
    name : 'aaa',
    userId : 'bbb'
  };

  const user = CarrierModel.create('User', test_data).getData();
  expect(user.name).toBe('aaa');
  expect(user.userId).toBe('bbb');

});

test('[class Friend]', ()=>{
  const test_data = {
    userInfo : {
      name : 'aaa',
      userId : 'bbb'
    },
    presence : 1,
    status : 0,
    label : 'ccc'
  };

  const f = CarrierModel.create('Friend', test_data).getData();
  expect(f.label).toBe('ccc');
});

test('[index]', ()=>{
  expect(()=>{
    CarrierModel.create('unknown_type', {});
  }).toThrow();
});
