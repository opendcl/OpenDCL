(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"HTML\" ein, um das Beispiel zu starten.\n")
)

(defun c:html (/ )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "HTML.odcl")
  
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog.   
  (dcl_FORM_SHOW HTML_Dcl1)
    
  (princ)
)






(defun c:HTML_Dcl1_OnInitialize ( / )
; Vorgegebene Webseite laden
 (dcl_Html_Navigate HTML_Dcl1_Html "http://www.opendcl.com")
)



; Angegebene Webseite laden
(defun c:HTML_Dcl1_TB_Go_OnClicked ( / url )
; URL aus der Textbox holen
(Setq url (dcl_Control_GetText HTML_Dcl1_TB_URL))
; Neue angegebene Webseite laden
(dcl_Html_Navigate HTML_Dcl1_Html url)
)


(defun c:HTML_Dcl1_TB_GetDocument_OnClicked ( / htmlCode)
; HTML-Code der Webseite auslesen
(Setq htmlCode (dcl_Html_GetHtmlDocument HTML_Dcl1_Html))
(princ htmlCode)
(alert "Der HTML-Quelltext wurde in die Befehlszeile geschrieben.")
)


(defun c:HTML_Dcl1_TB-GetOffline_OnClicked ( / status info)
; Broswer-Status prüfen
(Setq status (dcl_Html_GetOffline HTML_Dcl1_Html)
      info (if status "Browser OFFLINE" "Browser ONLINE") ; T - offline, NIL - online
      )
(dcl_MessageBox info "Browser-Status" 0 2)
)


(defun c:HTML_Dcl1_TB-SetEnables_OnClicked ()
; HTML-Steuerelement aktivieren
(dcl_Control_SetEnabled HTML_Dcl1_Html T)
)


(defun c:HTML_Dcl1_TB-SetDisabled_OnClicked ()
; HTML-Steuerelement deaktivieren
(dcl_Control_SetEnabled HTML_Dcl1_Html nil)
)

(defun c:HTML_Dcl1_TB-GetFullName_OnClicked ()
; Browserbezeichnung ausgeben
(dcl_MessageBox (dcl_Html_GetFullName HTML_Dcl1_Html) "Gesamter Name" 0 2)
)


(defun c:HTML_Dcl1_TB-LoadingStatus_OnClicked ( / status info)
; Prüft, ob die Webseite bereits vollständig geladen ist
(Setq status (dcl_Html_GetBusy HTML_Dcl1_Html)
      info (if status "Webseite wird geladen" "-----------")
      )
(dcl_MessageBox info "Info" 0 2)  
)


(defun c:HTML_Dcl1_TB-PageTitle_OnClicked ()
; Webseitentitel
(dcl_MessageBox (dcl_Html_GetLocationName HTML_Dcl1_Html) "Webseiten-Titel" 0 2)    
)

  
(defun c:HTML_Dcl1_RB-GoBack_OnClicked ()
; zur vorherigen Webseite wechseln
(dcl_Html_GoBack HTML_Dcl1_Html)     
)

(defun c:HTML_Dcl1_TB-Home_OnClicked ()
; Zur Staretseite wechseln
(dcl_Html_GoHome HTML_Dcl1_Html)     
)


(defun c:HTML_Dcl1_TB-GetUrl_OnClicked ()
; URL der aktuell geladenen Webseite wechseln
(dcl_MessageBox (dcl_Html_GetLocationURL HTML_Dcl1_Html) "URL" 0 2)    
)


(defun c:HTML_Dcl1_TB-UpdateHtml_OnClicked ()
 ; HTML-Code aktualisiseren
 (dcl_Html_UpdateHtmlCode HTML_Dcl1_Html "<b>Neuer Quelltext....</b>")
)


; Wird ausgelöst, wenn die Navigation abgeschlossen ist
(defun c:HTML_Dcl1_Html_OnNavigationComplete (sUrl)
(princ (strcat "\nNavigation abgeschlossen, URL: " sUrl))
)

(defun c:HTML_Dcl1_TB_Close_OnClicked()
(dcl_Form_Close HTML_Dcl1)   ; Dialog schließen
)

(princ)
;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
