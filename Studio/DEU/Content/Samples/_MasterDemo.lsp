;;;######################################################################
;;;
;;;OpenDCL Beispiel: MasterDemo
;;;
;;; Dieses Beispiel zeigt einen Dialog zum Start der einzelnen OpenDCL Beispiel.
;;;

;; Hauptprogramm
(DEFUN c:OpenDCLDemo (/ *error*)
	(DEFUN *error* (msg)
		(WHILE (< 0 (GETVAR "cmdactive")) (COMMAND))
		;; do error stuff
		(IF (DCL_FORM_ISACTIVE _MasterDemo_Main)
			(DCL_FORM_CLOSE _MasterDemo_Main)
		)
		(PRINC
			(STRCAT "\nAnwendungsfehler: " (ITOA (GETVAR "errno")) " :- " msg)
		)
		(PRINC)
	)
	;;------------------------

	;; Stellt sicher, dass die OpenDCL Laufzeitumgebung geladen ist (ohne Textausgabe an der Befehlszeile)
	(SETQ cmdecho (GETVAR "CMDECHO"))
	(SETVAR "CMDECHO" 0)
	(COMMAND "_OPENDCL")
	(SETVAR "CMDECHO" cmdecho)

	;; Lädt das Projekt
	(DCL_PROJECT_LOAD (*ODCL:Samples:FindFile "_MasterDemo.odcl"))

	;; Zeig den Dialog an.
	(DCL_FORM_SHOW _MasterDemo_Main)
	;; Die Ereignisfunktionen steuern an dieser Stelle den Dialog.
	(PRINC)
)

;; Lade das spezielle Beispiel
(DEFUN *ODCL:RunSample (filename)
	(SETQ *ODCL:MasterDemo T) ; definiert, dass das Beispiel aus dem Masterdemo geladen wurde
	(IF (NOT (LOAD (*ODCL:Samples:FindFile filename) NIL))
		((ALERT (STRCAT "\"" filename "\" konnte nicht geladen werden!")))
	)
	(SETQ *ODCL:MasterDemo nil)
)


;|<<OpenDCL Event Handlers>>|;

(DEFUN c:_MasterDemo_Main_btnMisc_OnClicked ()
	(*ODCL:RunSample "Misc.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnMethods_OnClicked ()
	(*ODCL:RunSample "Methods.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnEventHandling_OnClicked ()
	(*ODCL:RunSample "EventHandling.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnMessageBox_OnClicked ()
	(*ODCL:RunSample "MessageBox.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnTree_OnClicked ()
	(*ODCL:RunSample "Tree.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnViewDwg_OnClicked ()
	(*ODCL:RunSample "ViewDwg.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnModeless_OnClicked ()
	(*ODCL:RunSample "Modeless.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnOptionsTab_OnClicked ()
	(*ODCL:RunSample "OptionsTab.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnHTML_OnClicked ()
	(*ODCL:RunSample "HTML.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnGrid_OnClicked ()
	(*ODCL:RunSample "GRID.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnAnimation_OnClicked ()
	(*ODCL:RunSample "Animation.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnHatches_OnClicked ()
	(*ODCL:RunSample "Hatches.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnToolTip_OnClicked ()
	(*ODCL:RunSample "ToolTip.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnDragNDrop_OnClicked ()
	(*ODCL:RunSample "DragNDrop.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnSelections_OnClicked ()
	(*ODCL:RunSample "Selections.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnListView_OnClicked ()
	(*ODCL:RunSample "ListView.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnListBoxCopyPaste_OnClicked ()
	(*ODCL:RunSample "ListBoxCopyPaste.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnSplitter_OnClicked ()
	(*ODCL:RunSample "Splitter.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnDwgList_OnClicked ()
	(*ODCL:RunSample "DwgList.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnListBox_OnClicked ()
	(*ODCL:RunSample "ListBox.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnDistSample_OnClicked (/ readme)
	(SETQ readme (*ODCL:Samples:FindFile "DistSampleReadMe.txt"))
	(IF readme
		(STARTAPP "notepad" readme)
		(ALERT (STRCAT "Cant find \"DistSampleReadMe.txt\"!"))
	)
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnSplash_OnClicked (/)
	(*ODCL:RunSample "Splash.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnFormMover_OnClicked (/)
	(*ODCL:RunSample "FormMover.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnSlider_OnClicked (/)
	(*ODCL:RunSample "Slider.lsp")
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnReadme_OnClicked (/ readme)
	(SETQ readme (*ODCL:Samples:FindFile "_ReadME.txt"))
	(IF readme
		(STARTAPP "notepad" readme)
		(ALERT (STRCAT "Cant find \"_ReadME.txt\"!"))
	)
	(PRINC)
)

(DEFUN c:_MasterDemo_Main_btnUpdateCheck_OnClicked ()
	(DCL_UPDATECHECK)
	(PRINC)
)

(princ)

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

(princ "\nOPENDCLDEMO\n")
(C:OpenDCLDemo)

;;;######################################################################
;;;######################################################################

 ;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 2 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
