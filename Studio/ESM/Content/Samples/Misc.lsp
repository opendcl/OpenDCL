(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL sample programs.\nEnter \"Misc\" to run the sample.\n")
)

;; this function loads the project & shows the form
(DEFUN c:Misc (/  *error*)
    (DEFUN *error* (msg)
        (WHILE (< 0 (GETVAR "cmdactive")) (COMMAND))
        ;; do error stuff
        (PRINC (STRCAT "\nApplication Error: " (GETVAR "errno") " :- " msg))
        (PRINC)
    )

  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Misc.odcl")
  ;; Show the modal dialog .. (dcl_Form_IsActive test is not required anymore
  (dcl_FORM_SHOW Misc_DemoModal)
  ;; The Event handlers manage the form here.
  (PRINC)
)



;; this function gets fired when a tab gets changed
(DEFUN c:DemoModal_TabControl1_Changed (nSelIndex / nCount)
    (COND
       
        ;; cond tab 1
        ((= nSelIndex 1)
	  (AddBlocksToListBox)
	)
        ;; cond tab 2
        ((= nSelIndex 2)
         (SETQ nCount (dcl_TREE_COUNTITEMS Misc_DemoModal_TreeControl1))
         (IF (= nCount 0)
             (PROGN
                 (dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1 "slide1" "t1")
                 (dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1
                                      "slide library"
                                      "t2"
                 )
                 (dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1 "slide3" "t3")
                 (dcl_TREE_ADDCHILD Misc_DemoModal_TreeControl1
                                     '(("t2" "Child1" "C1")
                                       ("t2" "Child2" "C2")
                                       ("t2" "Child3" "C3")
                                      )
                 )
                 (dcl_TREE_SELECTITEM Misc_DemoModal_TreeControl1 "t1")
             )
         )
        )
        ;; cond tab 3
        ((= nSelIndex 3)
         (dcl_COMBOBOX_ADDCOLOR Misc_DemoModal_ComboBox4 156)
        )
        ((= nSelIndex 4)
         (IF (ZEROP (dcl_ImageComboBox_GetCount Misc_DemoModal_ImageCombo1))
             (PROGN
                 (dcl_ImageComboBox_AddString Misc_DemoModal_ImageCombo1 "Test 1" 0 0)
                 (dcl_ImageComboBox_AddString Misc_DemoModal_ImageCombo1 "Test 2" 1 1)
                 (dcl_ImageComboBox_AddString Misc_DemoModal_ImageCombo1 "Test 3" 2 2)
             )
         )
        )
        
    )
)


(defun c:Misc_DemoModal_cmdRun_OnClicked ( / cnt wait)
  (setq cnt 0)
  (while (< cnt 100)
    (setq wait 10000)
    (while (> (setq wait (1- wait)) 0))
    (dcl_Control_SetValue Misc_DemoModal_ProgressBar1 (setq cnt (1+ cnt)))
  )
  (princ)
)



;; this function gets fired when the BlockNameList selection is changed
(DEFUN c:DemoModal_BlockNameList_SelectionChanged (nSelCount sSelText /)
    (dcl_CONTROL_SETBLOCKNAME Misc_DemoModal_BlockView sSelText)
)

;; this function gets fired when the TreeControl selection is changed
(DEFUN c:DemoModal_TreeControl1_SelectionChanged
       (sSelText sSelKey / sParent FileName Path)
    ;; get the parent info
    (SETQ sParent (dcl_TREE_GETPARENT Misc_DemoModal_TreeControl1 sSelKey))
    (or
        (setq Path (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
        (setq Path (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
        (setq Path (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
        (setq Path (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
    )
    (setq FileName
        (strcat Path
            (IF (NULL sParent)
                (strcat sSelText ".sld")
                ;; else
                (strcat (dcl_TREE_GETITEMTEXT Misc_DemoModal_TreeControl1 sParent) ".slb")
            )
        )
    )
    (IF (FINDFILE FileName)
        (dcl_SLIDEVIEW_LOAD Misc_DemoModal_SlideView1 FileName (IF (NULL sParent) "" sSelText))
        ;; else
        (dcl_SLIDEVIEW_CLEAR Misc_DemoModal_SlideView1)
    )
)


(DEFUN c:DemoModal_BrowseFolders_Clicked (/ path)
    (IF (SETQ path (dcl_BROWSEFOLDER "Pick a Directory:" "c:\\Program Files" nil 81))
        (ALERT path)
    )
)


;; this function retrieves all the blocks and adds their names to the BlockName ListBox
(DEFUN AddBlocksToListBox (/ BlockInfo BlkLst Blk)
    (dcl_LISTBOX_CLEAR Misc_DemoModal_BlockNameList)
    (SETQ BlockInfo (TBLNEXT "BLOCK" T))
    ;; get the first block in the block table record
    ;; loop through all block records
    ;; get block name
    ;; ignore anonymous blocks, dim's, xref's, etc...
    ;; add the block name to the list 'BlkLst
    ;; goto next block
    (WHILE BlockInfo
        (SETQ blk (CDR (ASSOC 2 BlockInfo)))
        (IF (NOT (WCMATCH blk "`*U*,`*D*,`*X*,`*T*,_*,*|*,A$*"))
            (SETQ BlkLst (APPEND BlkLst (LIST blk)))
        )
        (SETQ BlockInfo (TBLNEXT "BLOCK"))
    )
    (IF (NULL BlkLst)
        (dcl_CONTROL_SETVISIBLE Misc_DemoModal_lblNoBlocks T)
        (dcl_CONTROL_SETVISIBLE Misc_DemoModal_lblNoBlocks nil)
    )
    (dcl_LISTBOX_ADDLIST Misc_DemoModal_BlockNameList BlkLst)
    ;;
    ;;_ alternate method adding one string at a time to the BlockList 
;;;   ; get the first block in the block table record
;;;   (setq BlockInfo (tblnext "BLOCK" T))
;;;   ; add the first item to the BlockName ListBox with Add String
;;;   (if BlockInfo 
;;;      (dcl_ListBox_AddString Misc_DemoModal_BlockNameList (cdr (assoc 2 BlockInfo)))
;;;   );_ if
;;;   ; do loop until all the blocks have been added
;;;   (while BlockInfo
;;;      (setq BlockInfo (tblnext "BLOCK"))
;;;      ; add the item to the BlockName ListBox
;;;      (if BlockInfo
;;;	 (dcl_ListBox_AddString Misc_DemoModal_BlockNameList (cdr (assoc 2 BlockInfo)))
;;;      );_ if
;;;   );_ while
    (PRINC)
)


(PRINC)
