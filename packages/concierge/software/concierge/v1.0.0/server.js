
var child = require('child_process'),
    flash = require('connect-flash'),
    request = require('request'),
    express = require('express'),
    crypto = require('crypto'),
    async = require('async'),
    fs = require('fs'),
    app = express();

var user = 'vm';
var protocol = 'http://';
var server = 'localhost:5984';
var private_path = '/srv/scripts/concierge/private';
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

  read_system_password(function (_err, _system_passwd) {
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
      },
      options: {
        lock: !_err
      }
    });
  });
});

/**
 */
app.all('/setup/finish', function (req, res) {

  if (req.method != 'POST' && req.method != 'GET') {
    res.send(500, 'Invalid HTTP method');
  }

  disable_concierge_service(req, function (_err) {
    res.send(500);
  });

});

/*
 */
app.all('/setup/password', function (req, res) {

  req.flash('error', null);

  if (req.method != 'POST' && req.method != 'GET') {
    res.send(500, 'Invalid HTTP method');
  }

  var key = trim(req.param('key'));
  var password = req.param('password');
  var confirmation = req.param('confirmation');

  if (key.length > 0) {
    req.flash('key', key);
  }

  set_password(req, password, confirmation, function (_err, _system_passwd) {
  
    if (_err) {
      return send_password_response(_err, req, res);
    }

    add_couchdb_defaults(req, _system_passwd, function (_err) {

      if (_err) {
        return send_password_response(_err, req, res);
      }
      
      if (key.length <= 0) {
        return send_password_response(
          null, req, res, 'Password successfully set'
        );
      }

      add_openssh_public_key(req, key, function (_e) {
        return send_password_response(
          _e, req, res, 'Password and public key successfully set'
        );
      });
    });

  });

});

/**
 * send_password_response:
 */
var send_password_response = function (_err, _req, _res, _success_text) {

  if (_req.param('api')) {
    if (_err) {
      return _res.send(500, _err.message);
    } else {
      return _res.send(200, _success_text);
    }
  }

  if (!_err) {
    _req.flash('success', _success_text);
  }

  _req.flash('key', null);
  return _res.redirect('/setup');
};

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
 * http_status_successful:
 */
var http_status_successful = function (_status) {

  return (_status >= 200 && _status < 300);
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

  if (!http_status_successful(_resp.statusCode)) {
    return request_error(
      _text + ' failed: ' + 'problem with database server',
        _req, _cb
    );
  }

  return _cb();
};

/**
 * disable_concierge_service:
 */
var disable_concierge_service = function (_req, _callback) {

  /* Terminate and disable the concierge process:
       That's us, so take care to finish up before we spawn a subprocess. */

  var disable = child.spawn(
    'sudo', [ private_path + '/concierge-disable' ],
      { stdio: 'pipe' }
  );
  
  disable.stdin.end();

  disable.on('exit', function (_code, _signal) {

    /* Error handling:
        If we're successful, our process will exit on SIGTERM,
        and this exit event will not be reached. If we do see a
        subprocess exit, something went wrong (we're still alive). */
        
    return request_error(
      'Failed to shut down: internal error',
        _req, _callback
    );
  });

};

/**
 * add_openssh_public_key:
 */
