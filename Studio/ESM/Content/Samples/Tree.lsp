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
    (dcl_Project_Load (*ODCL:Samples:FindFile "Tree.odcl"))

    ;; Show the main form
    (dcl_Form_Show Tree_Main)

    ;; This is a modal form, so (dcl_Form_Show) does not return until
    ;; the modal form is closed. In the meantime, the event handlers
    ;; manage the form.

    (princ)
)

(defun Tree_LogEvent (event)
  (if Tree_Main_lbxLog
    (dcl_ListBox_SetTopIndex Tree_Main_lbxLog (dcl_ListBox_AddString Tree_Main_lbxLog event))
  )
)

(defun Tree_IsTracking (/)
  (and Tree_Main_chkTrack (= 1 (dcl_Control_GetValue Tree_Main_chkTrack)))
)

;|<<OpenDCL Event Handlers>>|;

(defun c:Tree_Main_OnInitialize	(/ P1 P2)
  (setq P1 (dcl_Tree_AddParent Tree_Main_tree "First Parent" 0 -1 1))
  (dcl_Tree_AddChild Tree_Main_tree
    (list
      (list P1 "Child 1" 2 3)
      (list P1 "Child 2" 2 3)
      (list P1 "Child 3" 2 3)
    )
  )
  (setq P2 (dcl_Tree_AddParent Tree_Main_tree "Second Parent" 0 -1 8))
  (dcl_Tree_AddChild Tree_Main_tree
    (list
      (list P2 "Child C" 4 5)
      (list P2 "Child B" 4 5)
      (list P2 "Child A" 4 5)
    )
  )
  (dcl_Tree_AddParent Tree_Main_tree
    '(("Third Parent" 0 -1 1)
      ("Fourth Parent" 0 -1 1)
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


(defun c:Tree_Main_Tree_OnEndLabelEdit (NewValue Key /)
  (Tree_LogEvent (strcat "OnEndLabelEdit (" NewValue ")"))
  (if (Tree_IsTracking)
    (dcl_Control_SetText Tree_Main_tbxLabel NewValue)
  )
)


(defun c:Tree_Main_Tree_OnItemExpanded (Label Key /)
  (Tree_LogEvent
    (strcat
      "OnItem"
      (if (dcl_Tree_IsItemExpanded Tree_Main_tree Key) "Expanded" "Collapsed")
      " ("
      Label
      ")"
    )
  )
)


(defun c:Tree_Main_Tree_OnItemExpanding (Label Key /)
  (Tree_LogEvent
    (strcat
      "OnItem"
      (if (dcl_Tree_IsItemExpanded Tree_Main_tree Key) "Collapsing" "Expanding")
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
  ; Commented out to reduce log traffic
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
                 (   *ODCL:Prefix
                 ) ;_ already defined
                 (   (vl-registry-read
                         "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 32-bit location
                 (   (vl-registry-read
                         "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 32-bit location
                 (   (vl-registry-read
                         "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 64-bit location
                 (   (vl-registry-read
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
(   (lambda (demoname)
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
