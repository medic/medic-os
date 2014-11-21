
var child = require('child_process'),
    cookieParser = require('cookie-parser'),
    session = require('express-session'),
    bodyParser = require('body-parser'),
    helpers = require('./lib/helpers'),
    flash = require('connect-flash'),
    request = require('request'),
    express = require('express'),
    crypto = require('crypto'),
    _ = require('underscore'),
    async = require('async'),
    clone = require('clone'),
    fs = require('fs'),
    app = express();

/**
 * Configuration options:
 *   These are module-global constants.
 */
var user = 'vm';
var protocol = 'http://';
var server = 'localhost:5984';
var api_server = 'localhost:5988';
var private_path = '/srv/scripts/concierge/private';
var system_passwd_path = '/srv/storage/concierge/passwd/system';

/**
 * Start express:
 *   We need to do this before using `app`.
 */

app.use(flash());
app.use(bodyParser.urlencoded({ extended: true }));

app.use(cookieParser('50a812553b4e5a5660cbdb18525ee9f01120d11f', {
  saveUninitialized: true, resave: true
}));

app.use(session({
  saveUninitialized: true, resave: true,
  secret: '491dfd0f53bbfcafe6e8bba45a89fdec9a1f298a'
}));

app.set('views', __dirname + '/views');
app.use('/static', express.static(__dirname + '/static'));

helpers.register();

/**
 * Root directory:
 *   HTTP API method. Redirect to `/setup`.
 */
app.get('/', function (_req, _res) {
  _res.redirect('/setup');
});

/**
 * /setup:
 *   HTML API method. Emit the main user interface.
 */
app.get('/setup', function (_req, _res) {

  read_system_password(function (_err, _sys_passwd) {
    _res.render('setup/index.hbs', {
      title: (
        'Setup - Medic Mobile'
      ),
      data: {
        key: _req.flash('key'),
        step: _req.flash('step')
      },
      messages: {
        error: _req.flash('error'),
        success: _req.flash('success')
      },
      options: {
        lock: !_err
      }
    });
  });
});

/**
 * /setup/finish:
 *   REST API method. Shut down concierge, and allow the
 *   frontend nginx proxy to fail over.
 */
app.all('/setup/finish', function (_req, _res) {

  if (_req.method != 'POST' && _req.method != 'GET') {
    _res.status(500).send('Invalid HTTP method');
  }

  disable_concierge_service(_req, function (_err) {
    _res.status(500).send('Service shutdown failed');
  });
});

/**
 * /setup/password:
 *   REST API method. Set the administrative password.
 *   Parameters are ordinary non-JSON POST parameters; the
 *   `password` and `confirmation` parameters are required,
 *   and the `key` parameter is optional.
 */
app.all('/setup/password', function (_req, _res) {

  _req.flash('error', null);

  if (_req.method != 'POST' && _req.method != 'GET') {
    _res.status(500).send('Invalid HTTP method');
  }

  var user = _req.param('user');
  var key = trim(_req.param('key'));
  var password = _req.param('password');
  var confirmation = _req.param('confirmation');

  if (key.length > 0) {
    _req.flash('key', key);
  }

  async.waterfall([

    function (_next_fn) {
      setup_accounts(_req, user, password, confirmation, _next_fn);
    },

    function (_sys_passwd, _next_fn) {
      add_couchdb_defaults(_req, _sys_passwd, _next_fn);
    }

  ], function (_err) {

    if (_err) {
      return send_password_response(_err, _req, _res);
    }

    if (key.length <= 0) {
      return send_password_response(
        null, _req, _res, 'Password successfully set'
      );
    }

    add_openssh_public_key(_req, key, function (_e) {
      return send_password_response(
        _e, _req, _res, 'Password and public key successfully set'
      );
    });
  });
});

/**
 * /setup/poll:
 *   REST API method. Check to see whether or not gardener has
 *   started all required services (e.g. the API server/proxy).
 *   Returns a JSON-encoded object containing a `status` property
 *   (a string) and a `detail` property (an object).
 */
app.get('/setup/poll', function (_req, _res) {

  if (_req.method != 'POST' && _req.method != 'GET') {
    _res.status(500).send('Invalid HTTP method');
  }

  poll_required_services(_req, _res, function (_rv) {
    _res.set('Content-Type', 'application/json');
    return _res.status(200).send(JSON.stringify(_rv));
  });
});

/**
 * poll_required_services:
 *   Helper function for the `/setup/poll` REST API method.
 *   Figure out if the required background services are
 *   running, then call `_callback(_err, _data)`. The `_err`
 *   parameter is an object describing a connection error
 *   (or null if there was no connection error); `_data` is
 *   an object describing the state of the background services,
 *   including a boolean `ready` property, and a human-readable
 *   `detail` property (a string).
 */
