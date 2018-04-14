const _ = require('lodash');

module.exports = function (userInfo){
    const self = this;
    self.data = {
        name : userInfo.name,
        userId : userInfo.userId,
        description : userInfo.description,
        gender : userInfo.gender,
        phone : userInfo.phone,
        email : userInfo.email,
        region : userInfo.region
    };


    _.extend(self, {
        display(){
            const info = self.data;
            console.log("           ID: %s", info.userId);
            console.log("         Name: %s", info.name);
            console.log("  Description: %s", info.description);
            console.log("       Gender: %s", info.gender);
            console.log("        Phone: %s", info.phone);
            console.log("        Email: %s", info.email);
            console.log("       Region: %s", info.region);
        },
        getData(){
            return self.data;
        }
    });

};