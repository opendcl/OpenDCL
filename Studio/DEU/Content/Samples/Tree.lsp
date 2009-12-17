;;;
;;; Tree Sample
;;;
;;; Dieses Beispiel demonstriert die Anwendung einer Baumstruktur und ihren Ereignissen.
;;;

;; Hauptprogramm
(defun c:Tree (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "Tree.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show Tree_Main)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

(defun Tree_LogEvent (event)
	(if Tree_Main_lbxLog
		(dcl_ListBox_SetTopIndex Tree_Main_lbxLog (1- (dcl_ListBox_AddString Tree_Main_lbxLog event)))
	)
)

(defun Tree_IsTracking (/)
	(and Tree_Main_chkTrack (= 1 (dcl_Control_GetValue Tree_Main_chkTrack)))
)

;|<<OpenDCL Ereignisfunktionen>>|;

(defun c:Tree_Main_OnInitialize	(/ P1 P2)
	(setq P1 (dcl_Tree_AddParent Tree_Main_tree "Erste Gruppe" 0 -1 1))
	(dcl_Tree_AddChild Tree_Main_tree
		(list
			(list P1 "Element 1" 2 3)
			(list P1 "Element 2" 2 3)
			(list P1 "Element 3" 2 3)
		)
	)
	(setq P2 (dcl_Tree_AddParent Tree_Main_tree "Zweite Gruppe" 0 -1 8))
	(dcl_Tree_AddChild Tree_Main_tree
		(list
			(list P2 "Element C" 4 5)
			(list P2 "Element B" 4 5)
			(list P2 "Element A" 4 5)
		)
	)
	(dcl_Tree_AddParent Tree_Main_tree
		'(	("Dritte Gruppe" 0 -1 1)
			("Vierte Gruppe" 0 -1 1)
		)
	)
	(dcl_Tree_SelectItem Tree_Main_tree P1)
	(dcl_ListBox_Clear Tree_Main_lbxLog)
)


(defun c:Tree_Main_chkHasButtons_OnClicked (Value /)
	(dcl_Control_SetHasButtons Tree_Main_tree (= Value 1))
)


(defun c:Tree_Main_chkHasLines_OnClicked (Value /)
	(dcl_Control_SetHasLines Tree_Main_tree (= Value 1))
)


(defun c:Tree_Main_chkSingleClickExpand_OnClicked (Value /)
	(dcl_Control_SetSingleClickExpand Tree_Main_tree (= Value 1))
)


(defun c:Tree_Main_chkTrack_OnClicked (Value /)
	(setq CurSel (if (= 1 Value) (dcl_Tree_GetSelectedItem Tree_Main_tree)))
	(if CurSel
		(progn
			(dcl_Control_SetText Tree_Main_tbxLabel (dcl_Tree_GetItemLabel Tree_Main_tree CurSel))
			(setq Images (dcl_Tree_GetItemImages Tree_Main_tree CurSel))
			(dcl_ImageComboBox_SetCurSel Tree_Main_cbxImage (1+ (car Images)))
			(dcl_ImageComboBox_SetCurSel Tree_Main_cbxSelImage (1+ (cadr Images)))
			(dcl_ImageComboBox_SetCurSel Tree_Main_cbxExpImage (1+ (caddr Images)))
		)
	)
)


(defun c:Tree_Main_tbxLabel_OnEditChanged (NewValue / CurSel)
	(setq CurSel (if (Tree_IsTracking) (dcl_Tree_GetSelectedItem Tree_Main_tree)))
	(if CurSel
		(dcl_Tree_SetItemLabel Tree_Main_tree CurSel NewValue)
	)
)


(defun c:Tree_Main_cbxImage_OnSelChanged (ItemIndex Value / CurSel Images)
	(setq CurSel (if (Tree_IsTracking) (dcl_Tree_GetSelectedItem Tree_Main_tree)))
	(if CurSel
		(progn
			(setq Images (dcl_Tree_GetItemImages Tree_Main_tree CurSel))
			(dcl_Tree_SetItemImages Tree_Main_tree CurSel (1- ItemIndex) (cadr Images) (caddr Images))
		)
	)
)


(defun c:Tree_Main_cbxSelImage_OnSelChanged (ItemIndex Value / CurSel Images)
	(setq CurSel (if (Tree_IsTracking) (dcl_Tree_GetSelectedItem Tree_Main_tree)))
	(if CurSel
		(progn
			(setq Images (dcl_Tree_GetItemImages Tree_Main_tree CurSel))
			(dcl_Tree_SetItemImages Tree_Main_tree CurSel (car Images) (1- ItemIndex) (caddr Images))
		)
	)
)


(defun c:Tree_Main_cbxExpImage_OnSelChanged (ItemIndex Value / CurSel Images)
	(setq CurSel (if (Tree_IsTracking) (dcl_Tree_GetSelectedItem Tree_Main_tree)))
	(if CurSel
		(progn
			(setq Images (dcl_Tree_GetItemImages Tree_Main_tree CurSel))
			(dcl_Tree_SetItemImages Tree_Main_tree CurSel (car Images) (cadr Images) (1- ItemIndex))
		)
	)
)


(defun c:Tree_Main_btnAddParent_OnClicked (/)
	(dcl_Tree_AddParent Tree_Main_tree
		(dcl_Control_GetText Tree_Main_tbxLabel)
		(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxImage))
		(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxSelImage))
		(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxExpImage))
	)
)


