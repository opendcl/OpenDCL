;;;
;;; HTML Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung des HTML-Steuerelements und seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:HTML (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "HTML.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show HTML_Dcl1)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:HTML_Dcl1_OnInitialize (/)               ; Zu einer neuen Webseite navigieren
	(dcl_Html_Navigate HTML_Dcl1_Html "http://www.opendcl.com")
)

;; Zur eingegebenen Webseite navigieren
(defun c:HTML_Dcl1_TB_Go_OnClicked (/ url)
	;; URL aus dem Textfeld auslesen
	(Setq url (dcl_Control_GetText HTML_Dcl1_TB_URL))
	;; Zur eingegebenen Webseite navigieren
	(dcl_Html_Navigate HTML_Dcl1_Html url)
)

(defun c:HTML_Dcl1_TB_GetDocument_OnClicked (/ htmlCode)
	;; HTML-Code des Dokuments auslesen
	(Setq htmlCode (dcl_Html_GetHtmlDocument HTML_Dcl1_Html))
	(princ htmlCode)
	(alert "HTML-Code wurde in die Befehlszeile geschrieben")
)

(defun c:HTML_Dcl1_TB-GetOffline_OnClicked (/ status info)
	;; Online-Status des Browsers prüfen
	(Setq status (dcl_Html_GetOffline HTML_Dcl1_Html)
		  info   (if status
					 "Browser OFFLINE"
					 "Browser ONLINE"
				 )                                ; T - offline, NIL - online
	)
	(dcl_MessageBox info "Online-Status" 0 2)
)

(defun c:HTML_Dcl1_TB-SetEnables_OnClicked ()
	;; Aktiviere HTML-Steuerelement
	(dcl_Control_SetEnabled HTML_Dcl1_Html T)
)

(defun c:HTML_Dcl1_TB-SetDisabled_OnClicked ()
	;; Deaktiviere HTML-Steuerelement
	(dcl_Control_SetEnabled HTML_Dcl1_Html nil)
)

(defun c:HTML_Dcl1_TB-GetFullName_OnClicked ()    ; Kompletten Namen auslesen
	(dcl_MessageBox (dcl_Html_GetFullName HTML_Dcl1_Html) "Kompletter Name" 0 2)
)

(defun c:HTML_Dcl1_TB-LoadingStatus_OnClicked (/ status info)
	;; Ladestatus
	(Setq status (dcl_Html_GetBusy HTML_Dcl1_Html)
		  info   (if status
					 "Seite wird geladen"
					 "------------------"
				 )
	)
	(dcl_MessageBox info "Status" 0 2)
)

(defun c:HTML_Dcl1_TB-PageTitle_OnClicked ()
	;; Titel der Webseite auslesen
	(dcl_MessageBox (dcl_Html_GetLocationName HTML_Dcl1_Html) "Webseiten-Titel" 0 2)
)

(defun c:HTML_Dcl1_RB-GoBack_OnClicked ()
	;; Zurück zur vorherigen Webseite
	(dcl_Html_GoBack HTML_Dcl1_Html)
)

(defun c:HTML_Dcl1_TB-Home_OnClicked ()
	;; Zur Startseite wechseln
	(dcl_Html_GoHome HTML_Dcl1_Html)
)

(defun c:HTML_Dcl1_TB-GetUrl_OnClicked ()
	;; URL der aktuellen Webseite ausgeben
	(dcl_MessageBox (dcl_Html_GetLocationURL HTML_Dcl1_Html) "URL" 0 2)
)

(defun c:HTML_Dcl1_TB-UpdateHtml_OnClicked ()
	;; Neuen HTML-Code zuweisen
	(dcl_Html_UpdateHtmlCode HTML_Dcl1_Html "<b>Neuer HTML-Code....</b>")
)

;; Auslösen, wenn die Webseite geladen ist
(defun c:HTML_Dcl1_Html_OnNavigationComplete (sUrl)
	(princ (strcat "\nSeietnaufbau abgeschlossen, URL: " sUrl))
)

(defun c:HTML_Dcl1_TB_Close_OnClicked ()
	(dcl_Form_Close HTML_Dcl1)                    ; Dialog schließen
)

(princ)

;|<<OpenDCL Samples Epilog>>|;

;;;######################################################################
;;;######################################################################
;;; Der folgende Abschnitt dient dazu, die Beispiel-Dateien zu lokalisieren.
;;; Die Pfadangabe wird um den Abschnitt des Beispielordner, erweitert, der
;;; durch das Installationsprogramm in der Registry eingetragen wurde.
;;; Die globalen Variable *ODCL:Prefix und die Function *ODCL:Samples:FindFile
;;; werden in allen Beispieldateien verwendet.
;;;
(or *ODCL:Samples:FindFile
	(defun *ODCL:Samples:FindFile (file)
		(setq *ODCL:Prefix
			(cond
				(	*ODCL:Prefix
				) ;_ Bereits definiert
				(	(vl-registry-read
						"HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
						"SamplesFolder"
					)
				) ;_ 32-bit Variante aktueller Nutzer
				(	(vl-registry-read
						"HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
						"SamplesFolder"
					 )
				) ;_ 32-bit Variante alle Nutzer
				(	(vl-registry-read
						"HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
						"SamplesFolder"
					)
				) ;_ 64-bit Variante aktueller Nutzer
				(	(vl-registry-read
						"HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
						"SamplesFolder"
					)
				) ;_ 64-bit Variante alle Nutzer
			)
		)
		(cond
			((findfile file)) ; überprüfe zunächst den Supportpfad
			(*ODCL:Prefix (findfile (strcat *ODCL:Prefix file)))
			(file)
		)
	)
)

;; Ist der Hauptdialog der OpenDCL-Beispiele aktiv, starte das Beispiel sofort.
;; Andernfalls gib einen Text in der Befehlszeile aus, mit welchem Kommando das Beispiel
;; gestartet werden kann. Auf diesem Wege wird sichergestellt, dass der Name des Beispiels
;; nur an einer Stelle definiert werden muss. Das macht es einfacher, den Code wiederzuverwenden.

(	(lambda (demoname)
		(if *ODCL:MasterDemo
			(progn
				(princ (strcat "'" demoname "\n"))
				(apply (read (strcat "C:" demoname)) nil)
			)
			(progn
				(princ (strcat "\n" demoname " OpenDCL-Beispiel ist geladen."))
				(princ (strcat " (Starten Sie das Beispiel mit dem Befehl " (strcase demoname) ")\n"))
			)
		)
	)
	"HTML"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
