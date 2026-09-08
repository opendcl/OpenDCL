//JavaScript utlities used by the OpenDCL documentation files
//
// Copyright 2014 OpenDCL Consortium
// All Rights Reserved


// copy the contents of 'elem' to the clipboard
function copyclip( elem )
{ //need to use a temporary hidden textarea element to process the text
  var textToCopy = elem.innerText;
  var savedHTML = elem.innerHTML;
  elem.innerHTML = savedHTML + '<textarea id="clipboardtext" style="display:none;"></textarea>';
  clipboardtext.innerText = textToCopy;
  var range = clipboardtext.createTextRange();
  range.execCommand( "RemoveFormat" );
  range.execCommand( "Copy" );
  elem.innerHTML = savedHTML;
}

(function () {
  function setHdrlink() {
    var el = document.getElementById("hdrlink");
    if (!el) return;
    el.innerHTML =
      '<a href="https://www.opendcl.com/go?forum" target="_blank">\u0417\u0430\u0434\u0430\u0442\u044c \u0432\u043e\u043f\u0440\u043e\u0441</a><br />' +
      '<a href="https://www.opendcl.com/go?wishlist&new" target="_blank">\u041f\u043e\u0436\u0435\u043b\u0430\u043d\u0438\u044f</a><br />' +
      '<a href="https://www.opendcl.com/go?bugreport&new" target="_blank">\u0421\u043e\u043e\u0431\u0449\u0438\u0442\u044c \u043e \u0431\u0430\u0433\u0435</a>';
  }
  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", setHdrlink);
  } else {
    setHdrlink();
  }
})();
