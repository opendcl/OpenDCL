;;;
;;; DWG-Liste Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung des Steuerlements DWG-Liste mit seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:DwgList (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "DwgList.odcl") T)

	;; Dialog anzeigen
	(dcl_Form_Show DwgList_DwgListForm)

	;; Dies ist ein nicht-modaler Dialog. Das bedeutet, dass (dcl_Form_Show)
	;; sofort die Kontrolle zurückgibt und das Programm weiterläuft, während
	;; der Dialog aktiv ist.
	;; Die Ereignisfunktionen übernehmen erst nachher die Kontrolle und müssen
	;; daher global definiert sein.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:DwgListForm_DwgList1_OnDblClicked ()
	(if (and (Setq fileName (dcl_DwgList_GetFileName DwgList_DwgListForm_DwgList1))
                 (equal (strcase (substr fileName (- (strlen fileName) 3) 4)) ".DWG"))
		(progn                                    ;(dcl_SetCmdBarFocus)
			(dcl_sendstring (strcat "_-INSERT " fileName "\n"))
		)
	)
	(princ)
)

(defun c:DwgList_DwgListForm_cmdUP_OnClicked (/)
	(setq sPath (strcat (dcl_ComboBox_GetDir DwgList_DwgListForm_ComboBox1)
						"\\.."
				)
	)
	(dcl_ComboBox_Dir DwgList_DwgListForm_ComboBox1 sPath)
	(dcl_DwgList_Dir DwgList_DwgListForm_DwgList1 sPath)
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
	"DwgList"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