var poll_required_services = function (_req, _res, _callback) {

  var rv = { ready: false, handler: 'concierge' };
  var get = { uri: protocol + api_server + '/api/info' };

  request.get(get, function (_err, _resp, _body) {

    if (_err) {
      return _callback(_.extend(rv, {
        detail: 'Unable to contact the medic-api service'
      }));
    }

    if (_resp.statusCode != 200) {
      return _callback(_.extend(rv, {
        detail: 'Error requesting medic-api version information'
      }));
    }

    try {
      var info = JSON.parse(_body);
    } catch (_e) {
      return _callback(_.extend(rv, {
        detail: 'Invalid JSON response returned by medic-api'
      }));
    }

    return _callback(_.extend(rv, {
      ready: true, version: info.version,
      detail: 'All required services are currently running'
    }));
  });  
};

/**
 * _regenerate_view:
 *   Request `_view_url` using `_request_params`, in order to make
 *   sure a map/reduce view is up to date. Completely ignore the
 *   response body. Call `_callback` when finished, using a normal
 *   Node-style error object as the first parameter.
 */
var _regenerate_view = function (_view_url, _request_params, _callback) {

  var get = _.extend(
    { uri: _view_url }, (_request_params || {})
  );

  /* Just request the view:
   *  It will be regenerated *before* the results are returned. */

  request.get(get, function (_err, _resp, _body) {

    if (_err || _resp.statusCode != 200) {
      return _callback(new Error(
        'Failed to regenerate index for map/reduce view'
      ));
    }

    /* Don't parse result:
     *   Parsing the JSON would just waste CPU time. Trust the
     *   HTTP status code, and consider the view regenerated. */

    return _callback();
  });
};

/**
 * regenerate_views:
 *   Helper function for the `/setup/poll` REST API method.
 *   Regenerate all map/reduce views for the CouchDB database
 *   provided in `_database_url`, using views attached to the
 *   design document named `_ddoc_name`. After all views are
 *   finished regenerating, call `_callback(rv)`, where `rv`
 *   is an object containing at least a `ready` property.
 */
var regenerate_views = function (_database_url, _ddoc_name,
                                 _request_params, _callback) {

  var rv = { ready: false, handler: 'concierge' };
  var uri = [ _database_url, '_design', _ddoc_name ].join('/');

  var get = _.extend(
    { uri: uri }, (_request_params || {})
  );

  request.get(get, function (_err, _resp, _body) {

    if (_err || _resp.statusCode != 200) {
      return _callback(_.extend(rv, {
        detail: 'Failed to retrieve design document'
      }));
    }

    try {
      var ddoc = JSON.parse(_body);
    } catch (_e) {
      return _callback(_.extend(rv, {
        detail: 'Invalid JSON response returned by database server'
      }));
    }

    if (!_.isObject(ddoc) || !_.isObject(ddoc.views)) {
      return _callback(_.extend(rv, {
        detail: 'Database server returned an improperly-structured document'
      }));
    }

    /* For each view name... */
    async.each(
      _.keys(ddoc.views),

      /* Iterator */
      function (_view_name, _fn) {

        /* Skip internal view names */
        if (_view_name === 'lib') {
          return _fn();
        }

        /* Make URL */
        var view_url = [
          _database_url, '_design', _ddoc_name, '_view', _view_name
        ].join('/');

        /* Actually regenerate the view */
        _regenerate_view(view_url, _request_params, _fn);
      },

      /* Completion */
      function (_err) {
        if (_err) {
          return _callback(_.extend(rv, {
            failure: true, error: _err.message,
            detail: 'Failure while regenerating map/reduce views'
          }));
        }
        return _callback(_.extend(rv, {
          ready: true,
          detail: 'All map/reduce views regenerated successfully'
        }));
      }
    );
  });
};

/**
 * send_password_response:
 *   Helper function. Send an appropriate response to the client
 *   immediately following an administrative password-change operation.
 */
var send_password_response = function (_err, _req, _res, _success_text) {

  if (_req.param('api')) {
    if (_err) {
      return _res.status(500).send(_err.message);
    } else {
      return _res.status(200).send(_success_text);
    }
  }

  if (!_err) {
    _req.flash('step', 2);
    _req.flash('success', _success_text);
  }

  _req.flash('key', null);
  return _res.redirect('/setup');
};

/**
 * fatal:
 *   Report a process-fatal error and exit.
 */
var fatal = function (_message, _exception) {

  process.stderr.write('fatal: ' + _message + '\n');
  process.exit(1);
};

/**
 * trim:
 *   Remove leading and trailing whitespace from `_string`.
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
 *   Return true if the code in `_status` is a 2xx status
 *   code; false otherwise.
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
 *   Commit suicide. Ask the system's service supervisor to disable
 *   us, so that there's no way our elevated privileges can be used
 *   to mount an attack on the system. The head-end Nginx proxy is
 *   configured to fail over transparently.
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
 *   Add an OpenSSH-style private key to the globally-configured
 *   user's `authorized_keys` file. This function uses `sudo` and
 *   `ssh-addkey` to perform the actual file manipulation.
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
 *   Save the CouchDB `service` account password to the filesystem,
 *   in the globally-configured `system_passwd_path`. This password
 *   allows local services to connect to CouchDB with administrative
 *   privileges.
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
 *   Read an existing in-filesystem `service` account password from
 *   the globally-configured `system_passwd_path`. This password allows
 *   local services to connect to CouchDB with administrative privileges.
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
 *   Set the system password for the globally configured `user`
 *   account. If an error occurs, it is reported back to the client
 *   via `request_error` using a flash message.
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
 * setup_couchdb_accounts:
 *   Set a new administrative password for the local instance of
 *   CouchDB. If this is the first time the function is being used,
 *   the database server will be removed from "admin party" mode, and
 *   start requiring user authentication/authorization for operations.
 */
