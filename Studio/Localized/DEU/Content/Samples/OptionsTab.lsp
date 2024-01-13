;;;
;;; Beispiel einer Karteikarte in den Optionen
;;;
;;; Dieses Beispiel demonstriert die Anwendung einer zusätzlichen Karteikarte in den Optionen und ihren Ereignissen.
;;;

;; Hauptprogramm
(defun c:OptionsTab (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl-Project-Load (*ODCL:Samples-FindFile "OptionsTab.odcl"))

	;; Karteikarten in den Optionen werden registriert, wenn das Projekt geladen wird.
	;; Alle registrierten zusätzlichen Karteikarten werden automatisch angezeigt, wenn
	;; die AutoCAD-Optionen aufgerufen werden.
	(dcl-messagebox "Aktivieren Sie die Karteikarte \"OpenDCL Beispiel\"!" "OpenDCL - Zusätzliche Karteikarten in den Optionen")
	(command "._OPTIONS") ; Startet die AutoCAD-Optionen

	;; Dies ist ein nicht-modaler Dialog. Das bedeutet, dass (dcl-Form-Show)
	;; sofort die Kontrolle zurückgibt und das Programm weiterläuft, während
	;; der Dialog aktiv ist.
	;; Die Ereignisfunktionen übernehmen erst nachher die Kontrolle und müssen
	;; daher global definiert sein.

	(princ)
)

;|*OpenDCL Event Handlers*|;

(defun c:OptionsTab_OptionsTabDemo_OnOK (/)
	(princ "\n[OpenDCL - Zusätzliche Karteikarten in den Optionen] Ihre Eingabe: \"")
	(princ (dcl-Control-GetText OptionsTab/OptionsTabDemo/txtOptTabDemo))
	(princ "\"; Status des Kontrollkästchens ")
	(if (eq 1 (dcl-Control-GetValue OptionsTab/OptionsTabDemo/chkOptTabDemo))
		(princ "EIN")
		(princ "AUS")
	)
	(princ "\n")
	(princ)
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
	"OptionsTab"
)
(princ)

;;;######################################################################
;;;######################################################################
