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
	(dcl_Project_Load (*ODCL:Samples:FindFile "MessageBox.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show MessageBox_MessageBoxes)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:MessageBox_MessageBoxes_ShowMessageBox_OnClicked
		(/ OptIconCSel OptButtonCSel msg title ButtonFlag IconFlag HelpFlag)
	(Setq OptIconCSel
			 (dcl_OptionList_GetCurSel MessageBox_MessageBoxes_optIcons)
	)
	(Setq OptButtonCSel (dcl_OptionList_GetCurSel
							MessageBox_MessageBoxes_optButtons
						)
	)
	(setq HelpFlag
			(= 1 (dcl_Control_GetValue MessageBox_MessageBoxes_HelpButton))
	)
	(Setq msg (dcl_Control_GetText MessageBox_MessageBoxes_MsgText))
	(Setq Title (dcl_Control_GetText MessageBox_MessageBoxes_MsgTitle))
	(dcl_MessageBox msg title (1+ OptButtonCSel) OptIconCSel HelpFlag)
)

(defun c:MessageBox_MessageBoxes_cmdClose_OnClicked ()
	(dcl_Form_Close MessageBox_MessageBoxes)
)

(defun c:MessageBox_MessageBoxes_OnHelp (/)
	(dcl_MessageBox
		(strcat
			"Sie haben die Hilfe zu diesem Dialog angefordert!\r\n\r\n"
			"Bitte beachten Sie, dass die [Hilfe]-Schaltfläche das OnHelp-Ereignis in der aufrufenden Dialogbox auslöst und die MessageBox am Bildschirm bleibt."
		)
		"MessageBox-Beispiel"
	)
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
	"MsgBox"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
