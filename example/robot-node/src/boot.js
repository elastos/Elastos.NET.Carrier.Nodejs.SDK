import Carrier from './service/Carrier';
import Robot from './service/Robot';

export default (app, config)=>{

    // start carrier node
    const carrier = Carrier.create('robot-carrier');

};