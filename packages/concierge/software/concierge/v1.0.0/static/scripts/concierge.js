
var resize_handler = function () {

  var top_elt = $('.login-top');
  var top_height = top_elt.height();
  var background_elt = $('.login-background');

  $('.login-bottom').css({
    top: (top_elt.position().top + top_height) + 'px',
    height: Math.max(background_elt.height() - top_height, 0) + 'px'
  });
};

var textarea_focus = function (_elts) {

  $(_elts).each(function (_i, _elt) {

    var elt = $(_elt);
    var wrap_elt = elt.parents('.textarea-wrapper').first();

    elt.focus(function () {
      wrap_elt.addClass('textarea-focus');
    });

    elt.blur(function () {
      wrap_elt.removeClass('textarea-focus');
    });
  });
};

var button_hover = function (_elts) {

  $(_elts).each(function (_i, _elt) {

    var elt = $(_elt);
    
    if (elt.hasClass('disabled')) {
      return;
    }
      
    elt.mouseenter(function () {
      elt.addClass('hover');
    });

    elt.mouseleave(function () {
      elt.removeClass('hover');
    });
  });
};

var button_submit = function (_submit_elt, _elts) {

  var submit_elt = $(_submit_elt);
  var form_elt = submit_elt.parents('form').first();
  
  var toggle_image = function (_elt) {
    $(_elt).find('.icon-lock-open')
      .removeClass('icon-lock-open').addClass('icon-lock-closed');
  };

  $(_elts).each(function (_i, _elt) {

    var elt = $(_elt);

    elt.click(function () {
      toggle_image(elt);
      
      setTimeout(function () {  
        submit_elt.click();
      }, 100);
      
    });
  });
};

jQuery(function ($) {

  resize_handler();
  button_hover('#save');
  button_hover('#lock');
  textarea_focus('#key');
  button_submit('#submit', '#save');
  $(window).resize(resize_handler);

  $('#lock').click(function (_ev) {
  
    if ($(this).hasClass('disabled')) {
      return false;
    }
    
    $.ajax({
      url: 'setup/finish',
      complete: function (_xhr, _status) {
        window.location.href = 'dashboard/_design/dashboard/_rewrite';
      }
    });
  });
  
});


