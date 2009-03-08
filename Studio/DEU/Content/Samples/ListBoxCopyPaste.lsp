(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"CopyPaste\" ein, um das Beispiel zu starten.\n")
)


(defun c:CopyPaste ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "ListBoxCopyPaste.odcl")
  (dcl_FORM_SHOW ListBoxCopyPaste_Form)
  ;; The Event handlers manage the form here. 
  (PRINC) 
)

;;-----------------------------------------------------------

;; The following function fires when clicking the button

(defun c:ListBoxCopyPaste_Form_TextButton1_OnClicked (/ items len DelLst x cnt)
    ;; Get the selection from the left list box
    (setq items (dcl_ListBox_GetSelectedItems ListBoxCopyPaste_Form_ListBox1))
    (setq len (length items))
    (if (> len 0)
        ;; if there's one or more items selected then...
        
        (progn ;; query the status of the check box and if ticked,
               ;; continue to append items to the right list box
               (if (/= 1 (dcl_Control_GetValue ListBoxCopyPaste_Form_CheckBox1))
                   (dcl_ListBox_Clear ListBoxCopyPaste_Form_ListBox2)
               )
               ;; and add the selected items to the right list box
               (dcl_ListBox_AddList ListBoxCopyPaste_Form_ListBox2 items)

            ;; query the status of the check box and if ticked,
            ;; remove items to the left list box
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

;; if the user double clicks on items in the the left hand list
;; we perform the same action as if the button was clicked...
(defun c:ListBoxCopyPaste_Form_ListBox1_OnDblClicked ( /)
    ;; run the function above...
     (c:ListBoxCopyPaste_Form_TextButton1_OnClicked)
)

;;-----------------------------------------------------------

(princ)

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
