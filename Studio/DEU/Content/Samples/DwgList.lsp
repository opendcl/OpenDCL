(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"DwgList\" ein, um das Beispiel zu starten.\n")
)



(defun c:DwgList ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "DwgList.odcl")
  (dcl_FORM_SHOW DwgList_DwgListForm)
  ;; The Event handlers manage the form here.
  (princ)
)



(defun c:DwgListForm_DwgList1_OnDblClicked ()
   (Setq fileName (dcl_DwgList_GetFileName DwgList_DwgListForm_DwgList1))
    (if (equal (strcase (substr fileName (- (strlen fileName) 3) 4)) ".DWG")
      (progn
        ;(dcl_SetCmdBarFocus)
        (dcl_sendstring (strcat "-insert " fileName "\n"))
      )
    )
    (princ)
)


(defun c:DwgList_DwgListForm_cmdUP_OnClicked ( /)
    (setq sPath (strcat (dcl_ComboBox_GetDir DwgList_DwgListForm_ComboBox1) "\\.."))
    (dcl_ComboBox_Dir DwgList_DwgListForm_ComboBox1 sPath)
    (dcl_DwgList_Dir DwgList_DwgListForm_DwgList1 sPath)
    (princ)
)



(defun c:DwgList_DwgListForm_cmdDemo_OnClicked ( /)
  (dcl_Form_Close DwgList_DwgListForm)
  (if (or (vl-symbol-value 'c:Demo) (load "_MasterDemo"))
     (c:Demo)
  )
)

(princ)