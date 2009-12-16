;;;
;;; DWG-Vorschau-Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung des DWG-Vorschau-Steuerelements und seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:ViewDwg (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "ViewDwg.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show ViewDWG_DwgPreview)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

;|<<OpenDCL Ereignisfunktionen>>|;

(defun c:DwgPreview_Cancel_Clicked ()
	(dcl_Form_Close ViewDWG_DwgPreview)
)

(defun c:DwgPreview_Browse_Clicked ( / )
	(setq sFileName (GetFiled "Zeichnungsdatei wählen" "" "dwg" 8)); wählt eine Zeichnungsdatei
	(if sFileName
		(progn
			(dcl_DwgPreview_LoadDwg ViewDWG_DwgPreview_ViewDwg sFileName); Lädt das DWG-Vorschaubild in die DWG-Vorschau
			(dcl_BlockView_LoadDwg ViewDWG_DwgPreview_BlockView1 sFileName); Lädt die DWG in die Blockvorschau
			(dcl_Control_SetCaption ViewDWG_DwgPreview_Label1 (dcl_DwgPreview_GetDwgName ViewDWG_DwgPreview_ViewDwg)); Gibt den Pfad der Datei aus
			(dcl_Control_SetFocus ViewDWG_DwgPreview_BlockView1)
		)
	)
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
	"ViewDwg"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
