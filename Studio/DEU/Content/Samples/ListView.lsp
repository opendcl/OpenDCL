(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"ListView\" ein, um das Beispiel zu starten.\n")
)


(defun c:ListView ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "ListView.odcl")
  
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog. 
  (dcl_FORM_SHOW ListView_ListViewDlg)
    
  (PRINC)
)

;;-----------------------------------------------------------

;; Diese funktion wird jedes Mal ausgeführt, wenn der Dialog angezeigt wird.

(defun c:ListViewDlg_OnInitialize ( / Col0Width Col1Width Col2Width Col3Width)
    ;; Spaltenbreiten neuberechnen
    (setq Col0Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Spalte 0     ")
          Col1Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Spalte 1     ")
          Col2Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Spalte 2     ")
          Col3Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Spalte 3     ")
    )
    ;; neue Spaltenbreiten zuweisen
    (dcl_ListView_AddColumns ListView_ListViewDlg_ListView1
                              (list (list "Spalte 0" 0 Col0Width)
                                    (list "Spalte 1" 0 Col1Width)
                                    (list "Spalte 2" 0 Col2Width)
                                    (list "Spalte 3" 0 Col3Width)
                              )
    )
    ;; Liste bereinigen und neu befüllen
    (dcl_ListView_FillList
        ListView_ListViewDlg_ListView1
        (list ;; Elemente der Zeile 1 hinzufügen
              (list "Zelle 1a" 0 "Zelle 2a" 0 "Zelle 3a" 0 "Zelle 4a" 0)
              ;; Elemente der Zeile 2 hinzufügen
              (list "Zelle 1b" 1 "Zelle 2b" 1 "Zelle 3b" 1 "Zelle 4b" 1)
              ;; Elemente der Zeile 3 hinzufügen
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

;; Der Anwender hat die Schaltfläche Schließen geklickt, deshalb wird sie geschlossen
(defun c:ListViewDlg_Close_Clicked () (dcl_Form_Close ListView_ListViewDlg))

;;-----------------------------------------------------------

(princ)

 ;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
