
var child = require('child_process'),
    flash = require('connect-flash');
    express = require('express'),
    app = express();

/**
 */
app.configure(function () {

  app.use(flash());
  app.use(express.cookieParser());
  app.use(express.bodyParser());

  app.set('views', __dirname + '/views');

  app.use(express.session({
    secret: '2f6f99e7102059d7acb40bbe4fa8cf547ea18f96'
  }));
});


/**
 */
app.get('/', function (req, res) {
  res.redirect('/setup');
});


/**
 */
app.get('/setup', function (req, res) {
  res.render('setup/index.hbs', {});
});


/**
 */
app.post('/setup/password', function (req, res) {

  var value = req.param('value');
  var confirmation = req.param('confirm');

  if (value !== confirmation) {
    req.flash('error', 'Passwords do not match; please try again');
    return res.redirect('/setup');
  }

  var passwd = child.spawn(
    'sudo', [ 'passwd', 'root' ], { stdio: 'pipe' }
  );

  passwd.stdin.write(
    value + '\n' + confirmation + '\n'
  );
  
  passwd.on('exit', function (_code, _signal) {

    if (_code != 0) {
      req.flash('error', 'Password change was unsuccessful');
    }
    
    return res.redirect('/setup');
  });
});


/**
 */
var fatal = function (_message, _exception) {

  process.stderr.write('fatal: ' + _message + '\n');
  process.exit(1);
}


/**
 */
var main = function (_argv) {

  try {
    app.listen(80);
    process.setuid('concierge');
  } catch (e) {
    fatal("Couldn't bind socket for listening on port 80", e);
  }
 
}

main(process.argv);

