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
      '<a href="https://www.opendcl.com/go?forum" target="_blank">Frage stellen</a><br />' +
      '<a href="https://www.opendcl.com/go?wishlist&new" target="_blank">Wunschliste</a><br />' +
      '<a href="https://www.opendcl.com/go?bugreport&new" target="_blank">Fehler melden</a>';
  }
  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", setHdrlink);
  } else {
    setHdrlink();
  }
})();
