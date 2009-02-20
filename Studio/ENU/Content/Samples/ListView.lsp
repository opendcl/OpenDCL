(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL sample programs.\nEnter \"ListView\" to run the sample.\n")
)


(defun c:ListView ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "ListView.odcl")
  (dcl_FORM_SHOW ListView_ListViewDlg)
  ;; The Event handlers manage the form here. 
  (PRINC)
)

;;-----------------------------------------------------------

;; This function will run everytime the form is loaded into AutoCAD

(defun c:ListViewDlg_OnInitialize ( / Col0Width Col1Width Col2Width Col3Width)
    ;; calculate the required column widths
    (setq Col0Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Column 0     ")
          Col1Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Column 1     ")
          Col2Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Column 2     ")
          Col3Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Column 3     ")
    )
    ;; add columns of the calculated widths
    (dcl_ListView_AddColumns ListView_ListViewDlg_ListView1
                              (list (list "Column 0" 0 Col0Width)
                                    (list "Column 1" 0 Col1Width)
                                    (list "Column 2" 0 Col2Width)
                                    (list "Column 3" 0 Col3Width)
                              )
    )
    ;; clear and fill the list
    (dcl_ListView_FillList
        ListView_ListViewDlg_ListView1
        (list ;; add row 1 items
              (list "List 1a" 0 "List 2a" 0 "List 3a" 0 "List 4a" 0)
              ;; add row 2 items
              (list "List 1b" 1 "List 2b" 1 "List 3b" 1 "List 4b" 1)
              ;; add row 3 items
              (list "List 1c" 2 "List 2c" 2 "List 3c" 2 "List 4c" 2)
        )
    )
)

;;-----------------------------------------------------------

(defun c:ListView_ListViewDlg_ListView1_OnClicked (nRow nCol /)
     (dcl_MessageBox (strcat "Column " (itoa nCol) ", Row " (itoa nRow))
                      "Clicked Location"
     )
)

;;-----------------------------------------------------------

;; the user has clicked the "Close" button, so lets close the dialog now.
(defun c:ListViewDlg_Close_Clicked () (dcl_Form_Close ListView_ListViewDlg))

;;-----------------------------------------------------------

(princ)

 ;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
