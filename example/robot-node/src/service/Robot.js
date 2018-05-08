import request from 'request';
import _ from 'lodash';

class Robot {
    constructor(){
        this.config = {
            url : 'http://openapi.tuling123.com/openapi/api/v2'
        };
    }

    request(text, opts={}){

        request({
            url : this.config.url,
            method : 'post',
            body : JSON.stringify({
                reqType : 0,
                perception : {
                    inputText : {
                        text : text
                    }
                },
                userInfo : {
                    apiKey : process.env.ROBOT_API_KEY,
                    userId : opts.userId.substr(0, 32)
                }
            })
        }, (err, res, body)=>{
            console.log(body);
            const d = JSON.parse(body);
            if(d.results && d.results[0].values.text){
                opts.callback(d.results[0].values.text);
            }
            else{
                opts.callback(body);
            }
        });
    }
};

Robot.instance = null;
Robot.create = ()=>{
    if(!Robot.instance){
        Robot.instance = new Robot();
    }

    return Robot.instance;
};

export default Robot;