
var lazy = require('lazy');
var factory = require('medic-transport');


/**
 * @name read_stdin_password:
 */
var read_stdin_password = function (_callback) {

  new lazy(process.stdin).lines.head(function (_line) {
    return _callback(null, _line.toString());
  });
};


/**
 * @name main:
 */
var main = function (_argv) {

  var user = _argv[2];
  var password = _argv[3];

  if (user == '-h' || user == '--help') {
    process.stderr.write(
      'Usage: ' + _argv[1] + ' [ user [ password ] ]\n'
    );
    process.exit(1);
  }

  if (password == '-') {
    return read_stdin_password(function (_err, _password) {
      return run(user, _password);
    });
  }

  return run(user, password);
};


/**
 * run:
 */
var run = function (_user, _password) {

  var transport = factory.create({
    debug: true,
    name: 'gammu-json'
  }, {
    debug: true,
    name: 'medic-mobile',
    user: _user, password: _password,
    url: 'http://localhost/medic/_design/medic/_rewrite'
  });

  return transport.start();
}

main(process.argv);

