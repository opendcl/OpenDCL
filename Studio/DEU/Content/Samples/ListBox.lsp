(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"ListBox\" ein, um das Beispiel zu starten.\n")
)

(DEFUN c:ListBox (/
               c:ListBox_form1_clearlist_onclicked
               c:ListBox_form1_exitdialog_onclicked
               c:ListBox_form1_remove_onclicked
               c:ListBox_form1_selectfiles_onclicked
               c:ListBox_form1_sort_onclicked
              )
  ;;----------------------------------------------------
  (DEFUN c:ListBox_form1_selectfiles_onclicked (/ bs_getfilem bs_filelist fn)
    (IF (SETQ bs_getfilem (dcl_MULTIFILEDIALOG "Zeichnungsdateien (*.dwg)|*.dwg||"
                                                            ; ExtensionFilters
                                                "Dateien wählen" ; Title
                                                (IF g:lastfolder
                                                  g:lastfolder
                                                  (GETVAR "DWGPREFIX")
                                                )           ; Default Folder
                          )
        )
      (SETQ g:lastfolder (VL-FILENAME-DIRECTORY (CAR bs_getfilem))
            bs_filelist  bs_getfilem
      )
    )
    (IF bs_filelist
      (PROGN (SETQ bs_filelist (VL-SORT bs_filelist '<))
             (FOREACH fn bs_filelist
               (IF (NOT (VL-POSITION fn g:bs_filelist))
                 (SETQ g:bs_filelist (APPEND g:bs_filelist (LIST fn)))
               )
             )
             (dcl_LISTBOX_CLEAR ListBox_form1_listbox)
             (dcl_LISTBOX_ADDLIST ListBox_form1_listbox g:bs_filelist)
      )
    )
  )
  ;;----------------------------------------------------
  (DEFUN c:ListBox_form1_clearlist_onclicked (/)
    (dcl_LISTBOX_CLEAR ListBox_form1_listbox)
    (SETQ g:bs_filelist '())
  )
  ;;----------------------------------------------------
  (DEFUN c:ListBox_form1_sort_onclicked (/)
    (IF g:bs_filelist
      (PROGN (SETQ g:bs_filelist (VL-SORT g:bs_filelist '<))
             (dcl_LISTBOX_CLEAR ListBox_form1_listbox)
             (dcl_LISTBOX_ADDLIST ListBox_form1_listbox g:bs_filelist)
      )
    )
  )
  ;;----------------------------------------------------
  (DEFUN c:ListBox_form1_remove_onclicked (/ rvalue str)
    (IF (AND (SETQ rvalue (dcl_LISTBOX_GETCURSEL ListBox_form1_listbox))
             (NOT (MINUSP rvalue))
        )
      (PROGN (SETQ str           (NTH rvalue g:bs_filelist)
                   g:bs_filelist (VL-REMOVE-IF '(LAMBDA (x) (= x str)) g:bs_filelist)
             )
             (dcl_LISTBOX_CLEAR ListBox_form1_listbox)
             (dcl_LISTBOX_ADDLIST ListBox_form1_listbox g:bs_filelist)
      )
    )
  )
  ;;----------------------------------------------------
  (DEFUN c:ListBox_form1_exitdialog_onclicked (/) (dcl_FORM_CLOSE ListBox_form1))
  ;;----------------------------------------------------
  ;;----------------------------------------------------
  ;;Main
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "ListBox.odcl")
  (SETQ g:bs_filelist '())
  (dcl_Form_Show ListBox_Form1)
  (PRINC)
)
(PRINC)
 ;|«Visual LISP© Format Options»
(95 2 60 2 nil "end of " 90 60 2 1 0 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
