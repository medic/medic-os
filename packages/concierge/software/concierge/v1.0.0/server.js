
var child = require('child_process'),
    flash = require('connect-flash'),
    request = require('request'),
    express = require('express'),
    crypto = require('crypto'),
    async = require('async'),
    fs = require('fs'),
    app = express();

var system_passwd_path = '/srv/storage/concierge/passwd/system';

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
var save_system_password = function (_req, _passwd, _callback) {

  fs.open(system_passwd_path, 'w', 0640, function (_err, _fd) {

    if (_err) {
      _req.flash('error', "Internal error: file open failed");
      return _callback(_err);
    }

    var buffer = _passwd + '\n';
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
 * read_system_password:
 */
var read_system_password = function (_callback) {

  try {
    fs.readFile(system_passwd_path, function (_err, _data) {
  
      if (_err) {
        _req.flash('error', "Internal error: file read failed");
        return _callback(_err);
      }
    
      return _callback(null, trim(_data.toString()));
    });
  } catch (e) {
    return _callback(e);
  }
};

/**
 * set_unix_password:
 */
var set_unix_password = function (_req, _passwd, _confirm, _callback) {

  /* Set UNIX system password:
      This can be used to log in via OpenSSH. */

  var passwd_process = child.spawn(
    'sudo', [ 'passwd', 'tc' ], { stdio: 'pipe' }
  );

  passwd_process.stdin.write(_passwd + '\n' + _confirm + '\n');
   
  passwd_process.on('exit', function (_code, _signal) {

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
var set_couchdb_password = function (_req, _passwd, _confirm, _callback) {

  /* Set CouchDB administrative password:
      This takes the database out of 'admin party' mode. */

  var proto = 'http://';
  var server = 'localhost:5984';
  var admins_uri = server + '/_config/admins'

  var put = {
    body: JSON.stringify(_passwd),
    headers: { 'Content-type': 'application/json' },
    uri: proto + admins_uri + '/admin'
  };

  /* Start talking to CouchDB:
      This process involves multiple requests; async used for clarity. */

  async.waterfall([

    /* Step 0: Read existing password, if any */
    function (_cb) {
      read_system_password(function (_err, _system_passwd) {
        if (!_err) {
          put.auth = 'service:' + _system_passwd;
        }
        return _cb();
      });
    },
    
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
    function (_system_passwd, _cb) {

      put.auth = 'admin:' + _passwd;
      put.uri = proto + admins_uri + '/service';
      put.body = JSON.stringify(_system_passwd);

      request.put(put, function (_err, _resp, _body) {
	return check_response(
	  _err, _resp, _req, 'System account creation', function (_e) {
	    return _cb(_e, _system_passwd);
	  }
	);
      });
    },

    /* Step 4: Filesystem storage of secondary password */
    function (_system_passwd, _cb) {

      /* Store secondary administrative password in filesystem:
	  This is used by local services needing to connect to CouchDB. */

      save_system_password(_req, _system_passwd, function (_err) {
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
var set_password = function (_req, _passwd, _confirm, _callback) {

  /* Start of password change:
      Validate the supplied password and confirmation. */

  if ((_passwd || '').length < 8) {
    return request_error(
      'Password must be at least eight characters',
        _req, _callback
    );
  }

  if (_passwd !== _confirm) {
    return request_error(
      'Passwords do not match; please try again',
        _req, _callback
    );
  }

  /* Password databases are modified here */
  set_unix_password(_req, _passwd, _confirm, function (_err) {

    if (_err) {
      return _callback(_err);
    }

    set_couchdb_passwd(_req, _passwd, _confirm, _callback);
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

