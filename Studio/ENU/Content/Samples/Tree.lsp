;;;
;;; Tree Sample
;;;
;;; This sample demonstrates the tree control and all its events.
;;;

;; Main program
(defun c:Tree (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl-Project-Load (*ODCL:Samples:FindFile "Tree.odcl"))

	;; Show the main form
	(dcl-Form-Show Tree/Main)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

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
	(setq P1 (dcl-Tree-AddParent Tree/Main/tree "First Parent" 0 -1 1))
	(dcl-Tree-AddChild Tree/Main/tree
		(list
			(list P1 "Child 1" 2 3)
			(list P1 "Child 2" 2 3)
			(list P1 "Child 3" 2 3)
		)
	)
	(setq P2 (dcl-Tree-AddParent Tree/Main/tree "Second Parent" 0 -1 8))
	(dcl-Tree-AddChild Tree/Main/tree
		(list
			(list P2 "Child C" 4 5)
			(list P2 "Child B" 4 5)
			(list P2 "Child A" 4 5)
		)
	)
	(dcl-Tree-AddParent Tree/Main/tree
		'(	("Third Parent" 0 -1 1)
			("Fourth Parent" 0 -1 1)
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
			"OnItem"
			(if (dcl-Tree-IsItemExpanded Tree/Main/tree Key) "Expanded" "Collapsed")
			" ("
			Label
			")"
		)
	)
)


(defun c:Tree/Main/tree#OnItemExpanding (Label Key /)
	(Tree_LogEvent
		(strcat
			"OnItem"
			(if (dcl-Tree-IsItemExpanded Tree/Main/tree Key) "Collapsing" "Expanding")
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
	; Commented out to reduce log traffic
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
;;; The following section of code is designed to locate OpenDCL Studio
;;; sample files in the samples folder by prefixing the filename with
;;; the path prefix that was saved in the registry by the installer.
;;; The global *ODCL:Prefix and function *ODCL:Samples:FindFile
;;; are used throughout the samples.
;;;
(or *ODCL:Samples:FindFile
	(defun *ODCL:Samples:FindFile (file)
		(setq *ODCL:Prefix
			(cond
				(	*ODCL:Prefix
				) ;_ already defined
				(	(vl-registry-read
						 "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 32-bit location
				(	(vl-registry-read
						 "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 32-bit location
				(	(vl-registry-read
						 "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 64-bit location
				(	(vl-registry-read
						 "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 64-bit location
			)
		)
		(cond
			((findfile file)) ; check the support path first
			(*ODCL:Prefix (findfile (strcat *ODCL:Prefix file)))
			(file)
		)
	)
)

;; If master demo is active, run the main function immediately; otherwise
;; display a banner. The extra gymnastics allow the sample name to be
;; specified in only one place, thus making it easier to reuse this code.
(	(lambda (demoname)
		(if *ODCL:MasterDemo
			(progn
				(princ (strcat "'" demoname "\n"))
				(apply (read (strcat "C:" demoname)) nil)
			)
			(progn
				(princ (strcat "\n" demoname " OpenDCL sample loaded"))
				(princ (strcat " (Enter " (strcase demoname) " command to run)\n"))
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
