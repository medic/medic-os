"use strict";
var inherits = require('inh');


var ErrorBase = function () {
	Error.call(this);
	this.captureStackTrace();
};
inherits(ErrorBase, Error);

ErrorBase.prototype.name = 'ErrorBase';

ErrorBase.prototype.captureStackTrace = function () {
	if (Error.captureStackTrace) {
		Error.captureStackTrace(this, this.constructor);
	}
	else {
		var stackKeeper = new Error();
		var self = this;
		stackKeeper.toString = function () { return self.toString(); };
		var getStackTrace = function () {
			return stackKeeper.stack;
		};

		if (Object.defineProperties) {
			Object.defineProperties({
				stack: getStackTrace
			});
		}
		else {
			this.getStackTrace = getStackTrace;
		}
	}
};

ErrorBase.prototype.toString = function () {
	var result = this.name;
	var message = this.getMessage();
	if (message) {
		result = [result, message].join(': ');
	}
	return result;
};

ErrorBase.prototype.getMessage = function () {
	return null;
};

ErrorBase.prototype.getStackTrace = function () {
	return this.stack;
};

if (Object.defineProperties) {
	Object.defineProperties(ErrorBase.prototype, {
		message: {
			get: function () {
				return this.getMessage();
			}
		}
	});
}


module.exports = ErrorBase;