(defun c:Tree_Main_btnAddChild_OnClicked (/ CurSel Images)
	(setq CurSel (dcl_Tree_GetSelectedItem Tree_Main_tree))
	(if CurSel
		(progn
			(setq Images (dcl_Tree_GetItemImages Tree_Main_tree CurSel))
			(dcl_Tree_AddChild Tree_Main_tree CurSel
				(dcl_Control_GetText Tree_Main_tbxLabel)
				(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxImage))
				(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxSelImage))
				(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxExpImage))
			)
		)
	)
)


(defun c:Tree_Main_btnAddSibling_OnClicked (/ CurSel Images)
	(setq CurSel (dcl_Tree_GetSelectedItem Tree_Main_tree))
	(if CurSel
		(progn
			(setq Images (dcl_Tree_GetItemImages Tree_Main_tree CurSel))
			(dcl_Tree_AddSibling Tree_Main_tree CurSel
				(dcl_Control_GetText Tree_Main_tbxLabel)
				(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxImage))
				(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxSelImage))
				(1- (dcl_ImageComboBox_GetCurSel Tree_Main_cbxExpImage))
			)
		)
	)
)


(defun c:Tree_Main_btnDelete_OnClicked (/ CurSel)
	(setq CurSel (dcl_Tree_GetSelectedItem Tree_Main_tree))
	(if CurSel
		(dcl_Tree_DeleteItem Tree_Main_tree CurSel)
	)
)


(defun c:Tree_Main_btnSortChildren_OnClicked (/ CurSel)
	(setq CurSel (dcl_Tree_GetSelectedItem Tree_Main_tree))
	(if CurSel
		(dcl_Tree_SortChildren Tree_Main_tree CurSel)
	)
)


(defun c:Tree_Main_btnExpand_OnClicked (/ CurSel)
	(setq CurSel (dcl_Tree_GetSelectedItem Tree_Main_tree))
	(if CurSel
		(dcl_Tree_ExpandItem Tree_Main_tree CurSel 0)
	)
)


(defun c:Tree_Main_btnEditLabel_OnClicked (/ CurSel)
	(setq CurSel (dcl_Tree_GetSelectedItem Tree_Main_tree))
	(if CurSel
		(dcl_Tree_StartLabelEdit Tree_Main_tree CurSel)
	)
)


(defun c:Tree_Main_btnClear_OnClicked (/)
	(dcl_Tree_Clear Tree_Main_tree)
)


(defun c:Tree_Main_Tree_OnBeginLabelEdit (Key /)
	(Tree_LogEvent (strcat "OnBeginLabelEdit (" (dcl_Tree_GetItemLabel Tree_Main_tree Key) ")"))
)


(defun c:Tree_Main_Tree_OnClicked (/)
	(Tree_LogEvent "OnClicked")
)


(defun c:Tree_Main_Tree_OnDblClicked (/)
	(Tree_LogEvent "OnDblClicked")
)


