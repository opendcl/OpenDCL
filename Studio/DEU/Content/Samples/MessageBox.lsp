;;;
;;; MessageBox Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung der Systeminteren MessageBox und seinen Rückgabewerten.
;;;

;; Hauptprogramm
(defun c:MsgBox (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl-Project-Load (*ODCL:Samples-FindFile "MessageBox.odcl"))

	;; Dialog anzeigen
	(dcl-Form-Show MessageBox/MessageBoxes)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl-Form-Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

;|«OpenDCL Event Handlers»|;

(defun c:MessageBox/MessageBoxes/ShowMessageBox#OnClicked
		(/ OptIconCSel OptButtonCSel msg title ButtonFlag IconFlag HelpFlag)
	(Setq OptIconCSel
			 (dcl-OptionList-GetCurSel MessageBox/MessageBoxes/optIcons)
	)
	(Setq OptButtonCSel (dcl-OptionList-GetCurSel
							MessageBox/MessageBoxes/optButtons
						)
	)
	(setq HelpFlag
			(= 1 (dcl-Control-GetValue MessageBox/MessageBoxes/HelpButton))
	)
	(Setq msg (dcl-Control-GetText MessageBox/MessageBoxes/MsgText))
	(Setq Title (dcl-Control-GetText MessageBox/MessageBoxes/MsgTitle))
	(dcl-MessageBox msg title (1+ OptButtonCSel) OptIconCSel HelpFlag)
)

(defun c:MessageBox/MessageBoxes/cmdClose#OnClicked ()
	(dcl-Form-Close MessageBox/MessageBoxes)
)

(defun c:MessageBox/MessageBoxes#OnHelp (/)
	(dcl-MessageBox
		(strcat
			"Sie haben die Hilfe zu diesem Dialog angefordert!\r\n\r\n"
			"Bitte beachten Sie, dass die [Hilfe]-Schaltfläche das OnHelp-Ereignis in der aufrufenden Dialogbox auslöst und die MessageBox am Bildschirm bleibt."
		)
		"MessageBox-Beispiel"
	)
)

(princ)

;|«OpenDCL Samples Epilog»|;

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
	"MsgBox"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
