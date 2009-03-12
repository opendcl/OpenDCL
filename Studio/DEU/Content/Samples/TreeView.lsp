(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"TREE\" ein, um das Beispiel zu starten.\n")
)


;; Diese Funktion lädt das Projekt und zeigt den Dialog an
(defun c:TREE (/)
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "TreeView.odcl")
    
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog. 
  (dcl_FORM_SHOW TreeView_Main)
  (PRINC)
)


(defun TreeView_LogEvent (event)
  (if TreeView_Main_lbxLog
    (dcl_ListBox_SetTopIndex TreeView_Main_lbxLog (dcl_ListBox_AddString TreeView_Main_lbxLog event))
  )
)


(defun TreeView_IsTracking (/)
  (and TreeView_Main_chkTrack (= 1 (dcl_Control_GetValue TreeView_Main_chkTrack)))
)


(defun c:TreeView_Main_OnInitialize	(/ P1 P2)
  (setq P1 (dcl_Tree_AddParent TreeView_Main_tree "Erstes Eltern-Element" 0 -1 1))
  (dcl_Tree_AddChild TreeView_Main_tree
    (list
      (list P1 "Element 1" 2 3)
      (list P1 "Element 2" 2 3)
      (list P1 "Element 3" 2 3)
    )
  )
  (setq P2 (dcl_Tree_AddParent TreeView_Main_tree "Zweites Eltern-Element" 0 -1 8))
  (dcl_Tree_AddChild TreeView_Main_tree
    (list
      (list P2 "Element C" 4 5)
      (list P2 "Element B" 4 5)
      (list P2 "Element A" 4 5)
    )
  )
  (dcl_Tree_AddParent TreeView_Main_tree
    '(("Drittes Eltern-Element" 0 -1 1)
      ("Viertes Eltern-Element" 0 -1 1)
     )
  )
  (dcl_Tree_SelectItem TreeView_Main_tree P1)
)


(defun c:TreeView_Main_chkHasButtons_OnClicked (Value /)
  (dcl_Control_SetHasButtons TreeView_Main_tree (= Value 1))
)


(defun c:TreeView_Main_chkHasLines_OnClicked (Value /)
  (dcl_Control_SetHasLines TreeView_Main_tree (= Value 1))
)


(defun c:TreeView_Main_chkSingleClickExpand_OnClicked (Value /)
  (dcl_Control_SetSingleClickExpand TreeView_Main_tree (= Value 1))
)


(defun c:TreeView_Main_chkTrack_OnClicked (Value /)
  (setq CurSel (if (= 1 Value) (dcl_Tree_GetSelectedItem TreeView_Main_tree)))
  (if CurSel
    (progn
      (dcl_Control_SetText TreeView_Main_tbxLabel (dcl_Tree_GetItemLabel TreeView_Main_tree CurSel))
      (setq Images (dcl_Tree_GetItemImages TreeView_Main_tree CurSel))
      (dcl_ImageComboBox_SetCurSel TreeView_Main_cbxImage (1+ (car Images)))
      (dcl_ImageComboBox_SetCurSel TreeView_Main_cbxSelImage (1+ (cadr Images)))
      (dcl_ImageComboBox_SetCurSel TreeView_Main_cbxExpImage (1+ (caddr Images)))
    )
  )
)


(defun c:TreeView_Main_tbxLabel_OnEditChanged (NewValue / CurSel)
  (setq CurSel (if (TreeView_IsTracking) (dcl_Tree_GetSelectedItem TreeView_Main_tree)))
  (if CurSel
    (dcl_Tree_SetItemLabel TreeView_Main_tree CurSel NewValue)
  )
)


(defun c:TreeView_Main_cbxImage_OnSelChanged (ItemIndex Value / CurSel Images)
  (setq CurSel (if (TreeView_IsTracking) (dcl_Tree_GetSelectedItem TreeView_Main_tree)))
  (if CurSel
    (progn
      (setq Images (dcl_Tree_GetItemImages TreeView_Main_tree CurSel))
      (dcl_Tree_SetItemImages TreeView_Main_tree CurSel (1- ItemIndex) (cadr Images) (caddr Images))
    )
  )
)


