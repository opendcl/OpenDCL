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
	(dcl-Project-Load (*ODCL:Samples:FindFile "Tree.odcl"))

	;; Dialog anzeigen
	(dcl-Form-Show Tree/Main)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl-Form-Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

(defun Tree_LogEvent (event)
	(if Tree/Main/lbxLog
		(dcl-ListBox-SetTopIndex Tree/Main/lbxLog (1- (dcl-ListBox-AddString Tree/Main/lbxLog event)))
	)
)

(defun Tree_IsTracking (/)
	(and Tree/Main/chkTrack (= 1 (dcl-Control-GetValue Tree/Main/chkTrack)))
)

;|<<OpenDCL Event Handlers>>|;

(defun c:Tree/Main#OnInitialize	(/ P1 P2)
	(setq P1 (dcl-Tree-AddParent Tree/Main/tree "Erste Gruppe" 0 -1 1))
	(dcl-Tree-AddChild Tree/Main/tree
		(list
			(list P1 "Element 1" 2 3)
			(list P1 "Element 2" 2 3)
			(list P1 "Element 3" 2 3)
		)
	)
	(setq P2 (dcl-Tree-AddParent Tree/Main/tree "Zweite Gruppe" 0 -1 8))
	(dcl-Tree-AddChild Tree/Main/tree
		(list
			(list P2 "Element C" 4 5)
			(list P2 "Element B" 4 5)
			(list P2 "Element A" 4 5)
		)
	)
	(dcl-Tree-AddParent Tree/Main/tree
		'(	("Dritte Gruppe" 0 -1 1)
			("Vierte Gruppe" 0 -1 1)
		)
	)
	(dcl-Tree-SelectItem Tree/Main/tree P1)
	(dcl-ListBox-Clear Tree/Main/lbxLog)
)


(defun c:Tree/Main/chkHasButtons#OnClicked (Value /)
	(dcl-Control-SetHasButtons Tree/Main/tree (= Value 1))
)


(defun c:Tree/Main/chkHasLines#OnClicked (Value /)
	(dcl-Control-SetHasLines Tree/Main/tree (= Value 1))
)


(defun c:Tree/Main/chkSingleClickExpand#OnClicked (Value /)
	(dcl-Control-SetSingleClickExpand Tree/Main/tree (= Value 1))
)


(defun c:Tree/Main/chkTrack#OnClicked (Value /)
	(setq CurSel (if (= 1 Value) (dcl-Tree-GetSelectedItem Tree/Main/tree)))
	(if CurSel
		(progn
			(dcl-Control-SetText Tree/Main/tbxLabel (dcl-Tree-GetItemLabel Tree/Main/tree CurSel))
			(setq Images (dcl-Tree-GetItemImages Tree/Main/tree CurSel))
			(dcl-ImageComboBox-SetCurSel Tree/Main/cbxImage (1+ (car Images)))
			(dcl-ImageComboBox-SetCurSel Tree/Main/cbxSelImage (1+ (cadr Images)))
			(dcl-ImageComboBox-SetCurSel Tree/Main/cbxExpImage (1+ (caddr Images)))
		)
	)
)


(defun c:Tree/Main/tbxLabel#OnEditChanged (NewValue / CurSel)
	(setq CurSel (if (Tree_IsTracking) (dcl-Tree-GetSelectedItem Tree/Main/tree)))
	(if CurSel
		(dcl-Tree-SetItemLabel Tree/Main/tree CurSel NewValue)
	)
)


(defun c:Tree/Main/cbxImage#OnSelChanged (ItemIndex Value / CurSel Images)
	(setq CurSel (if (Tree_IsTracking) (dcl-Tree-GetSelectedItem Tree/Main/tree)))
	(if CurSel
		(progn
			(setq Images (dcl-Tree-GetItemImages Tree/Main/tree CurSel))
			(dcl-Tree-SetItemImages Tree/Main/tree CurSel (1- ItemIndex) (cadr Images) (caddr Images))
		)
	)
)


