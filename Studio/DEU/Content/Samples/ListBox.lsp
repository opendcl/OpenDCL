;;;
;;; Listenfeld Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung des Listenfelds und seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:ListBox (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "ListBox.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show listbox_form1)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

;;----------------------------------------------------
(DEFUN c:listbox_form1_selectfiles_onclicked (/ bs_filelist fn)
	(SETQ bs_filelist (dcl_multifiledialog "Zeichnungsdateien (*.dwg)|*.dwg||" ; Filter
										   "Dateien auswählen" ; Titel
										   (IF g:lastfolder
											   g:lastfolder
											   (GETVAR "DWGPREFIX")
										   ) ; Default Folder
					  )
	)
	(IF bs_filelist
		(PROGN
			(SETQ g:lastfolder (VL-FILENAME-DIRECTORY (CAR bs_filelist)))
			(FOREACH fn bs_filelist
				(IF (< (DCL_LISTBOX_FINDSTRINGEXACT ListBox_Form1_ListBox fn) 0)
					(DCL_LISTBOX_ADDSTRING ListBox_Form1_ListBox fn)
				)
			)
		)
	)
)
;;----------------------------------------------------
(DEFUN c:listbox_form1_clearlist_onclicked (/)
	(DCL_LISTBOX_CLEAR listbox_form1_listbox)
)
;;----------------------------------------------------
(DEFUN c:listbox_form1_remove_onclicked (/ rsel item)
	(IF (SETQ rsel (DCL_LISTBOX_GETSELECTEDNTHS ListBox_Form1_ListBox))
		(PROGN
			(FOREACH item (REVERSE rsel)
				(DCL_LISTBOX_DELETEITEM ListBox_Form1_ListBox item)
			)
			(SETQ g:bs_filelist '())
			(SETQ item (DCL_LISTBOX_GETCOUNT ListBox_Form1_ListBox))
			(WHILE (>= (SETQ item (1- item)) 0)
				(SETQ
					g:bs_filelist
					(CONS
						(DCL_LISTBOX_GETITEMTEXT ListBox_Form1_ListBox item)
						g:bs_filelist
					)
				)
			)
		)
	)
)
;;----------------------------------------------------
(DEFUN c:listbox_form1_exitdialog_onclicked (/) (DCL_FORM_CLOSE listbox_form1))

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
	"ListBox"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
