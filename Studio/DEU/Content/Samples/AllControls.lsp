;; ALLCONTROLS
;; 2009-03-14
;; 
;; Zeigt eine OpenDCL Palette mit allen OpenDCL Steuerelementen
;;
;; Author: Owen Wengerd (owenw@manusoft.com)
;;
;;
;; Diese Datei wird als frei verfügbar bereitgestellt und kann weitergegeben oder für jeden 
;; anderen Zweck genutzt werden. Änderungen an dieser Datei müssen klar als solche erkennbar sein.
;; 
;;

(defun C:ALLCONTROLS (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl-Project-Load (*ODCL:Samples:FindFile "AllControls.odcl"))

	;; Dialog anzeigen
	(dcl-Form-Show AllControls/Form1)

	(princ)
)

(defun C:ALLCONTROLS2 (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden und Dialog anzeigen
	(and
		(dcl-Project-Load (*ODCL:Samples:FindFile "AllControls.odcl"))
		(not (dcl-Form-IsActive AllControls/Form2))
		(dcl-Form-show AllControls/Form2)
	)

	(princ)
)

(defun c:AllControls/Form1#OnInitialize (/ parent)
  (setq parent (dcl-Tree-AddParent AllControls/Form1/TreeControl1 "Baumstruktur" 0 -1 1))
  (dcl-Tree-AddChild AllControls/Form1/TreeControl1
    (list (list parent "Element 1" 2) (list parent "Element 2" 2) (list parent "Element 3" 2)))
  (dcl-Tree-AddParent AllControls/Form1/TreeControl1 '(("Element 2" 0 -1 1) ("Element 3" 0 -1 1)))
  (dcl-ListView-AddColumns AllControls/Form1/ListView1
    '(("Liste" 0 100) ("Spalte 2" 0 50) ("Spalte 3" 0 50)))
  (dcl-ListView-AddString AllControls/Form1/ListView1
    "Element 1\tElement  2\tElement  3")
  (dcl-Grid-AddString AllControls/Form1/Grid1
    "Element 1\tElement 2\tElement 3")
  (dcl-Animation-Load AllControls/Form1/Animation1 (*ODCL:Samples:FindFile "AllControls.avi"))
  (dcl-BlockView-PreLoadDwg AllControls/Form1/BlockView1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-BlockView-DisplayBlockToScale AllControls/Form1/BlockView1 "Block List")
  (dcl-SlideView-Load AllControls/Form1/SlideView1 (*ODCL:Samples:FindFile "AllControls.sld"))
  (dcl-Html-UpdateHtmlCode AllControls/Form1/Html1
    "<HTML><HEAD></HEAD><BODY><h4>Html</h4></BODY></HTML>")
  (dcl-DWGPreview-LoadDwg AllControls/Form1/DwgPreview1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-BlockList-LoadDwg AllControls/Form1/BlockList1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-Hatch-SetPattern AllControls/Form1/Hatch1 "LINE")
  (princ)
)

(defun c:AllControls/Form2#OnInitialize (/ parent)
  (setq parent (dcl-Tree-AddParent AllControls/Form2/TreeControl1 "Baumstruktur" 0 -1 1))
  (dcl-Tree-AddChild AllControls/Form2/TreeControl1
    (list (list parent "Element 1" 2) (list parent "Element 2" 2) (list parent "Element 3" 2)))
  (dcl-Tree-AddParent AllControls/Form2/TreeControl1 '(("Element 2" 0 -1 1) ("Element 3" 0 -1 1)))
  (dcl-ListView-AddColumns AllControls/Form2/ListView1
    '(("Liste" 0 100) ("Spalte 2" 0 50) ("Spalte 3" 0 50)))
  (dcl-ListView-AddString AllControls/Form2/ListView1
    "Element 1\tElement 2\tElement 3")
  (dcl-Grid-AddString AllControls/Form2/Grid1
    "Element 1\tElement 2\tElement 3")
  (dcl-Animation-Load AllControls/Form2/Animation1 (*ODCL:Samples:FindFile "AllControls.avi"))
  (dcl-BlockView-PreLoadDwg AllControls/Form2/BlockView1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-BlockView-DisplayBlockToScale AllControls/Form2/BlockView1 "Block List")
  (dcl-SlideView-Load AllControls/Form2/SlideView1 (*ODCL:Samples:FindFile "AllControls.sld"))
  (dcl-Html-UpdateHtmlCode AllControls/Form2/Html1
    "<HTML><HEAD></HEAD><BODY><h4>Html</h4></BODY></HTML>")
  (dcl-DWGPreview-LoadDwg AllControls/Form2/DwgPreview1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-BlockList-LoadDwg AllControls/Form2/BlockList1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-Hatch-SetPattern AllControls/Form2/Hatch1 "LINE")
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
	"AllControls"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