(defun c:Tree/Main/cbxSelImage#OnSelChanged (ItemIndex Value / CurSel Images)
	(setq CurSel (if (Tree_IsTracking) (dcl-Tree-GetSelectedItem Tree/Main/tree)))
	(if CurSel
		(progn
			(setq Images (dcl-Tree-GetItemImages Tree/Main/tree CurSel))
			(dcl-Tree-SetItemImages Tree/Main/tree CurSel (car Images) (1- ItemIndex) (caddr Images))
		)
	)
)


(defun c:Tree/Main/cbxExpImage#OnSelChanged (ItemIndex Value / CurSel Images)
	(setq CurSel (if (Tree_IsTracking) (dcl-Tree-GetSelectedItem Tree/Main/tree)))
	(if CurSel
		(progn
			(setq Images (dcl-Tree-GetItemImages Tree/Main/tree CurSel))
			(dcl-Tree-SetItemImages Tree/Main/tree CurSel (car Images) (cadr Images) (1- ItemIndex))
		)
	)
)


(defun c:Tree/Main/btnAddParent#OnClicked (/)
	(dcl-Tree-AddParent Tree/Main/tree
		(dcl-Control-GetText Tree/Main/tbxLabel)
		(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxImage))
		(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxSelImage))
		(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxExpImage))
	)
)


(defun c:Tree/Main/btnAddChild#OnClicked (/ CurSel Images)
	(setq CurSel (dcl-Tree-GetSelectedItem Tree/Main/tree))
	(if CurSel
		(progn
			(setq Images (dcl-Tree-GetItemImages Tree/Main/tree CurSel))
			(dcl-Tree-AddChild Tree/Main/tree CurSel
				(dcl-Control-GetText Tree/Main/tbxLabel)
				(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxImage))
				(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxSelImage))
				(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxExpImage))
			)
		)
	)
)


(defun c:Tree/Main/btnAddSibling#OnClicked (/ CurSel Images)
	(setq CurSel (dcl-Tree-GetSelectedItem Tree/Main/tree))
	(if CurSel
		(progn
			(setq Images (dcl-Tree-GetItemImages Tree/Main/tree CurSel))
			(dcl-Tree-AddSibling Tree/Main/tree CurSel
				(dcl-Control-GetText Tree/Main/tbxLabel)
				(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxImage))
				(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxSelImage))
				(1- (dcl-ImageComboBox-GetCurSel Tree/Main/cbxExpImage))
			)
		)
	)
)


(defun c:Tree/Main/btnDelete#OnClicked (/ CurSel)
	(setq CurSel (dcl-Tree-GetSelectedItem Tree/Main/tree))
	(if CurSel
		(dcl-Tree-DeleteItem Tree/Main/tree CurSel)
	)
)


(defun c:Tree/Main/btnSortChildren#OnClicked (/ CurSel)
	(setq CurSel (dcl-Tree-GetSelectedItem Tree/Main/tree))
	(if CurSel
		(dcl-Tree-SortChildren Tree/Main/tree CurSel)
	)
)


(defun c:Tree/Main/btnExpand#OnClicked (/ CurSel)
	(setq CurSel (dcl-Tree-GetSelectedItem Tree/Main/tree))
	(if CurSel
		(dcl-Tree-ExpandItem Tree/Main/tree CurSel 0)
	)
)


(defun c:Tree/Main/btnEditLabel#OnClicked (/ CurSel)
	(setq CurSel (dcl-Tree-GetSelectedItem Tree/Main/tree))
	(if CurSel
		(dcl-Tree-StartLabelEdit Tree/Main/tree CurSel)
	)
)


(defun c:Tree/Main/btnClear#OnClicked (/)
	(dcl-Tree-Clear Tree/Main/tree)
)


(defun c:Tree/Main/tree#OnBeginLabelEdit (Key /)
	(Tree_LogEvent (strcat "OnBeginLabelEdit (" (dcl-Tree-GetItemLabel Tree/Main/tree Key) ")"))
)


(defun c:Tree/Main/tree#OnClicked (/)
	(Tree_LogEvent "OnClicked")
)


(defun c:Tree/Main/tree#OnDblClicked (/)
	(Tree_LogEvent "OnDblClicked")
)


