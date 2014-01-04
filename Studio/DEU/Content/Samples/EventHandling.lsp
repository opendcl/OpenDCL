;;;
;;; Ereignis Beispiel
;;;
;;; Dieses Beispiel demonstriert die grundlegende Ereignisbehandlung
;;;

;; Hauptprogramm
(defun c:Events (/ cmdecho result)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl-Project-Load (*ODCL:Samples:FindFile "EventHandling.odcl") T)

	;; Dialog anzeigen
	(setq result (dcl-Form-Show EventHandling/Main))

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl-Form-Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.
	;; Der Rückgabewert von (dcl-Form-Show) wird verwendet, um die nächste Aktion
	;; zu bestimmen.

	(if (= result 10)
		(dcl-Form-Show EventHandling/Congrats)
	)

	(princ)
)

;; Den Anwender fragen, ob er abbrechen möchte
;; Gibt T zurück, wenn der Anwender Ja gedrückt hat; nil, wenn nicht
(defun EventHandling_PromptClose (/)
	(=
		6 ; 'Ja' Ergebnis
		(dcl-MessageBox
			"Sind Sie sicher, dass Sie abbrechen wollen?"
			"OpenDCL Beispiel"
			15 ; Ja/Nein mit 'Nein' als Vorgabe/Fokus
			3 ; Fragezeichen-Symbol
		)
	)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:EventHandling/Main#OnInitialize (/)
	(dcl-Control-SetText EventHandling/Main/txtPassword "")
	(princ)
)

(defun c:EventHandling/Main/txtPassword#OnEditChanged (sText /)
	(if (= (strcase sText) "TON")
		(dcl-Form-Close EventHandling/Main 10) ; 10 zurückgeben im Falle einer korrekten Antwort
	)
	(princ)
)

(defun c:EventHandling/Main/btnGiveUp#OnClicked ()
	(if (EventHandling_PromptClose)
		(dcl-Form-Close EventHandling/Main)
	)
	(princ)
)

(defun c:EventHandling/Main#OnCancelClose (Canceling /)
	;; Gibt die Ereignisfunktion den Wert T zurück, wird der Dialog nicht geschlossen
	(not (EventHandling_PromptClose))
)

(defun c:EventHandling/Congrats/btnExit#OnClicked ()
	(dcl-Form-Close EventHandling/Congrats)
	(princ)
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
	"Events"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
