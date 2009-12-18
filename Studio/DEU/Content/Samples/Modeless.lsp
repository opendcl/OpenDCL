;;;
;;; Beispiel einer nicht-modalen Dialogbox
;;;
;;; Dieses Beispiel demonstriert die Anwendung einer nicht-modalen Dialogbox und ihren Ereignissen.
;;;

;; Hauptprogramm
(defun c:Modeless (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "Modeless.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show Modeless_DemoModeless)

	;; Dies ist ein nicht-modaler Dialog. Das bedeutet, dass (dcl_Form_Show)
	;; sofort die Kontrolle zurückgibt und das Programm weiterläuft, während
	;; der Dialog aktiv ist.
	;; Die Ereignisfunktionen übernehmen erst nachher die Kontrolle und müssen
	;; daher global definiert sein.

	(princ)
)

;|<<OpenDCL Ereignisfunktionen>>|;

(defun c:DemoModeless_cmdDrawLine_OnClicked ()
	(command "_LINE" "0,0" "10,10" "")
	(command "_ZOOM" "_W" "-10,-10" "20,20")
	(princ)
)

(defun c:DemoModeless_cmdZoomWin_OnClicked ()
	(command "_ZOOM" "_W" PAUSE PAUSE)
	(princ)
)

(defun c:DemoModeless_cmdZoomExt_OnClicked ()
	(command "_ZOOM" "_E")
	(princ)
)

(defun c:DemoModeless_CloseButton_Clicked ()
	(dcl_Form_Close Modeless_DemoModeless)
	(princ)
)

(princ)

;|<<OpenDCL Beispiel Abschluss>>|;

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
	"Modeless"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