(defun c:TreeView_Main_cbxSelImage_OnSelChanged (ItemIndex Value / CurSel Images)
  (setq CurSel (if (TreeView_IsTracking) (dcl_Tree_GetSelectedItem TreeView_Main_tree)))
  (if CurSel
    (progn
      (setq Images (dcl_Tree_GetItemImages TreeView_Main_tree CurSel))
      (dcl_Tree_SetItemImages TreeView_Main_tree CurSel (car Images) (1- ItemIndex) (caddr Images))
    )
  )
)


(defun c:TreeView_Main_cbxExpImage_OnSelChanged (ItemIndex Value / CurSel Images)
  (setq CurSel (if (TreeView_IsTracking) (dcl_Tree_GetSelectedItem TreeView_Main_tree)))
  (if CurSel
    (progn
      (setq Images (dcl_Tree_GetItemImages TreeView_Main_tree CurSel))
      (dcl_Tree_SetItemImages TreeView_Main_tree CurSel (car Images) (cadr Images) (1- ItemIndex))
    )
  )
)


(defun c:TreeView_Main_btnAddParent_OnClicked (/)
  (dcl_Tree_AddParent TreeView_Main_tree
    (dcl_Control_GetText TreeView_Main_tbxLabel)
    (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxImage))
    (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxSelImage))
    (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxExpImage))
  )
)


(defun c:TreeView_Main_btnAddChild_OnClicked (/ CurSel Images)
  (setq CurSel (dcl_Tree_GetSelectedItem TreeView_Main_tree))
  (if CurSel
    (progn
      (setq Images (dcl_Tree_GetItemImages TreeView_Main_tree CurSel))
      (dcl_Tree_AddChild TreeView_Main_tree CurSel
        (dcl_Control_GetText TreeView_Main_tbxLabel)
        (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxImage))
        (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxSelImage))
        (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxExpImage))
      )
    )
  )
)


(defun c:TreeView_Main_btnAddSibling_OnClicked (/ CurSel Images)
  (setq CurSel (dcl_Tree_GetSelectedItem TreeView_Main_tree))
  (if CurSel
    (progn
      (setq Images (dcl_Tree_GetItemImages TreeView_Main_tree CurSel))
      (dcl_Tree_AddSibling TreeView_Main_tree CurSel
        (dcl_Control_GetText TreeView_Main_tbxLabel)
        (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxImage))
        (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxSelImage))
        (1- (dcl_ImageComboBox_GetCurSel TreeView_Main_cbxExpImage))
      )
    )
  )
)


(defun c:TreeView_Main_btnDelete_OnClicked (/ CurSel)
  (setq CurSel (dcl_Tree_GetSelectedItem TreeView_Main_tree))
  (if CurSel
    (dcl_Tree_DeleteItem TreeView_Main_tree CurSel)
  )
)


(defun c:TreeView_Main_btnSortChildren_OnClicked (/ CurSel)
  (setq CurSel (dcl_Tree_GetSelectedItem TreeView_Main_tree))
  (if CurSel
    (dcl_Tree_SortChildren TreeView_Main_tree CurSel)
  )
)


(defun c:TreeView_Main_btnExpand_OnClicked (/ CurSel)
  (setq CurSel (dcl_Tree_GetSelectedItem TreeView_Main_tree))
  (if CurSel
    (dcl_Tree_ExpandItem TreeView_Main_tree CurSel 0)
  )
)


(defun c:TreeView_Main_btnEditLabel_OnClicked (/ CurSel)
  (setq CurSel (dcl_Tree_GetSelectedItem TreeView_Main_tree))
  (if CurSel
    (dcl_Tree_StartLabelEdit TreeView_Main_tree CurSel)
  )
)


(defun c:TreeView_Main_btnClear_OnClicked (/)
  (dcl_Tree_Clear TreeView_Main_tree)
)


(defun c:TreeView_Main_Tree_OnBeginLabelEdit (Key /)
  (TreeView_LogEvent (strcat "OnBeginLabelEdit (" (dcl_Tree_GetItemLabel TreeView_Main_tree Key) ")"))
)


(defun c:TreeView_Main_Tree_OnClicked (/)
  (TreeView_LogEvent "OnClicked")
)


