(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"CopyPaste\" ein, um das Beispiel zu starten.\n")
)


(defun c:CopyPaste ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "ListBoxCopyPaste.odcl")
  
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog. 
  (dcl_FORM_SHOW ListBoxCopyPaste_Form)
  (PRINC) 
)

;;-----------------------------------------------------------

;; Die folgende Funktion wird ausgelöst, wenn die Schaltfläche geklickt wird.

(defun c:ListBoxCopyPaste_Form_TextButton1_OnClicked (/ items len DelLst x cnt)
    ;; Auswahl aus der Liste holen
    (setq items (dcl_ListBox_GetSelectedItems ListBoxCopyPaste_Form_ListBox1))
    (setq len (length items))
    (if (> len 0)
        ;; sind einer oder mehr gewählt ...
        
        (progn ;; Prüfen, ob das Kontrollkästchen angehakt ist
               ;; ansonsten zunächst Liste leeren
               (if (/= 1 (dcl_Control_GetValue ListBoxCopyPaste_Form_CheckBox1))
                   (dcl_ListBox_Clear ListBoxCopyPaste_Form_ListBox2)
               )
               ;; und die Elemente der rechten Liste hinzufügen
               (dcl_ListBox_AddList ListBoxCopyPaste_Form_ListBox2 items)

            ;; Prüfen, ob das Kontrollkästchen angehakt ist
            ;; wenn ja, dann aus der linken Liste entfernen
            (if (= 1 (dcl_Control_GetValue ListBoxCopyPaste_Form_CheckBox2))
              (progn
                (Setq DelLst (dcl_ListBox_GetSelectedNths ListBoxCopyPaste_Form_ListBox1))
                (Setq cnt 0)
                
                (while (< cnt (length DelLst))
                  (setq x (nth cnt DelLst))
                  (dcl_ListBox_DeleteString ListBoxCopyPaste_Form_ListBox1 (- x cnt))
                  (setq cnt (1+ cnt))
                )
              )
            )
            
        )
    )
)

;; Beim Doppelklick auf einen Eintrag der linken Liste wird der gleiche Vorgang ausgelöst
(defun c:ListBoxCopyPaste_Form_ListBox1_OnDblClicked ( /)
    ;; Ereignisfunktion aufrufen
     (c:ListBoxCopyPaste_Form_TextButton1_OnClicked)
)

;;-----------------------------------------------------------

(princ)

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
