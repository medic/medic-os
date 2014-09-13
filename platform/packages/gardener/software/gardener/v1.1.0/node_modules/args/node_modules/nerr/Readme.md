# nerr

Using custom errors is tricky in JS. Unfortunately, it is not enough to just inherit your class from Error. You need to do really strange things to provide stack trace with your error objects.

**nerr** fixes this problem by providing ErrorBase class. By inheriting it you are getting correct error implementation with stack trace included.

ErrorBase features:

* Inherited from Error, all your error objects will be instances of Error
* Fully Error-compatible, mimics all it's properties, code working with Error will also work with classes inherited from ErrorBase
	* but if `Object.defineProperties` is not available, you will need to use functions instead of `stack` and `message` properties, see below
* `message` is getter, not just string property, you can override getMessage() to construct message you need using any of your error object properties

## How to use

* Inherit your error class from ErrorBase
* Don't forget to call superclass constructor in your constructor
* Override prototype.name by setting it to your class name
* Override getMessage() to provide error message

Note, that ErrorBase hasn't `message` argument that Error has. However, you can provide it in your error class yourself, as well as any other arguments and properties you may need. But be careful with property names. For example, inaccurate overriding of `message` property (which is a getter) can break functionality.

## Usage example

Defining custom error class:

```js
var inherits = require('util').inherits;
var ErrorBase = require('nerr').ErrorBase;

var MyError = function (code) {
	ErrorBase.call(this);
	this.code = code;
};
inherits(MyError, ErrorBase);

MyError.prototype.name = 'MyError';

MyError.prototype.getMessage = function () {
	return 'Error code: ' + this.code;
};
```

Using it:

```js
try {
	throw new MyError(500);
}
catch(err) {
	// prints error name, message and stack trace
	console.log('Stack trace:\n', err.stack);

	console.log();

	// prints error name and message
	console.log('Error:', err.toString());

	console.log();

	console.log('Error name:', err.name);
	console.log('Error message:', err.message);
}
```

## How it works

ErrorBase constructor calls `Error.captureStackTrace(obj, func)` which provides an object instance with a `stack` getter (`func` is used to appropriately truncate captured stack trace, excluding error construction function).

`stack` getter returns concatenation of obj.toString() and a captured stack trace. Obtained value is cached, so sequential getter calls will return the same value even if obj.toString() result is changed.

## Compatibility

`Error.captureStackTrace()` is undocumented feature of V8 (at least), so it's behavior can be changed.

If `captureStackTrace()` is not available, ErrorBase uses `new Error()` to get stack trace. In this case it cannot exclude error construction part of trace.

If `Object.defineProperties` is not available, `message` and `stack` getters will not be exposed, use `getMessage()` and `getStackTrace` functions instead.
