var assert = require('chai').assert,
    request = require('request'),
    path = describe;

describe('nginx config', function() {

  describe('http://', function() {

    it('should be forwarded to https://', function(done) {

      // when
      get(http('/some-path'))

        .then(function(res) {

          // then
          assert.equal(res.statusCode, 301);
          assert.equal(res.headers.location, 'https://localhost/some-path'); // NB port is removed

        })
        .then(done)
        .catch(done);
    });

  });


  path('/_utils', function() {

    it('should forward to deep futon URL', function(done) {

      // when
      get('/_utils')

        .then(function(res) {

          // then
          assert.equal(res.statusCode, 301);
          assert.equal(res.headers.location, https('/_couch/_utils/index.html'));

        })
        .then(done)
        .catch(done);
    });

  });


  describe('futon access', function() {

    function withAuthHeader(username, password) {
      var creds = Buffer.from(username + ':' + password).toString('base64');
      return {
        headers: {
          Authorization: 'Basic ' + creds,
        },
      }
    }

    it('should be allowed for admin users', function(done) {

      // when
      get('/_couch/_utils/index.html', withAuthHeader('admin', 'pass'))

        .then(function(res) {

          // then
          assert.equal(res.statusCode, 200);

        })
        .then(done)
        .catch(done);
    });

    it('should be denied for non-admin users', function(done) {

      // when
      get('/_couch/_utils/index.html', withAuthHeader('user', 'pass'))

        .then(function(res) {

          // then
          assert.equal(res.statusCode, 403);

        })
        .then(done)
        .catch(done);
    });

  });

  function http(path) {
    return 'http://localhost:8080' + path;
  }

  function https(path) {
    return 'https://localhost:8081' + path;
  }

  function withDefaults(options) {
    return Object.assign({},
        {
          followRedirect: false,
          followOriginalHttpMethod: true,
        },
        options);
  }

  function get(path, options) {
    if(path.indexOf('://') === -1) path = https(path);

    return new Promise(function(resolve, reject) {
      request.get(path, withDefaults(options), function(err, res) {
        if(err) reject(err);
        else resolve(res);
      });
    });
  }

});
