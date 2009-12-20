;;;
;;; Schraffur Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung des Schraffur-Steuerelements und seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:Hatches (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "Hatches.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show Hatches_Hatch)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:Hatches_Hatch_OnInitialize (/ scale idx)
	(setq scale (rtos (dcl_Control_GetHatchScale Hatches_Hatch_Hatch1) 2 0))
	(setq idx (dcl_ComboBox_FindString Hatches_Hatch_ComboBox2 scale))
	(if (< idx 0) (setq idx 0))
	(dcl_ComboBox_SetCurSel Hatches_Hatch_ComboBox2 idx)
)

(defun c:Hatches_Hatch_ComboBox1_OnSelChanged (nSelection sSelText /)
	(dcl_hatch_setpattern Hatches_Hatch_Hatch1 sSelText)
	(dcl_hatch_setpattern Hatches_Hatch_Hatch2 sSelText)
)

(defun c:Hatches_Hatch_ComboBox2_OnSelChanged (nSelection sSelText / hatpat)
	;; Ausgewählten Skalierfaktor beiden Voransichten zuweisen
	(dcl_Control_SetHatchScale Hatches_Hatch_Hatch1 (atof sSelText))
	(dcl_Control_SetHatchScale Hatches_Hatch_Hatch2 (atof sSelText))
    
	;; Gewählten Schraffurnamen aus der Auswahlliste abfragen
	(setq hatpat (dcl_ComboBox_GetLBText
					 Hatches_Hatch_ComboBox1
					 (dcl_ComboBox_GetCurSel Hatches_Hatch_ComboBox1)
				 )
	) 
	;; Ereignisfunktion zur Aktualisierung der Voransichten ausführen
	(c:Hatches_Hatch_ComboBox1_OnSelChanged nil hatpat)
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
	"Hatches"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