var add_openssh_public_key = function (_req, _key, _callback) {

  /* Add data to OpenSSH's authorized_keys file:
       This feature requires the `ssh-addkey` script and sudo privileges. */

  var addkey = child.spawn(
    'sudo', [ '-u', user, private_path + '/ssh-addkey' ],
      { stdio: 'pipe' }
  );

  addkey.stdin.write(_key);
  addkey.stdin.end();

  addkey.on('exit', function (_code, _signal) {

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
    
    fs.write(_fd, buffer, 0, 'utf-8', function (_err, _len, _buf) {
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

  try {
    var passwd = child.spawn(
      'sudo', [ 'passwd', user ], { stdio: 'pipe' }
    );

    passwd.stdin.write(_passwd + '\n' + _confirm + '\n');
    passwd.stdin.end();
   
    passwd.on('exit', function (_code, _signal) {

      if (_code != 0) {
        return request_error(
          'Password change failed: password utility indicated a failure',
            _req, _callback
        );
      }

      return _callback();
    });
  } catch (e) {
    return request_error(
      "Password change failed: couldn't find/execute password utility",
        _req, _callback
    );
  }
};

/**
 * set_couchdb_password:
 */
var set_couchdb_password = function (_req, _passwd, _confirm, _callback) {

  /* Set CouchDB administrative password:
      This takes the database out of 'admin party' mode. */

  var admins_uri = server + '/_config/admins'

  var put = {
    body: JSON.stringify(_passwd),
    uri: protocol + admins_uri + '/admin',
    headers: { 'Content-type': 'application/json' }
  };
  
  /* Start talking to CouchDB:
      This process involves multiple requests; async used for clarity. */

  async.waterfall([

    /* Step 0: Read system password, if it's already set */
    function (_cb) {
    
      read_system_password(function (_err, _system_passwd) {
        if (!_err) {
          put.auth = { user: 'service', pass: _system_passwd };
        }
        /* Ignore errors: file might not exist */
        return _cb(null, _system_passwd);
      });
    },
    
    /* Step 1: Primary password change (i.e. admin) */
    function (_system_passwd, _cb) {

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'Primary password change', function(_e) {
            return _cb(_e, _system_passwd);
          }
        );
      });
    },

    /* Step 2: Secondary password generation, if necessary */
    function (_system_passwd, _cb) {
    
      if (_system_passwd) {

        return _cb(null, _system_passwd, false);
      }

      /* Generate random 2048-bit password */
      crypto.randomBytes(256, function (_err, _data) {
        if (_err) {
          return _cb(_err);
        }

        return _cb(null, _data.toString('hex'), true);
      });
    },

    /* Step 3: Secondary password change */
    function (_system_passwd, _first_run, _cb) {
    
      if (_first_run) {
        put.auth = { user: 'admin', pass: _passwd };
      } else {
        put.auth = { user: 'service', pass: _system_passwd };
      }
      
      put.body = JSON.stringify(_system_passwd);
      put.uri = protocol + admins_uri + '/service';

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'System account creation', function (_e) {

            return _cb(_e, _system_passwd, _first_run);
          }
        );
      });
    },

    /* Step 4: Create user document for `admin` */
    function (_system_passwd, _first_run, _cb) {
    
      if (!_first_run) {
        return _cb(null, _system_passwd);
      }
      
      var doc = {
        _id: 'org.couchdb.user:admin', roles: [],
        type: 'user', name: 'admin', password: null
      };
      
      put.body = JSON.stringify(doc);
      put.uri = protocol + server + '/_users/' + doc._id,
      
      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'Administrative account creation',
          function (_e) {
            return _cb(_e, _system_passwd);
          }
        );
      });
    },
    
    /* Step 5: In-filesystem storage of secondary password */
    function (_system_passwd, _cb) {

      /* Store secondary administrative password in filesystem:
          This is used by local services needing to connect to CouchDB. */

      save_system_password(_req, _system_passwd, function (_err) {
        return _cb(_err, _system_passwd);
      });
    }

  ], function (_err, _system_passwd) {
    return _callback(_err, _system_passwd);
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

    set_couchdb_password(_req, _passwd, _confirm, _callback);
  });
};

/**
 * add_couchdb_defaults:
 */
var add_couchdb_defaults = function (_req, _system_passwd, _callback) {

  var put = {
    auth: { user: 'service', pass: _system_passwd },
    headers: { 'Content-Type': 'application/json' }
  };
  
  async.waterfall([

    /* Step 1: Restrict CouchDB to valid users */
    function (_cb) {
      var config_uri = server + '/_config/couch_httpd_auth'

      put.body = '"true"';
      put.uri = protocol + config_uri + '/require_valid_user';

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'Configuration change', _cb
        );
      });
    }
  ], 
  function (_err) {
    return _callback(_err);
  });
}

/**
 * main:
 */
var main = function (_argv) {

  if (process.getuid() !== 0) {
    fatal("This application must be started in privileged mode; use sudo");
  }

  try {
    app.listen(280);
  } catch (e) {
    fatal("Couldn't bind socket for listening on port 280", e);
  }
 
  try {
    process.setuid('concierge');
  } catch (e) {
    fatal("Couldn't drop privileges; ensure user 'concierge' exists", e);
  }
};

main(process.argv);

