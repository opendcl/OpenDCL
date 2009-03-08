(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"Hatches\" ein, um das Beispiel zu starten.\n")
)

(defun c:Hatches ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Hatches.odcl")
  (dcl_FORM_SHOW Hatches_Hatch)
  ;; The Event handlers manage the form here. 
  (PRINC)
)

;;-----------------------------------------------------------------------------


(defun c:Hatches_Hatch_ComboBox1_OnSelChanged (nSelection sSelText /)
  (dcl_hatch_setpattern Hatches_Hatch_Hatch1 sSelText)
  (dcl_hatch_setpattern Hatches_Hatch_Hatch2 sSelText)
)

;;-----------------------------------------------------------------------------


(defun c:Hatches_Hatch_ComboBox2_OnSelChanged (nSelection sSelText / hatpat)
    ;; set the selected scale for each hatch display tile
    (dcl_Control_SetHatchScale Hatches_Hatch_Hatch1 (atof sSelText))
    (dcl_Control_SetHatchScale Hatches_Hatch_Hatch2 (atof sSelText))

    ;; get the current hatch pattern name from the ComboBox
    (setq hatpat (dcl_ComboBox_GetLBText
                     Hatches_Hatch_ComboBox1
                     (dcl_ComboBox_GetCurSel Hatches_Hatch_ComboBox1)
                 )
    )
    ; Use the previously defined function to re-draw the pattern in the hatch displays
    (c:Hatches_Hatch_ComboBox1_OnSelChanged nil hatpat)
)

;;-----------------------------------------------------------------------------

(princ)

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
