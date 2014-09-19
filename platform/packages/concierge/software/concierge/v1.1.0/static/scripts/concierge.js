
/**
 * poll_required_services:
 */
var poll_required_services = function (_url, _callback) {

  $.ajax(_url, {
    success: function (_data, _status, _xhr) {
      return _callback(null, _data);
    },
    error: function (_xhr, _status, _err) {
      return _callback(_err);
    }
  });
};

/**
 * wait_for_required_services:
 */
var wait_for_required_services = function (_url, _options, _callback) {

  var options = (_options || {});
  var limit = (options.limit || 30);
  var interval = (options.interval || 2000);

  poll_required_services(_url, function (_err, _result) {

    if (_err) {
      return _cb(_err);   /* Network error */
    }

    if (_result.ready) {

      /* Success */
      return _callback(null, _result);

    } else {

      /* Check iteration limit */
      if (--limit <= 0) {
        return _callback(null, {
          ready: false,
          detail: 'Timed out while waiting for required services'
        });
      }

      /* Iteration complete */
      options.limit = limit;
      
      /* Reschedule */
      setTimeout(function () {
        wait_for_required_services(_url, options, _callback);
      }, interval);
    }
  });
};

/**
 * main:
 */
jQuery(function ($) {

  $('#lock').click(function (_ev) {

    if ($(this).hasClass('disabled')) {
      return false;
    }

    $.ajax({
      url: 'setup/finish',
      complete: function (_xhr, _status) {
        window.location.href = 'medic/_design/medic/_rewrite';
      }
    });
  });
});


