(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"ListView\" ein, um das Beispiel zu starten.\n")
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
    (setq Col0Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Spalte 0     ")
          Col1Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Spalte 1     ")
          Col2Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Spalte 2     ")
          Col3Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Spalte 3     ")
    )
    ;; add columns of the calculated widths
    (dcl_ListView_AddColumns ListView_ListViewDlg_ListView1
                              (list (list "Spalte 0" 0 Col0Width)
                                    (list "Spalte 1" 0 Col1Width)
                                    (list "Spalte 2" 0 Col2Width)
                                    (list "Spalte 3" 0 Col3Width)
                              )
    )
    ;; clear and fill the list
    (dcl_ListView_FillList
        ListView_ListViewDlg_ListView1
        (list ;; add row 1 items
              (list "Zelle 1a" 0 "Zelle 2a" 0 "Zelle 3a" 0 "Zelle 4a" 0)
              ;; add row 2 items
              (list "Zelle 1b" 1 "Zelle 2b" 1 "Zelle 3b" 1 "Zelle 4b" 1)
              ;; add row 3 items
              (list "Zelle 1c" 2 "Zelle 2c" 2 "Zelle 3c" 2 "Zelle 4c" 2)
        )
    )
)

;;-----------------------------------------------------------

(defun c:ListView_ListViewDlg_ListView1_OnClicked (nRow nCol /)
     (dcl_MessageBox (strcat "Spalte " (itoa nCol) ", Zeile " (itoa nRow))
                      "Geklickte Position"
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
