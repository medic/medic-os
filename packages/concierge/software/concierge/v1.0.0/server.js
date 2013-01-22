
var child = require('child_process'),
    flash = require('connect-flash'),
    request = require('request'),
    express = require('express'),
    crypto = require('crypto'),
    async = require('async'),
    fs = require('fs'),
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

  set_password(req, password, confirmation, function (_err) {

    if (_err) {
      return res.redirect('/setup');
    }

    if (key.length <= 0) {
      req.flash('success', 'Password successfully set');
      return res.redirect('/setup');
    }

    add_openssh_public_key(req, key, function (_was_added) {

      if (_was_added) {
        req.flash('key', null);
        req.flash('success', 'Password and public key successfully set');
      }

      return res.redirect('/setup');
    });

  });

});

/**
 * fatal:
 */
var fatal = function (_message, _exception) {

  process.stderr.write('fatal: ' + _message + '\n');
  process.exit(1);
};

/**
 * trim:
 */
var trim = function (_string) {

  return (
    (_string || '').toString()
      .replace(/^\s*/, '').replace(/\s*$/, '')
  );
};

/**
 * request_error:
 *  Add `_message` to `_req.flash`, invoke `_cb` with a new
 *  error object, and then return whatever that callback returned.
 */

var request_error = function (_message, _req, _callback) {

  _req.flash('error', _message);

  return _callback(
    new Error(_message)
  );
};


/**
 * check_response:
 *  Check both `_err` and `_resp.statusCode`, then fill
 *  `_req.flash` if necessary and return a single `_err` object.
 */

var check_response = function (_err, _resp, _req, _text, _cb) {

  if (_err) {
    return _cb(_err);
  }

  if (_resp.statusCode !== 200) {
    return request_error(
      _text + ' failed: ' + 'problem with database server',
        _req, _cb
    );
  }

  return _cb();
};


/**
 * add_openssh_public_key:
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
      return request_error(
        'Failed to add public key(s): internal error',
          _req, _callback
      );
    }

    return _callback();
  });

};


/**
 * save_system_password:
 */
var save_system_password = function (_req, _password, _callback) {

  var path = '/srv/storage/concierge/passwd/system';

  fs.open(path, 'w', 0640, function (_err, _fd) {

    if (_err) {
      _req.flash('error', "Internal error: file open failed");
      return _callback(_err);
    }

    var buffer = _password + '\n';
    var len = buffer.length;

    fs.write(_fd, buffer, 0, len, function (_err, _len, _buf) {

      if (_err) {
        _req.flash('error', "Internal error: file write failed");
        return _callback(_err);
      }

      fs.fsync(_fd, function (_err) {
          return _callback(_err);
      });
    });
  });
}


/**
 * set_unix_password:
 */
var set_unix_password = function (_req, _password, _confirm, _callback) {

  /* Set UNIX system password:
      This can be used to log in via OpenSSH. */

  var passwd = child.spawn(
    'sudo', [ 'passwd', 'tc' ], { stdio: 'pipe' }
  );

  passwd.stdin.write(_password + '\n' + _confirm + '\n');
   
  passwd.on('exit', function (_code, _signal) {

    if (_code != 0) {
      return request_error(
        'Password change failed: an internal error occurred',
          _req, _callback
      );
    }

    return _callback();
  });
};

/**
 * set_couchdb_password:
 */
var set_couchdb_password = function (_req, _password, _confirm, _callback) {

  /* Set CouchDB administrative password:
      This takes the database out of 'admin party' mode. */

  var proto = 'http://';
  var server = 'localhost:5984';
  var admins_uri = server + '/_config/admins'

  var put = {
    body: JSON.stringify(_password),
    headers: { 'Content-type': 'application/json' },
    uri: proto + admins_uri + '/admin'
  };

  /* Start talking to CouchDB:
      This process involves multiple requests; async used for clarity. */

  async.waterfall([

    /* Step 1: Primary password change */
    function (_cb) {
      request.put(put, function (_err, _resp, _body) {
	return check_response(
	  _err, _resp, _req, 'Primary password change', _cb
	);
      });
    },

    /* Step 2: Secondary password generation */
    function (_cb) {
      crypto.randomBytes(128, function (_err, _data) {
	if (_err) {
	  return _cb(_err);
	}
	return _cb(null, _data.toString('hex'));
      });
    },

    /* Step 3: Secondary password change */
    function (_system_password, _cb) {

      put.auth = 'admin:' + _password;
      put.uri = proto + admins_uri + '/service';
      put.body = JSON.stringify(_system_password);

      request.put(put, function (_err, _resp, _body) {
	return check_response(
	  _err, _resp, _req, 'System account creation', function (_e) {
	    return _cb(_e, _system_password);
	  }
	);
      });
    },

    /* Step 4: Filesystem storage of secondary password */
    function (_system_password, _cb) {

      /* Store secondary administrative password in filesystem:
	  This is used by local services needing to connect to CouchDB. */

      save_system_password(_req, _system_password, function (_err) {
	return _cb(_err);
      });
    }

  ], function (_err) {
    return _callback(_err);
  });
};      

/**
 * set_password:
 */
var set_password = function (_req, _password, _confirm, _callback) {

  /* Start of password change:
      Validate the supplied password and confirmation. */

  if ((_password || '').length < 8) {
    return request_error(
      'Password must be at least eight characters',
        _req, _callback
    );
  }

  if (_password !== _confirm) {
    return request_error(
      'Passwords do not match; please try again',
        _req, _callback
    );
  }

  /* Password databases are modified here */
  set_unix_password(_req, _password, _confirm, function (_err) {

    if (_err) {
      return _callback(_err);
    }

    set_couchdb_password(_req, _password, _confirm, _callback);
  });
};

/**
 * main:
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