(defun c:Tree_Main_Tree_OnDeleteItem (Label Key /)
	(Tree_LogEvent (strcat "OnDeleteItem (" Label ")"))
)


(defun c:Tree_Main_Tree_OnDragnDropBegin (/)
	(Tree_LogEvent "OnDragnDropBegin")
)


(defun c:Tree_Main_Tree_OnDragnDropFromControl (ProjectName FormName ControlName DropPoint /)
	(Tree_LogEvent "OnDragnDropFromControl")
)


(defun c:Tree_Main_Tree_OnDragnDropFromOther (DropPoint /)
	(Tree_LogEvent "OnDragnDropFromOther")
)


(defun c:Tree_Main_Tree_OnDragnDropToAutoCAD (DropPoint Viewport /)
	(Tree_LogEvent "OnDragnDropToAutoCAD")
)


(defun c:Tree_Main_tree_OnDragOverFromControl (ProjectName FormName ControlName DropPoint /)
	(Tree_LogEvent "OnDragOverFromControl")
	1 ; return 1 to force copy
)


(defun c:Tree_Main_Tree_OnEndLabelEdit (NewValue Key /)
	(Tree_LogEvent (strcat "OnEndLabelEdit (" NewValue ")"))
	(if (Tree_IsTracking)
		(dcl_Control_SetText Tree_Main_tbxLabel NewValue)
	)
)


(defun c:Tree_Main_Tree_OnItemExpanded (Label Key /)
	(Tree_LogEvent
		(strcat
			"Element "
			(if (dcl_Tree_IsItemExpanded Tree_Main_tree Key) "erweitert" "zusammengeklappt")
			" ("
			Label
			")"
		)
	)
)


(defun c:Tree_Main_Tree_OnItemExpanding (Label Key /)
	(Tree_LogEvent
		(strcat
			"Element "
			(if (dcl_Tree_IsItemExpanded Tree_Main_tree Key) "zusammenklappen" "erweitern")
			" ("
			Label
			")"
		)
	)
)


(defun c:Tree_Main_Tree_OnKeyDown (Character RepeatCount Flags /)
	(Tree_LogEvent (strcat "OnKeyDown (" (itoa (ascii Character)) " " (itoa RepeatCount) " " (itoa Flags) ")"))
)


(defun c:Tree_Main_Tree_OnKeyUp (Character RepeatCount Flags /)
	(Tree_LogEvent (strcat "OnKeyUp (" (itoa (ascii Character)) " " (itoa RepeatCount) " " (itoa Flags) ")"))
)


(defun c:Tree_Main_Tree_OnKillFocus (/)
	(Tree_LogEvent "OnKillFocus")
)


(defun c:Tree_Main_Tree_OnMouseMove (Flags X Y /)
	; wurde auskommentiert, um den Protokollierungsaufwand zu reduzieren
	NIL ;(Tree_LogEvent (strcat "OnMouseMove (" (itoa Flags) " " (itoa X) " " (itoa Y) ")"))
)


(defun c:Tree_Main_Tree_OnReturnPressed (/)
	(Tree_LogEvent "OnReturnPressed")
)


(defun c:Tree_Main_Tree_OnRightClick (/)
	(Tree_LogEvent "OnRightClick")
)


(defun c:Tree_Main_Tree_OnRightDblClick (/)
	(Tree_LogEvent "OnRightDblClick")
)


(defun c:Tree_Main_Tree_OnSelChanged (Label Key / Images)
	(Tree_LogEvent (strcat "OnSelChanged (" Label ")"))
	(if (Tree_IsTracking)
		(progn
			(dcl_Control_SetText Tree_Main_tbxLabel Label)
			(setq Images (dcl_Tree_GetItemImages Tree_Main_tree Key))
			(dcl_ImageComboBox_SetCurSel Tree_Main_cbxImage (1+ (car Images)))
			(dcl_ImageComboBox_SetCurSel Tree_Main_cbxSelImage (1+ (cadr Images)))
			(dcl_ImageComboBox_SetCurSel Tree_Main_cbxExpImage (1+ (caddr Images)))
		)
	)
)


(defun c:Tree_Main_Tree_OnSetFocus (/)
	(Tree_LogEvent "OnSetFocus")
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
	"Tree"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