(defun c:TreeView_Main_Tree_OnDblClicked (/)
  (TreeView_LogEvent "OnDblClicked")
)


(defun c:TreeView_Main_Tree_OnDeleteItem (Label Key /)
  (TreeView_LogEvent (strcat "OnDeleteItem (" Label ")"))
)


(defun c:TreeView_Main_Tree_OnDragnDropBegin (/)
  (TreeView_LogEvent "OnDragnDropBegin")
)


(defun c:TreeView_Main_Tree_OnDragnDropFromControl (ProjectName FormName ControlName DropPoint /)
  (TreeView_LogEvent "OnDragnDropFromControl")
)


(defun c:TreeView_Main_Tree_OnDragnDropFromOther (DropPoint /)
  (TreeView_LogEvent "OnDragnDropFromOther")
)


(defun c:TreeView_Main_Tree_OnDragnDropToAutoCAD (DropPoint Viewport /)
  (TreeView_LogEvent "OnDragnDropToAutoCAD")
)


(defun c:TreeView_Main_Tree_OnEndLabelEdit (NewValue Key /)
  (TreeView_LogEvent (strcat "OnEndLabelEdit (" NewValue ")"))
  (if (TreeView_IsTracking)
    (dcl_Control_SetText TreeView_Main_tbxLabel NewValue)
  )
)


(defun c:TreeView_Main_Tree_OnItemExpanded (Label Key /)
  (TreeView_LogEvent
    (strcat
      "OnItem"
      (if (dcl_Tree_IsItemExpanded TreeView_Main_tree Key) "Expanded" "Collapsed")
      " ("
      Label
      ")"
    )
  )
)


(defun c:TreeView_Main_Tree_OnItemExpanding (Label Key /)
  (TreeView_LogEvent
    (strcat
      "OnItem"
      (if (dcl_Tree_IsItemExpanded TreeView_Main_tree Key) "Collapsing" "Expanding")
      " ("
      Label
      ")"
    )
  )
)


(defun c:TreeView_Main_Tree_OnKeyDown (Character RepeatCount Flags /)
  (TreeView_LogEvent (strcat "OnKeyDown (" (itoa (ascii Character)) " " (itoa RepeatCount) " " (itoa Flags) ")"))
)


(defun c:TreeView_Main_Tree_OnKeyUp (Character RepeatCount Flags /)
  (TreeView_LogEvent (strcat "OnKeyUp (" (itoa (ascii Character)) " " (itoa RepeatCount) " " (itoa Flags) ")"))
)


(defun c:TreeView_Main_Tree_OnKillFocus (/)
  (TreeView_LogEvent "OnKillFocus")
)


(defun c:TreeView_Main_Tree_OnMouseMove (Flags X Y /)
  ; Auskommentiert, um den LOG-Traffic zu minimieren
  NIL ;(TreeView_LogEvent (strcat "OnMouseMove (" (itoa Flags) " " (itoa X) " " (itoa Y) ")"))
)


(defun c:TreeView_Main_Tree_OnReturnPressed (/)
  (TreeView_LogEvent "OnReturnPressed")
)


(defun c:TreeView_Main_Tree_OnRightClick (/)
  (TreeView_LogEvent "OnRightClick")
)


(defun c:TreeView_Main_Tree_OnRightDblClick (/)
  (TreeView_LogEvent "OnRightDblClick")
)


(defun c:TreeView_Main_Tree_OnSelChanged (Label Key / Images)
  (TreeView_LogEvent (strcat "OnSelChanged (" Label ")"))
  (if (TreeView_IsTracking)
    (progn
      (dcl_Control_SetText TreeView_Main_tbxLabel Label)
      (setq Images (dcl_Tree_GetItemImages TreeView_Main_tree Key))
      (dcl_ImageComboBox_SetCurSel TreeView_Main_cbxImage (1+ (car Images)))
      (dcl_ImageComboBox_SetCurSel TreeView_Main_cbxSelImage (1+ (cadr Images)))
      (dcl_ImageComboBox_SetCurSel TreeView_Main_cbxExpImage (1+ (caddr Images)))
    )
  )
)


(defun c:TreeView_Main_Tree_OnSetFocus (/)
  (TreeView_LogEvent "OnSetFocus")
)


(princ)
