
var child = require('child_process'),
    flash = require('connect-flash'),
    request = require('request'),
    express = require('express'),
    app = express();

/**
 */
app.configure(function () {

  app.use(flash());
  app.use(express.cookieParser());
  app.use(express.bodyParser());

  app.set('views', __dirname + '/views');
  app.use('/static', express.static(__dirname + '/static'));

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

  res.render('setup/index.hbs', {
    title: (
      'Set Administrative Password: ' +
      'Medic Mobile Virtual Server Configuration'
    ),
    data: {
      key: req.flash('key')
    },
    messages: {
      error: req.flash('error'),
      success: req.flash('success')
    }
  });
});


/*
 */
app.post('/setup/password', function (req, res) {

  var key = trim(req.param('key'));
  var password = req.param('password');
  var confirmation = req.param('confirmation');

  if (key.length > 0) {
    req.flash('key', key);
  }

  set_system_password(req, password, confirmation, function (_was_set) {

    if (_was_set && key.length > 0) {

      add_openssh_public_key(req, key, function (_was_added) {

        if (_was_added) {
          req.flash('key', null);
          req.flash('success', 'Password and public key successfully set');
	}

        return res.redirect('/setup');
      });

    } else {

      if (_was_set) {
        req.flash('success', 'Password successfully set');
      }

      return res.redirect('/setup');
    }
  });
});

/**
 */
var fatal = function (_message, _exception) {

  process.stderr.write('fatal: ' + _message + '\n');
  process.exit(1);
};

/**
 */
var trim = function (_string) {

  return (
    (_string || '').toString()
      .replace(/^\s*/, '').replace(/\s*$/, '')
  );
};


/**
 */
var add_openssh_public_key = function (_req, _key, _callback) {

  /* Add data to OpenSSH's authorized_keys file:
       This feature requires the `ssh-addkey` script and sudo privileges. */

  var passwd = child.spawn(
    'sudo', [ '-u', 'tc', '/srv/scripts/concierge/private/ssh-addkey' ],
      { stdio: 'pipe' }
  );

  passwd.stdin.write(_key);
  passwd.stdin.end();

  passwd.on('exit', function (_code, _signal) {

    if (_code != 0) {
      _req.flash('error', 'Failed to add public key(s): internal error');
      return _callback(false);
    }

    return _callback(true);
  });

};

/**
 */
var set_system_password = function (_req, _password, _confirmation, _callback) {

  if ((_password || '').length < 8) {
    _req.flash('error', 'Password must be at least eight characters');
    return _callback(false);
  }

  if (_password !== _confirmation) {
    _req.flash('error', 'Passwords do not match; please try again');
    return _callback(false);
  }

  /* Set system password:
       This can be used to log in via OpenSSH. */

  var passwd = child.spawn(
    'sudo', [ 'passwd', 'tc' ], { stdio: 'pipe' }
  );

  passwd.stdin.write(
    _password + '\n' + _confirmation + '\n'
  );
  
  passwd.on('exit', function (_code, _signal) {

    if (_code != 0) {
      _req.flash(
        'error', 'Password change failed: an internal error occurred'
      );
      return _callback(false);
    }

    /* Set CouchDB administrative password:
         This takes the database out of 'admin party' mode. */

    var put = {
      body: JSON.stringify(_password),
      headers: { 'Content-type': 'application/json' },
      uri: 'http://localhost:5984/_config/admins/admin'
    };

    request.put(put, function (err, resp, body) {

      if (resp.statusCode !== 200) {
        _req.flash(
          'error', "Password change failed: problem with database server"
	);
        return _callback(false, err);
      }
        
      return _callback(true);
    });
  });
};

/**
 */
var main = function (_argv) {

  if (process.getuid() !== 0) {
    fatal("This application must be started in privileged mode; use sudo");
  }

  try {
    app.listen(80);
  } catch (e) {
    fatal("Couldn't bind socket for listening on port 80", e);
  }
 
  try {
    process.setuid('concierge');
  } catch (e) {
    fatal("Couldn't drop privileges; ensure user 'concierge' exists", e);
  }
};

main(process.argv);