(defun c:Tree/Main/tree#OnDeleteItem (Label Key /)
	(Tree_LogEvent (strcat "OnDeleteItem (" Label ")"))
)


(defun c:Tree/Main/tree#OnDragnDropBegin (/)
	(Tree_LogEvent "OnDragnDropBegin")
)


(defun c:Tree/Main/tree#OnDragnDropFromControl (ProjectName FormName ControlName DropPoint /)
	(Tree_LogEvent "OnDragnDropFromControl")
)


(defun c:Tree/Main/tree#OnDragnDropFromOther (DropPoint /)
	(Tree_LogEvent "OnDragnDropFromOther")
)


(defun c:Tree/Main/tree#OnDragnDropToAutoCAD (DropPoint Viewport /)
	(Tree_LogEvent "OnDragnDropToAutoCAD")
)


(defun c:Tree_Main_tree_OnDragOverFromControl (ProjectName FormName ControlName DropPoint /)
	(Tree_LogEvent "OnDragOverFromControl")
	1 ; return 1 to force copy
)


(defun c:Tree/Main/tree#OnEndLabelEdit (NewValue Key /)
	(Tree_LogEvent (strcat "OnEndLabelEdit (" NewValue ")"))
	(if (Tree_IsTracking)
		(dcl-Control-SetText Tree/Main/tbxLabel NewValue)
	)
)


(defun c:Tree/Main/tree#OnItemExpanded (Label Key /)
	(Tree_LogEvent
		(strcat
			"Element "
			(if (dcl-Tree-IsItemExpanded Tree/Main/tree Key) "erweitert" "zusammengeklappt")
			" ("
			Label
			")"
		)
	)
)


(defun c:Tree/Main/tree#OnItemExpanding (Label Key /)
	(Tree_LogEvent
		(strcat
			"Element "
			(if (dcl-Tree-IsItemExpanded Tree/Main/tree Key) "zusammenklappen" "erweitern")
			" ("
			Label
			")"
		)
	)
)


(defun c:Tree/Main/tree#OnKeyDown (Character RepeatCount Flags /)
	(Tree_LogEvent (strcat "OnKeyDown (" (itoa (ascii Character)) " " (itoa RepeatCount) " " (itoa Flags) ")"))
)


(defun c:Tree/Main/tree#OnKeyUp (Character RepeatCount Flags /)
	(Tree_LogEvent (strcat "OnKeyUp (" (itoa (ascii Character)) " " (itoa RepeatCount) " " (itoa Flags) ")"))
)


(defun c:Tree/Main/tree#OnKillFocus (/)
	(Tree_LogEvent "OnKillFocus")
)


(defun c:Tree/Main/tree#OnMouseMove (Flags X Y /)
	; wurde auskommentiert, um den Protokollierungsaufwand zu reduzieren
	NIL ;(Tree_LogEvent (strcat "OnMouseMove (" (itoa Flags) " " (itoa X) " " (itoa Y) ")"))
)


(defun c:Tree/Main/tree#OnReturnPressed (/)
	(Tree_LogEvent "OnReturnPressed")
)


(defun c:Tree/Main/tree#OnRightClick (/)
	(Tree_LogEvent "OnRightClick")
)


(defun c:Tree/Main/tree#OnRightDblClick (/)
	(Tree_LogEvent "OnRightDblClick")
)


(defun c:Tree/Main/tree#OnSelChanged (Label Key / Images)
	(Tree_LogEvent (strcat "OnSelChanged (" Label ")"))
	(if (Tree_IsTracking)
		(progn
			(dcl-Control-SetText Tree/Main/tbxLabel Label)
			(setq Images (dcl-Tree-GetItemImages Tree/Main/tree Key))
			(dcl-ImageComboBox-SetCurSel Tree/Main/cbxImage (1+ (car Images)))
			(dcl-ImageComboBox-SetCurSel Tree/Main/cbxSelImage (1+ (cadr Images)))
			(dcl-ImageComboBox-SetCurSel Tree/Main/cbxExpImage (1+ (caddr Images)))
		)
	)
)


(defun c:Tree/Main/tree#OnSetFocus (/)
	(Tree_LogEvent "OnSetFocus")
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
	"Tree"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
