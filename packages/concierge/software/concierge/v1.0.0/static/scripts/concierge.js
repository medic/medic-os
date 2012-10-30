
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

    elt.mouseenter(function () {
      elt.addClass('hover');
    });

    elt.mouseleave(function () {
      elt.removeClass('hover');
    });
  });
};

var button_submit = function (_form_elt, _elts) {

  $(_elts).each(function (_i, _elt) {

    var elt = $(_elt);

    elt.click(function () {
      $(_form_elt).submit();
    });
  });
};

jQuery(function ($) {

  resize_handler();
  button_hover('#save');
  textarea_focus('#key');
  button_submit('#form', '#save');
  $(window).resize(resize_handler);

});


