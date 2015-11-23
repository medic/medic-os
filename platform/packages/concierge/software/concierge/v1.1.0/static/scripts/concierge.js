
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
  var limit = (options.limit || 120);
  var attempts = (options.attempts || 0);
  var interval = (options.interval || 2000);

  poll_required_services(_url, function (_err, _result) {

    if (_err) {
      return _callback(_err); /* Network error */
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

      /* Check for explicit failure */
      if (_result.failure) {
        return _callback(null, {
          ready: false,
          detail: 'One or more services failed to start properly'
        });
      }

      /* Iteration complete */
      options.limit = limit;
      options.attempts = ++attempts;

      /* Optional callback */
      if (options.onIteration) {
        options.onIteration.call(null, _result, options);
      }
      
      /* Reschedule */
      setTimeout(function () {
        wait_for_required_services(_url, options, _callback);
      }, interval);
    }
  });
};

/**
 * update_status_text:
 */
var update_status_text = function (_elt, _result, _options) {

  switch (_result.phase) {
    case 'database':
      _elt.html('Initializing databases...');
      break;
    case 'api':
      _elt.html('Starting required services...');
      break;
    default:
      break;
  }

  if (_options.attempts >= 20) {
    _elt.html('Waiting for services to start...');
  }
};


/**
 * show_services_startup:
 */
var show_services_startup = function () {

  var elt = $('.progress .text');

  $('#spinner').show();
  $('#success, #failure').hide();
  $('#success-text, #failure-text').hide();
  $('#finish').prop('disabled', true);

  /* Common error path */
  var handle_error = function (_message) {

    $('#failure').show();
    $('#failure-text').fadeIn();
    elt.html('Platform failed to start');
  };

  /* Options */
  var o = {
    onIteration: function (_result, _options) {
      update_status_text(elt, _result, _options);
    }
  };

  /* Before first poll */
  setTimeout(function () {
 
    /* Provide some early feedback */
    elt.html('Starting the platform...');

  }, 2500);

  /* Start progress */
  setTimeout(function () {

    wait_for_required_services('/setup/poll', o, function (_err, _data) {

      /* Done working */
      $('#spinner').hide();

      if (_err) {
        /* Error connecting to server */
        handle_error("Couldn't connect to platform");
        return;
      }

      if (!_data.ready) {
        /* Wait operation timed out */
        handle_error('Platform failed to start');
        return;
      }

      /* Success */
      $('#success').show();
      $('#success-text').fadeIn();
      $('#finish').prop('disabled', false);

      elt.html('Platform started successfully');
    });
  }, 5000);

  return false;
};

/**
 * main:
 */
jQuery(function ($) {

  $('#back').click(function (_ev) {
    $('#action').val('back');
    $('#form').submit();
    return false;
  });

  $('#next').click(function (_ev) {
    $('#action').val('next');
    $('#form').submit();
    return false;
  });

  $('button').on('mouseover', function (_ev) {
    $(this).addClass('hover');
  });
  
  $('button').on('mouseout', function (_ev) {
    $(this).removeClass('hover');
  });

  $('#retry').on('click', function (_ev) {
    $('.progress .text').html('Configuring platform...');
    show_services_startup();
    return false;
  });

  $('#finish').click(function (_ev) {

    if ($(this).hasClass('disabled')) {
      return false;
    }

    $.ajax({
      url: 'setup/finish',
      complete: function (_xhr, _status) {
        window.location.href = '/medic/_design/medic/_rewrite/';
      }
    });

    /* Do nothing else */
    return false;
  });
});


