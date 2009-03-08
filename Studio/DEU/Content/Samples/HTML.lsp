(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"HTML\" ein, um das Beispiel zu starten.\n")
)

(defun c:html (/ )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "HTML.odcl")
  (dcl_FORM_SHOW HTML_Dcl1)
  ;; The Event handlers manage the form here.   
  (princ)
)






(defun c:HTML_Dcl1_OnInitialize ( / )
;navigate to new url on HTML control
 (dcl_Html_Navigate HTML_Dcl1_Html "http://www.opendcl.com")
)



;Navigate to entred URL
(defun c:HTML_Dcl1_TB_Go_OnClicked ( / url )
;get url from textBox
(Setq url (dcl_Control_GetText HTML_Dcl1_TB_URL))
;navigate to new url on HTML control  
(dcl_Html_Navigate HTML_Dcl1_Html url)
)


(defun c:HTML_Dcl1_TB_GetDocument_OnClicked ( / htmlCode)
;Get HTML code of document  
(Setq htmlCode (dcl_Html_GetHtmlDocument HTML_Dcl1_Html))
(princ htmlCode)
(alert "Der HTML-Quelltext wurde in die Befehlszeile geschrieben.")
)


(defun c:HTML_Dcl1_TB-GetOffline_OnClicked ( / status info)
;Check browser status  
(Setq status (dcl_Html_GetOffline HTML_Dcl1_Html)
      info (if status "Browser OFFLINE" "Browser ONLINE") ; T - offline, NIL - online
      )
(dcl_MessageBox info "Browser-Status" 0 2)
)


(defun c:HTML_Dcl1_TB-SetEnables_OnClicked ()
;Set html control enabled  
(dcl_Control_SetEnabled HTML_Dcl1_Html T)
)


(defun c:HTML_Dcl1_TB-SetDisabled_OnClicked ()
;Set html control disabled  
(dcl_Control_SetEnabled HTML_Dcl1_Html nil)
)

(defun c:HTML_Dcl1_TB-GetFullName_OnClicked ()
;Get Full Name   
(dcl_MessageBox (dcl_Html_GetFullName HTML_Dcl1_Html) "Gesamter Name" 0 2)
)


(defun c:HTML_Dcl1_TB-LoadingStatus_OnClicked ( / status info)
;get loading status  
(Setq status (dcl_Html_GetBusy HTML_Dcl1_Html)
      info (if status "Webseite wird geladen" "-----------")
      )
(dcl_MessageBox info "Info" 0 2)  
)


(defun c:HTML_Dcl1_TB-PageTitle_OnClicked ()
; get title of website
(dcl_MessageBox (dcl_Html_GetLocationName HTML_Dcl1_Html) "Webseiten-Titel" 0 2)    
)

  
(defun c:HTML_Dcl1_RB-GoBack_OnClicked ()
; go back to previous website  
(dcl_Html_GoBack HTML_Dcl1_Html)     
)

(defun c:HTML_Dcl1_TB-Home_OnClicked ()
; navigate to home website  
(dcl_Html_GoHome HTML_Dcl1_Html)     
)


(defun c:HTML_Dcl1_TB-GetUrl_OnClicked ()
; get url of current loaded website
(dcl_MessageBox (dcl_Html_GetLocationURL HTML_Dcl1_Html) "URL" 0 2)    
)


(defun c:HTML_Dcl1_TB-UpdateHtml_OnClicked ()
 ; update html code  
 (dcl_Html_UpdateHtmlCode HTML_Dcl1_Html "<b>Neuer Quelltext....</b>")
)


; triger when website is loaded
(defun c:HTML_Dcl1_Html_OnNavigationComplete (sUrl)
(princ (strcat "\nNavigation abgeschlossen, URL: " sUrl))
)

; triger when mouse pointer is moving over the html control
;;;(defun c:HTML_Dcl1_Html_OnMouseMove ()
;;;(dcl_MessageBox "Mouse move over HTML control" "Info" 0 2)
;;;)


(defun c:HTML_Dcl1_TB_Close_OnClicked()
(dcl_Form_Close HTML_Dcl1)   ; close the form now
)

(princ)
;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
