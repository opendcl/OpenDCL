(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL sample programs.\nEnter \"TOOL\" to run the sample.\n")
)

(defun c:Tool ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "ToolTip.odcl")
  (dcl_FORM_SHOW ToolTip_dclToolTip)
   ;; The Event handlers manage the form here. 
  (PRINC)
)

(defun c:ToolTip_dclToolTip_OnInitialize ( /)
  (dcl_ComboBox_SetCurSel ToolTip_dclToolTip_cboColor 1)
  (dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "<None>" -1 -1)
  (dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "Question" 1 1)
  (dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "Information" 0 0)
  (dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "Exclamation" 2 2)
  (dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "Error" 3 3)
  (dcl_ImageComboBox_SetCurSel ToolTip_dclToolTip_cboIcon 0)
)


(defun c:ToolTip_dclToolTip_cmdOK_OnClicked ( / AviPath)

;;;  (if (dcl_Control_GetValue ToolTip_dclToolTip_chkAVI)
;;;    (progn
;;;      (setq AviPath (findfile "FileCopy.avi"))
;;;      (if AviPath
;;;        (dcl_Control_SetToolTipAviFileName  ToolTip_dclToolTip_cmdOK AviPath)
;;;        (alert "Sorry can't find FileCopy.avi, make sure it's in a support path")
;;;      );_ if
;;;    );_ progn
;;;    (dcl_Control_SetToolTipAviFileName ToolTip_dclToolTip_cmdOK nil)
;;;  ); _if

  (dcl_Control_SetToolTipTitle ToolTip_dclToolTip_cmdOK (dcl_Control_GetText ToolTip_dclToolTip_txtTitle))   
  (dcl_Control_SetToolTipMainText ToolTip_dclToolTip_cmdOK (dcl_Control_GetText ToolTip_dclToolTip_txtMain))
  (dcl_Control_SetToolTipLine ToolTip_dclToolTip_cmdOK (= 1 (dcl_Control_GetValue ToolTip_dclToolTip_chkLine)))
  (dcl_Control_SetToolTipTitleColor ToolTip_dclToolTip_cmdOK (dcl_ComboBox_GetItemData ToolTip_dclToolTip_cboColor (dcl_ComboBox_GetCurSel ToolTip_dclToolTip_cboColor)))
  (dcl_Control_SetToolTipPicture ToolTip_dclToolTip_cmdOK (- (dcl_ImageComboBox_GetCurSel ToolTip_dclToolTip_cboIcon)))
  (dcl_Control_SetToolTipBalloon ToolTip_dclToolTip_cmdOK (= 1 (dcl_Control_GetValue ToolTip_dclToolTip_chkBalloon)))

  (dcl_Control_ShowToolTip ToolTip_dclToolTip_cmdOK)
    
)


(princ)