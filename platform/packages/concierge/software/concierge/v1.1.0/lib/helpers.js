
var hbs = require('hbs');

/**
 * register:
 *   Register all of the Handlebars helpers available.
 */
exports.register = function () {

  hbs.registerHelper('cmp', function (_l, _op, _r, _options) {

    if (arguments.length < 3) {
      throw new Error('The `cmp` helper requires exactly three parameters');
    }

    var operators = {
      '==':       function (l,r) { return (l == r); },
      '!=':       function (l,r) { return (l != r); },
      '===':      function (l,r) { return (l === r); },
      '<':        function (l,r) { return (l < r); },
      '<=':       function (l,r) { return (l <= r); },
      '>':        function (l,r) { return (l > r); },
      '>=':       function (l,r) { return (l >= r); },
      'typeof':   function (l,r) { return ((typeof l) == r); }
    };

    if (!operators[_op]) {
      throw new Error('Unrecognized operator `' + _op + '`');
    }

    var rv = operators[_op](_l, _r);

    if (rv) {
      return _options.fn(this);
    } else {
      return _options.inverse(this);
    }
  });
};

