
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
    path = require('path'),
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
var system_passwd_dir = '/srv/storage/concierge/passwd';

/**
 * Background task status:
 *   Some setup tasks, including view regeneration, need to happen in
 *   ithe background so that we can display status/progress information
 *   in the browser. From an HTTP perspective, only the initial `admin`
 *   password change is performed synchronously; the remainder of the
 *   setup process spans requests. If this variable is a boolean `true`,
 *   the background setup process has started. If it's an object, the
 *   background process has completed and the object is the result. */

var background_task_status = false;

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

  /* Reset to first step */
  _req.session.step = 1;
  
  /* Start setup process */
  _res.redirect('/setup');
});

/**
 * /setup:
 *   HTML API method. Emit the main user interface.
 */
app.get('/setup', function (_req, _res) {

  if (!_req.session.step) {
    _req.session.step = 1;
  }

  _res.render('setup/index.hbs', {
    title: (
      'Setup - Medic Mobile'
    ),
    data: {
      key: _req.session.key,
      step: _req.session.step,
      name: _req.session.name,
      fullname: _req.session.fullname,
      password: _req.flash('password'),
      confirmation: _req.flash('confirmation')
    },
    messages: {
      error: _req.flash('error'),
      success: _req.flash('success')
    }
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
app.post('/setup/password', function (_req, _res) {

  if (_req.body.action === 'back') {
    _req.session.step = 1;
    return _res.redirect('/setup');
  }

  var user = {
    name: _req.body.name,
    fullname: _req.body.fullname
  };

  _req.flash('error', null);
  _req.flash('success', null);

  var key = trim(_req.body.key);
  var password = _req.body.password;
  var confirmation = _req.body.confirmation;

  _req.flash('password', password);
  _req.flash('confirmation', confirmation);
  
  _req.session.key = key;
  _req.session.name = user.name;
  _req.session.fullname = user.fullname;

  /* Start setup */
  async.waterfall([

    /* Step 1:
     *   Secure system and CouchDB. */

    function (_next_fn) {
      setup_minimal_accounts(_req, user, password, confirmation, _next_fn);
    },

    /* Step 2:
     *   Perform any required database modifications as `admin`. */

    function (_next_fn) {
      add_couchdb_defaults(_req, password, _next_fn);
    },

    /* Step 3:
     *   Start the background portion of the setup process. */

    function (_next_fn) {

      /* Block reentrance */
      background_task_status = true;

      /* Start background processing */
      run_background_setup_tasks(_req, user, password, function (_rv) {
        background_task_status = _rv;
      });

      /* Intentional */
      return _next_fn();
    }

  ], function (_err) {

    /* Finished:
     *   Send a result page, start browser-based polling. */

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

  poll_required_services(_req, _res, function (_rv) {
    _res.set('Content-Type', 'application/json');
    return _res.status(200).send(JSON.stringify(_rv));
  });
});

/**
 * poll_required_services:
 *   Helper function for the `/setup/poll` REST API method.  Figure
 *   out if the required background services are running, then call
 *   `_callback(_err, _data)`. The `_err` parameter is an object
 *   describing a connection error (or null if there was no connection
 *   error); `_data` is an object describing the state of the background
 *   services, including a boolean `ready` property, and a supplementary
 *   human-readable `detail` property (a string).
 */

var poll_required_services = function (_req, _res, _callback) {

  var rv = { ready: false, handler: 'concierge' };
  var get = { url: protocol + api_server + '/api/info' };

  /* Database setup phase */
  rv.phase = 'database';

  /* Wait for background tasks */
  if (!_.isObject(background_task_status)) {
    return _callback(_.extend(rv, {
      detail: 'Databases and views are being initialized'
    }));
  }

  if (background_task_status.failure) {
    return _callback(background_task_status);
  }

  /* API startup phase */
  rv.phase = 'api';

  /* Wait for medic-api to start */
  request.get(get, function (_err, _resp, _body) {

    if (_err) {
      return _callback(_.extend(rv, {
        detail: 'The medic-api service has not started yet'
      }));
    }

    if (!http_status_successful(_resp.statusCode)) {
      return _callback(_.extend(rv, {
        failure: true,
        detail: 'Error requesting medic-api version information'
      }));
    }

    try {
      var info = JSON.parse(_body);
    } catch (_e) {
      return _callback(_.extend(rv, {
        failure: true,
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
 * _regenerate_couchdb_view:
 *   Request `_view_url` using `_request_params`, in order to make
 *   sure a map/reduce view is up to date. Completely ignore the
 *   response body. Call `_callback` when finished, using a normal
 *   Node-style error object as the first parameter.
 */
var _regenerate_couchdb_view = function (_view_url,
                                         _request_params, _callback) {

  var get = _.extend(
    { url: _view_url }, (_request_params || {})
  );

  /* Just request the view:
   *  It will be regenerated *before* the results are returned. */

  request.get(get, function (_err, _resp, _body) {

    if (_err || !http_status_successful(_resp.statusCode)) {
      return _callback(
        new Error("Failed to request view from '" + _view_url + "'")
      );
    }

    /* Don't parse result:
     *   Parsing the JSON would just waste CPU time. Trust the
     *   HTTP status code, and consider the view regenerated. */

    return _callback();
  });
};

/**
 * regenerate_couchdb_views:
 *   Helper function for the `/setup/poll` REST API method.
 *   Regenerate all map/reduce views for the CouchDB database
 *   provided in `_database_url`, using views attached to the
 *   design document named `_ddoc_name`. After all views are
 *   finished regenerating, call `_callback(rv)`, where `rv`
 *   is an object containing at least a `ready` property.
 */
var regenerate_couchdb_views = function (_database_url, _ddoc_name,
                                         _request_params, _callback) {

  var url = [ _database_url, '_design', _ddoc_name ].join('/');

  var get = _.extend(
    { url: url }, (_request_params || {})
  );

  request.get(get, function (_err, _resp, _body) {

    if (_err || !http_status_successful(_resp.statusCode)) {
      return _callback(
        new Error('Failed to retrieve design document')
      );
    }

    try {
      var ddoc = JSON.parse(_body);
    } catch (_e) {
      return _callback(
        new Error('Invalid JSON response returned by database server')
      );
    }

    if (!_.isObject(ddoc) || !_.isObject(ddoc.views)) {
      return _callback(
        new Error('Database server returned a malformed document')
      );
    }

    /* In parallel */
    async.each(

      /* For each view... */
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
        _regenerate_couchdb_view(view_url, _request_params, _fn);
      },

      /* Completion handler */
      function (_err) {

        /* Finished */
        return _callback(_err);
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

  if (_req.body.api) {
    if (_err) {
      return _res.status(500).send(_err.message);
    } else {
      return _res.status(200).send(_success_text);
    }
  }

  if (!_err) {
    _req.session.step = 2;
    _req.flash('success', _success_text);
  }

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
      _text + ' failed: ' + 'problem with server',
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
 *   in the globally-configured `system_passwd_dr` directory. This
 *   password allows local services to connect to CouchDB with
 *   administrative privileges.
 */
var save_system_password = function (_name, _passwd, _callback) {

  var system_passwd_path = path.join(system_passwd_dir, _name);

  fs.open(system_passwd_path, 'w', 0640, function (_err, _fd) {

    if (_err) {
      return _callback(_err);
    }

    var buffer = _passwd + '\n';

    fs.write(_fd, buffer, 0, 'utf-8', function (_err, _len, _buf) {
      if (_err) {
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
var read_system_password = function (_name, _callback) {

  var system_passwd_path = path.join(system_passwd_dir, _name);

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
 * is_builtin_user_name:
 *   Return true if the user name `_user_name` is reserved by
 *   the system for internal use. Otherwise, return false.
 */
var is_builtin_user_name = function (_user_name) {

  /* Reserved */
  var builtin_users = [
    'admin', 'lucene', 'gardener', 'transport', 'concierge' 
  ];

  /* Normalize */
  var user_name = (
    (_user_name || '').replace(/^org\.couchdb\.user:/, '')
  );

  return _.contains(builtin_users, user_name);
};

/**
 * _delete_couchdb_user:
 */
var _delete_couchdb_user = function (_user_name, _is_admin,
                                     _request_params, _callback) {

  var users_url = protocol + server + '/_users/';
  var config_url = protocol + server + '/_config/';

  /* Primary URL:
   *   Either the `admins` document or the user document. */

  var primary_url = (
    config_url + '/' +
      (_is_admin ? 'admins' : 'users') + '/' + _user_name
  );
  
  /* Secondary URL:
   *   If we're deleting an administrative user, we'll also try
   *   to delete the ordinary user document for that user, if
   *   it's present. If it's not or deletion fails, ignore it. */
  
  var secondary_url = false;

  if (_is_admin) {
    secondary_url = users_url + 'org.couchdb.user:' + _user_name;
  }

  var req = _.extend(
    { url: primary_url }, (_request_params || {})
  );

  /* Let's kill some users */
  request.del(req, function (_err, _resp, _body) {

    if (_err || !http_status_successful(_resp.statusCode)) {
      return _callback(new Error('Failed to delete user'));
    }

    /* Early exit:
     *   If we were deleting a non-admin user, we're done. */

    if (!secondary_url) {
      return _callback();
    }

    req.url = secondary_url;

    /* Get document revision */
    request.get(req, function (_err, _resp, _body) {

      /* Errors intentionally ignored */
      if (_err || !http_status_successful(_resp.statusCode)) {
        return _callback();
      }

      try {
        var doc = JSON.parse(_body);
      } catch (_e) {
        return _callback();
      }

      /* Errors intentionally ignored */
      if (!_.isObject(doc) || !doc._rev) {
        return _callback();
      }

      /* Include revision */
      req.qs = { rev: doc._rev };

      /* Delete user document; ignore errors */
      request.del(req, function (_err, _resp, _body) {
        return _callback();
      });
    });
  });
};

/**
 * delete_couchdb_unknown_users:
 */
var delete_couchdb_unknown_users = function (_query_admins_list,
                                             _request_params, _callback) {
  var url = (
    protocol + server + '/_config/' +
      (_query_admins_list ? 'admins' : 'users')
  );

  var get = _.extend(
    { url: url }, (_request_params || {})
  );

  request.get(get, function (_err, _resp, _body) {

    if (_err || !http_status_successful(_resp.statusCode)) {
      return _callback(new Error(
        'Failed to retrieve list of users'
      ));
    }

    try {
      var doc = JSON.parse(_body);
    } catch (_e) {
      return _callback(new Error(
        'Invalid JSON response returned by database server'
      ));
    }

    if (!_.isObject(doc)) {
      return _callback(new Error(
        'Database server returned an improperly-structured document'
      ));
    }

    /* In parallel */
    async.each(

      /* For each user... */
      _.keys(doc),

      /* Iterator */
      function (_user_name, _fn) {

        /* Required users: don't delete these */
        if (is_builtin_user_name(_user_name)) {
          return _fn();
        }

        /* Anything else: delete it */
        _delete_couchdb_user(_user_name, true, _request_params, _fn);
      },

      /* Completion */
      function (_err) {

        if (_err) {
          return _callback(new Error(
            'Failure while retrieving list of users'
          ));
        }
        return _callback();
      }
    );
  });
};

/**
 * setup_minimal_couchdb_accounts:
 *   Set a new administrative password for the system and the local
 *   instance of CouchDB. If this is the first time the function is
 *   being called, the database server will be removed from "admin party"
 *   mode, and start requiring user authentication/authorization for
 *   operations. This does not create any other accounts.
 */
var setup_minimal_couchdb_accounts = function (_req,
                                               _user, _passwd,
                                               _confirm, _callback) {

  var system_passwd = false;
  var system_user = 'concierge';

  var request_template = {
    headers: { 'Content-type': 'application/json' }
  };

  /* Start talking to CouchDB:
      This process involves multiple requests; async is used
      to make this easier to modify/extend at a later date. */

  async.waterfall([

    /* Step 1:
     *   Read system password, if it's already set. */

    function (_cb) {

      read_system_password(system_user, function (_err, _system_passwd) {

        if (!_err) {

          /* We have a system password */
          system_passwd = _system_passwd;

          /* Authentication required */
          request_template.auth = {
            user: system_user, pass: system_passwd
          };
        }

        return _cb();
      });
    },

    /* Step 2:
     *   Change the password for `admin`. */

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

    /* Step 3:
     *   Delete any named users created in previous runs. */

    function (_cb) {

      /* Authentication required */
      if (!request_template.auth) {
        request_template.auth = { user: 'admin', pass: _passwd };
      }

      delete_couchdb_unknown_users(true, request_template, _cb);
    },

    /* Step 4:
     *   Create a user document for the `admin` user. */

    function (_cb) {

      var put = make_couchdb_user_creation_request(
        'admin', _passwd, request_template
      );

      request.put(put, function (_err, _resp, _body) {

        /* Detect and ignore conflicts:
         *  A conflict means the document has already been created. */

        if (_resp.statusCode == 409) {
          return _cb();
        }

        return check_response(
          _err, _resp, _req, 'Administrative user creation', _cb
        );
      });
    },

    /* Step 5:
     *   Set up an administrative password for the named user. */

    function (_cb) {

      if (is_builtin_user_name(_user.name)) {
        return _cb(); /* Skip this step */
      }

      var put = make_couchdb_password_change_request(
        _user.name, _passwd, request_template
      );

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'User password setup', _cb
        );
      });
    },

    /* Step 6:
     *   Create a user document for the named user. */

    function (_cb) {

      if (is_builtin_user_name(_user.name)) {
        return _cb(); /* Skip this step */
      }

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
     *   Report any error status back to our caller. */

    return _callback(_err);
  });
};

/**
 * setup_couchdb_service_account:
 *   Set up a special-purpose service account with a random password,
 *   if it hasn't already been done. This service account allows the
 *   local machine to autonomously initiate connections to CouchDB
 *   without requiring that the user's administrative password be
 *   stored anywhere.
 */
var setup_couchdb_service_account = function (_req, _account_name,
                                              _admin_passwd, _callback) {

  var is_first_run = true;
  var system_passwd = false;

  var request_template = {
    headers: { 'Content-type': 'application/json' }
  };

  /* Start talking to CouchDB:
      This process involves multiple requests; async used for clarity. */

  async.waterfall([

    /* Step 1:
     *   Read system password, if it's already set. */

    function (_cb) {

      read_system_password(_account_name, function (_err, _system_passwd) {

        if (_err) {
          return _cb(); /* First run */
        }

        is_first_run = false;
        system_passwd = _system_passwd;

        /* Authentication required */
        request_template.auth = {
          user: _account_name, pass: _system_passwd
        };

        /* Success */
        return _cb();
      });
    },

    /* Step 2:
     *   Generate a new random "system service" password. This
     *   will allow local processes to connect to the database. */

    function (_cb) {

      if (!is_first_run) {
        return _cb(); /* Skip this step */
      }

      /* Generate random 2048-bit password */
      crypto.randomBytes(256, function (_err, _data) {

        if (_err) {
          return _cb(_err);
        }

        system_passwd = _data.toString('hex');
        return _cb();
      });
    },

    /* Step 3:
     *   Set the password for the system service account. */

    function (_cb) {

      /* Authenticate if password is provided */
      if (_admin_passwd && !request_template.auth) {
        request_template.auth = { user: 'admin', pass: _admin_passwd };
      }

      var put = make_couchdb_password_change_request(
        _account_name, system_passwd, request_template
      );

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'System password setup', _cb
        );
      });
    },

    /* Step 4:
     *   Store system service password in the local filesystem. */

    function (_cb) {

      /* Store system password in filesystem:
          This is used by local services connecting to CouchDB. */

      save_system_password(_account_name, system_passwd, _cb);
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
 *   The `_user` argument must be either an object containing a
 *   `name` property and optional `fullname` property, or an
 *   ordinary string representing the user name.
 */
var make_couchdb_user_creation_request = function (_user, _passwd,
                                                   _request_template) {

  if (!_.isObject(_user)) {
    _user = { name: _user };
  }

  var doc = {
    type: 'user', roles: [],
    name: _user.name, password: _passwd,
    _id: 'org.couchdb.user:' + _user.name,
  };

  if (_user.fullname) {
    doc.fullname = _user.fullname;
  }

  var req = {
    body: JSON.stringify(doc),
    url: protocol + server + '/_users/' + doc._id
  };

  return _.extend(req, (_request_template || {}));
};

/**
 * make_couchdb_password_change_request:
 */
make_couchdb_password_change_request = function (_name, _password,
                                                 _request_template) {

  var admins_url = server + '/_config/admins';

  var req = {
    body: JSON.stringify(_password),
    url: [ protocol + admins_url, _name ].join('/')
  };

  return _.extend(req, (_request_template || {}));
};

/**
 * setup_minimal_accounts:
 *   Create any accounts that can be safely created within the
 *   scope of the browser's initial HTTP submission.
 */
var setup_minimal_accounts = function (_req, _user,
                                       _passwd, _confirm, _callback) {

  /* Start of account setup:
      Validate the supplied user name and password. */

  if (!_.isString(_user.fullname) || _user.fullname.length <= 0) {
    return request_error(
      'Your full name cannot be left empty',
        _req, _callback
    );
  }

  if (!_.isString(_user.name) || _user.name.length < 4) {
    return request_error(
      'User name must be at least four characters',
        _req, _callback
    );
  }

  if (!_user.name.match(/^[\d\w\-\.]+$/)) {
    return request_error(
      'User name cannot contain spaces or punctuation',
        _req, _callback
    );
  }

  if (_user.name.match(/^[_\-\.]/)) {
    return request_error(
      'User name cannot start with punctuation',
        _req, _callback
    );
  }

  if (is_builtin_user_name(_user.name)) {
    return request_error(
      'User name already taken; please choose another',
        _req, _callback
    );
  }

  if (!_.isString(_passwd) || _passwd.length < 8) {
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

      /* Secure system-level account */
      set_unix_password(_req, _passwd, _confirm, _next_fn);
    },

    function (_next_fn) {

      /* Set up early-access administrative account */
      setup_couchdb_service_account(_req, 'concierge', false, _next_fn);
    },

    function (_system_passwd, _next_fn) {

      /* Set up user-accessible administrative account */
      setup_minimal_couchdb_accounts(
        _req, _user, _passwd, _confirm, _next_fn
      );
    },

    function (_next_fn) {

      /* Set up administrative account for full-text search */
      setup_couchdb_service_account(_req, 'lucene', _passwd, _next_fn);
    },

  ], function (_err, _system_passwd) {

    /* Finished */
    return _callback(_err);
  });
};

/**
 * run_background_setup_tasks:
 */
var run_background_setup_tasks = function (_req, _user, _passwd, _callback) {

  var rv = {
    failure: true, ready: false,
    handler: 'concierge', phase: 'database'
  };

  /* Change passwords:
   *   The system and CouchDB passwords are modified here. */

  async.waterfall([

    function (_next_fn) {

      /* View regeneration:
       *   Regenerate each view in parallel by requesting its contents.
       *   We don't do this in parallel with the `gardener` startup
       *   process, because the modules it launches may rapidly make
       *   requests. These operations may block while views are being
       *   regenerated, causing requests to pile up and ultimately
       *   overwhelm CouchDB's maximum-concurrent-processes limit. */

      var url = protocol + server + '/medic';
      var req = { auth: { user: 'admin', pass: _passwd } };

      regenerate_couchdb_views(url, 'medic', req, _next_fn);
    },

    function (_next_fn) {

      /* Create gardener service account:
       *   This will allow gardener to authenticate to CouchDB, which
       *   will effectively launch the remainder of the setup process. */

      setup_couchdb_service_account(_req, 'gardener', _passwd, _next_fn);
    },

    function (_system_passwd, _next_fn) {

      /* Create medic-transport service account:
       *   This will allow medic-transport to authenticate to CouchDB,
       *   which will start the message transmission/delivery pipeline. */

      setup_couchdb_service_account(_req, 'transport', _passwd, _next_fn);
    }

  ], function (_err, _system_passwd) {

    if (_err) {
      return _callback(_.extend(rv, {
        message: _err.message,
        detail: 'Failed to set up database server'
      }));
    }

    /* Success */
    return _callback(_.extend(rv, {
      ready: true, failure: false,
      detail: 'Database server initialized successfully'
    }));
  });
};

/**
 * add_couchdb_defaults:
 *   Modify the CouchDB configuration, adding any options that
 *   are required by Medic Mobile's applications. When execution
 *   is complete, `_callback` will be invoked with a Node-style
 *   error parameter.
 */
var add_couchdb_defaults = function (_req, _admin_passwd, _callback) {

  var put = {
    auth: { user: 'admin', pass: _admin_passwd },
    headers: { 'Content-Type': 'application/json' }
  };

  async.waterfall([

    /* Step 1:
     *   Restrict CouchDB to valid users only. */

    function (_cb) {
      var config_url = server + '/_config/couch_httpd_auth'

      put.body = '"true"';
      put.url = protocol + config_url + '/require_valid_user';

      request.put(put, function (_err, _resp, _body) {
        return check_response(
          _err, _resp, _req, 'Configuration change', _cb
        );
      });
    }
  ], 

  /* Completion */
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
    fatal("This application must be started by root; use sudo");
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

