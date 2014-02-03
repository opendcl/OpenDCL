;;;
;;; ListView und ListBox Drag&Drop Beispiel
;;;
;;; Dieses Beispiel demonstriert die Verwendung der Drag&Drop-Funktionen zwischen den Listen.
;;;

;; Hauptprogramm
(defun c:DragNDrop (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl-Project-Load (*ODCL:Samples-FindFile "DragNDrop.odcl"))

	;; Dialog anzeigen
	(dcl-Form-Show DragNDrop/Form1)

	;; Dies ist ein nicht-modaler Dialog. Das bedeutet, dass (dcl-Form-Show)
	;; sofort die Kontrolle zurückgibt und das Programm weiterläuft, während
	;; der Dialog aktiv ist.
	;; Die Ereignisfunktionen übernehmen erst nachher die Kontrolle und müssen
	;; daher global definiert sein.

	(princ)
)

;|«OpenDCL Event Handlers»|;

(defun c:DragNDrop/Form1/ListView1#OnDragnDropToAutoCAD (3dPoint Viewport / rValue)
	(Setq rValue (dcl-ListView-GetSelectedItems DragNDrop/Form1/ListView1))
	(dcl-MessageBox (strcat "Sie schoben \""
							(car rValue)
							"\" zum Punkt ("
							(rtos (car 3dPoint) 2 4)
							", "
							(rtos (cadr 3dPoint) 2 4)
							", "
							(rtos (caddr 3dPoint) 2 4)
							") im Ansichtsfenster "
							(itoa Viewport)
							"."
					)
					"Drag&Drop"
	)
	(princ)
)

(defun c:DragNDrop/Form1#OnInitialize (/)
	(dcl-ListView-AddColumns DragNDrop/Form1/ListView1 (list "Spalte 1" 0 140))
	(dcl-ListView-FillList DragNDrop/Form1/ListView1
						   '(("Element 1a" 0) ("Element 2a" 1) ("Element 3a" 2))
	)
	(princ)
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
	"DragNDrop"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
