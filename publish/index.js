var elca = null;

if (process.platform == "linux" && process.arch == "x64") {
	elca = require('./linux/elca');
}
else if(process.platform == "darwin" && process.arch == "x64") {
	elca = require('./mac/elca');
}

module.exports = elca;