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
	(dcl-Project-Load (*ODCL:Samples-FindFile "HTML.odcl"))

	;; Dialog anzeigen
	(dcl-Form-Show HTML/Dcl1)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl-Form-Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

;|*OpenDCL Event Handlers*|;

(defun c:HTML/Dcl1#OnInitialize (/)               ; Zu einer neuen Webseite navigieren
	(dcl-Html-Navigate HTML/Dcl1/Html "http://www.opendcl.com")
)

;; Zur eingegebenen Webseite navigieren
(defun c:HTML/Dcl1/TB_Go#OnClicked (/ url)
	;; URL aus dem Textfeld auslesen
	(Setq url (dcl-Control-GetText HTML/Dcl1/TB_URL))
	;; Zur eingegebenen Webseite navigieren
	(dcl-Html-Navigate HTML/Dcl1/Html url)
)

(defun c:HTML/Dcl1/TB_GetDocument#OnClicked (/ htmlCode)
	;; HTML-Code des Dokuments auslesen
	(Setq htmlCode (dcl-Html-GetHtmlDocument HTML/Dcl1/Html))
	(princ htmlCode)
	(alert "HTML-Code wurde in die Befehlszeile geschrieben")
)

(defun c:HTML/Dcl1/TB-GetOffline#OnClicked (/ status info)
	;; Online-Status des Browsers prüfen
	(Setq status (dcl-Html-GetOffline HTML/Dcl1/Html)
		  info   (if status
					 "Browser OFFLINE"
					 "Browser ONLINE"
				 )                                ; T - offline, NIL - online
	)
	(dcl-MessageBox info "Online-Status" 0 2)
)

(defun c:HTML/Dcl1/TB-SetEnables#OnClicked ()
	;; Aktiviere HTML-Steuerelement
	(dcl-Control-SetEnabled HTML/Dcl1/Html T)
)

(defun c:HTML/Dcl1/TB-SetDisabled#OnClicked ()
	;; Deaktiviere HTML-Steuerelement
	(dcl-Control-SetEnabled HTML/Dcl1/Html nil)
)

(defun c:HTML/Dcl1/TB-GetFullName#OnClicked ()    ; Kompletten Namen auslesen
	(dcl-MessageBox (dcl-Html-GetFullName HTML/Dcl1/Html) "Kompletter Name" 0 2)
)

(defun c:HTML/Dcl1/TB-LoadingStatus#OnClicked (/ status info)
	;; Ladestatus
	(Setq status (dcl-Html-GetBusy HTML/Dcl1/Html)
		  info   (if status
					 "Seite wird geladen"
					 "------------------"
				 )
	)
	(dcl-MessageBox info "Status" 0 2)
)

(defun c:HTML/Dcl1/TB-PageTitle#OnClicked ()
	;; Titel der Webseite auslesen
	(dcl-MessageBox (dcl-Html-GetLocationName HTML/Dcl1/Html) "Webseiten-Titel" 0 2)
)

(defun c:HTML/Dcl1/RB-GoBack#OnClicked ()
	;; Zurück zur vorherigen Webseite
	(dcl-Html-GoBack HTML/Dcl1/Html)
)

(defun c:HTML/Dcl1/TB-Home#OnClicked ()
	;; Zur Startseite wechseln
	(dcl-Html-GoHome HTML/Dcl1/Html)
)

(defun c:HTML/Dcl1/TB-GetUrl#OnClicked ()
	;; URL der aktuellen Webseite ausgeben
	(dcl-MessageBox (dcl-Html-GetLocationURL HTML/Dcl1/Html) "URL" 0 2)
)

(defun c:HTML/Dcl1/TB-UpdateHtml#OnClicked ()
	;; Neuen HTML-Code zuweisen
	(dcl-Html-UpdateHtmlCode HTML/Dcl1/Html "<b>Neuer HTML-Code....</b>")
)

;; Auslösen, wenn die Webseite geladen ist
(defun c:HTML/Dcl1/Html#OnNavigationComplete (sUrl)
	(princ (strcat "\nSeietnaufbau abgeschlossen, URL: " sUrl))
)

(defun c:HTML/Dcl1/TB_Close#OnClicked ()
	(dcl-Form-Close HTML/Dcl1)                    ; Dialog schließen
)

(princ)

;|*OpenDCL Samples Epilog*|;

;;;######################################################################
;;;######################################################################
;;; Der folgende Abschnitt dient dazu, die Beispiel-Dateien zu lokalisieren.
;;; Die Pfadangabe wird um den Abschnitt des Beispielordner, erweitert, der
;;; durch das Installationsprogramm in der Registry eingetragen wurde.
;;; Die globalen Variable *ODCL:Prefix und die Function *ODCL:Samples-FindFile
;;; werden in allen Beispieldateien verwendet.
;;;
(or *ODCL:Samples-FindFile
	(defun *ODCL:Samples-FindFile (file)
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
		(if *ODCL:AllSamples
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