var setup_couchdb_accounts = function (_req, _user,
                                       _passwd, _confirm, _callback) {

  var is_first_run = false;
  var system_passwd = false;

  var request_template = {
    headers: { 'Content-type': 'application/json' }
  };

  /* Start talking to CouchDB:
      This process involves multiple requests; async used for clarity. */

  async.waterfall([

    /* Step 0:
     *   Read system password, if it's already set. */

    function (_cb) {

      read_system_password(function (_err, _system_passwd) {

        if (!_err) {
          request_template.auth = { user: 'service', pass: _system_passwd };
        }

        /* Ignore errors: file might not exist */
        system_passwd = _system_passwd;
        return _cb();
      });
    },

    /* Step 1:
     *   Change the administrator password. */

    function (_cb) {

      var put = make_couchdb_password_change_request(
        'admin', _passwd, request_template
      );

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'Password setup', _cb
        );
      });
    },

    /* Step 2:
     *   Generate a random system password, if necessary. This
     *   will allow local processes to connect to the database. */

    function (_cb) {

      if (system_passwd) {
        return _cb();
      }

      /* Generate random 2048-bit password */
      crypto.randomBytes(256, function (_err, _data) {

        if (_err) {
          return _cb(_err);
        }

        is_first_run = true;
        system_passwd = _data.toString('hex');

        return _cb();
      });
    },

    /* Step 3:
     *   Change the password for the system service account. */

    function (_cb) {

      if (is_first_run) {
        request_template.auth = { user: 'admin', pass: _passwd };
      } else {
        request_template.auth = { user: 'service', pass: system_passwd };
      }

      var put = make_couchdb_password_change_request(
        'service', system_passwd, request_template
      );

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'System password setup', _cb
        );
      });
    },

    /* Step 4:
     *   Store system password in the local filesystem. */

    function (_cb) {

      /* Store system password in filesystem:
          This is used by local services connecting to CouchDB. */

      save_system_password(_req, system_passwd, function (_err) {
        return _cb(_err);
      });
    },

    /* Step 5:
     *   Create a user document for the `admin` user. */

    function (_cb) {

      if (!is_first_run) {
        return _cb();
      }

      var put = make_couchdb_user_creation_request(
        'admin', _passwd, request_template
      );

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'Administrator user creation', _cb
        );
      });
    },

    /* Step 6:
     *   Set up an admin-enabled password for the named user. */

    function (_cb) {

      var put = make_couchdb_password_change_request(
        _user, _passwd, request_template
      );

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'User password setup', _cb
        );
      });
    },

    /* Step 7:
     *   Create a user document for the named user. */

    function (_cb) {

      var put = make_couchdb_user_creation_request(
        _user, _passwd, request_template
      );

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'User creation', _cb
        );
      });
    }

  ], function (_err) {

    /* Finished:
     *   Deliver the randomly-generated password to our caller. */

    return _callback(_err, system_passwd);
  });
};      

/**
 * make_couchdb_user_creation_request:
 *   Return an PUT request, formatted as an object, that can be
 *   used with `request` to create a new CouchDB user account.
 */
var make_couchdb_user_creation_request = function (_name, _passwd,
                                                   _request_template) {
  var doc = {
    _id: 'org.couchdb.user:' + _name,
    roles: [], type: 'user', name: _name, password: _passwd
  };

  var req = {
    body: JSON.stringify(doc),
    uri: protocol + server + '/_users/' + doc._id
  };

  return _.extend(req, (_request_template || {}));
};

/**
 * make_couchdb_password_change_request:
 */
make_couchdb_password_change_request = function (_name, _password,
                                                 _request_template) {

  var admins_uri = server + '/_config/admins';

  var req = {
    body: JSON.stringify(_password),
    uri: [ protocol + admins_uri, _name ].join('/')
  };

  return _.extend(req, (_request_template || {}));
};

/**
 * setup_accounts:
 */
var setup_accounts = function (_req, _user,
                               _passwd, _confirm, _callback) {

  /* Start of account setup:
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

  /* Change passwords:
   *   The system and CouchDB passwords are modified here. */

  async.waterfall([
    function (_next_fn) {
      set_unix_password(
        _req, _passwd, _confirm, _next_fn
      );
    },
    function (_next_fn) {
      setup_couchdb_accounts(
        _req, _user, _passwd, _confirm, _next_fn
      );
    }
  ], function (_err, _system_passwd) {
    return _callback(_err, _system_passwd);
  });
};

/**
 * add_couchdb_defaults:
 *   Modify the CouchDB configuration, adding any options that
 *   are required by Medic Mobile's applications. When execution
 *   is complete, `_callback` will be invoked with a Node-style
 *   error parameter.
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
 *   Execution begins here.
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

/* Start */
main(process.argv);

