//JavaScript utlities used by the OpenDCL documentation files
//
// Copyright 2018 OpenDCL Consortium
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
