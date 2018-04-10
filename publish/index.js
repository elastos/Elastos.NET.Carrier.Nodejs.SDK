var elca = null;

if (process.platform == "linux" && process.arch == "x64") {
	elca = require('./linux/elac');
}
else if(process.platform == "mac" && process.arch == "x64") {
	elca = require('./mac/elca');
}
else {
	// Load the new built binary for other platforms.
	robot = require('./build/Release/robot');
}

module.exports = elca;