(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"Misc\" ein, um das Beispiel zu starten.\n")
)

;; Diese Funktion lädt das Projekt und zeigt den Dialog an
(DEFUN c:Misc (/  *error*)
    (DEFUN *error* (msg)
        (WHILE (< 0 (GETVAR "cmdactive")) (COMMAND))
        ;; Errorhandling
        (PRINC (STRCAT "\nProgrammfehler: " (GETVAR "errno") " :- " msg))
        (PRINC)
    )

  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Misc.odcl")
  
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog. 
  (dcl_FORM_SHOW Misc_DemoModal)
  (PRINC)
)



;; Wird beim Wechsel der Karteikarten ausgelöst
(DEFUN c:DemoModal_TabControl1_Changed (nSelIndex / nCount)
    (COND
       
        ;; Karteikarte 1
        ((= nSelIndex 1)
	  (AddBlocksToListBox)
	)
        ;; Karteikarte 2
        ((= nSelIndex 2)
         (SETQ nCount (dcl_TREE_COUNTITEMS Misc_DemoModal_TreeControl1))
         (IF (= nCount 0)
             (PROGN
                 (dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1 "Dia1" "t1")
                 (dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1
                                      "Diabibliothek"
                                      "t2"
                 )
                 (dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1 "Dia3" "t3")
                 (dcl_TREE_ADDCHILD Misc_DemoModal_TreeControl1
                                     '(("t2" "Element1" "C1")
                                       ("t2" "Element2" "C2")
                                       ("t2" "Element3" "C3")
                                      )
                 )
                 (dcl_TREE_SELECTITEM Misc_DemoModal_TreeControl1 "t1")
             )
         )
        )
        ;; Karteikarte 3
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



;; Wird ausgelöst, wenn sich die Auswahl in BlockNameList ändert
(DEFUN c:DemoModal_BlockNameList_SelectionChanged (nSelCount sSelText /)
    (dcl_CONTROL_SETBLOCKNAME Misc_DemoModal_BlockView sSelText)
)

;; Wird ausgelöst, wenn sich die Auswahl in der Baumstruktur ändert
(DEFUN c:DemoModal_TreeControl1_SelectionChanged
       (sSelText sSelKey / sParent FileName Path)
    ;; Übergeordnetes Element
    (SETQ sParent (dcl_TREE_GETPARENT Misc_DemoModal_TreeControl1 sSelKey))
    (or
        (setq Path (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit Position
        (setq Path (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit Position
        (setq Path (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit Position
        (setq Path (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit Position
    )
    (setq FileName
        (strcat Path
            (IF (NULL sParent)
                (strcat sSelText ".sld")
                ;; ansonsten
                (strcat (dcl_TREE_GETITEMTEXT Misc_DemoModal_TreeControl1 sParent) ".slb")
            )
        )
    )
    (IF (FINDFILE FileName)
        (dcl_SLIDEVIEW_LOAD Misc_DemoModal_SlideView1 FileName (IF (NULL sParent) "" sSelText))
        ;; ansonsten
        (dcl_SLIDEVIEW_CLEAR Misc_DemoModal_SlideView1)
    )
)


(DEFUN c:DemoModal_BrowseFolders_Clicked (/ path)
    (IF (SETQ path (dcl_BROWSEFOLDER "Wählen Sie ein Verzeichnis:" "c:\\Programme" nil 81))
        (ALERT path)
    )
)


;; Diese Funktion liest alle Blöcke der Zeichnung aus und fügt ihre Namen zur Liste hinzu
(DEFUN AddBlocksToListBox (/ BlockInfo BlkLst Blk)
    (dcl_LISTBOX_CLEAR Misc_DemoModal_BlockNameList)
    (SETQ BlockInfo (TBLNEXT "BLOCK" T))
    ;; ersten Block der Blocktabelle holen
    ;; Alle Blöcke der Objektsammlung durchlaufen
    ;; Blocknamen holen
    ;; Anonyme Blöcke, Bemaßungen, XREF's usw. ignorieren
    ;; Blockname zur Liste 'BlkLst hinzufügen
    ;; zum nächsten Block gehen
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
    ;;_ Alternative Methode zum Hinzufügen der Blocknamen zur Liste
;;;   ; ersten Block der Blocktabelle holen
;;;   (setq BlockInfo (tblnext "BLOCK" T))
;;;   ; Blocknamen mit AddString dem Listenfeld hinzufügen
;;;   (if BlockInfo 
;;;      (dcl_ListBox_AddString Misc_DemoModal_BlockNameList (cdr (assoc 2 BlockInfo)))
;;;   );_ if
;;;   ; in einer Schleife den Vorgang für alle Blöcke durchlaufen
;;;   (while BlockInfo
;;;      (setq BlockInfo (tblnext "BLOCK"))
;;;      ; zum Listenfeld hinzufügen
;;;      (if BlockInfo
;;;	 (dcl_ListBox_AddString Misc_DemoModal_BlockNameList (cdr (assoc 2 BlockInfo)))
;;;      );_ if
;;;   );_ while
    (PRINC)
)


(PRINC)
