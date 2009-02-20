(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL sample programs.\nEnter \"MsgBox\" to run the sample.\n")
)

(DEFUN c:MsgBox (/ )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit)) 
  (LoadRunTime)
  (LoadODCLProj "MessageBox.odcl")
  (dcl_FORM_SHOW MessageBox_MessageBoxes)
  ;; The Event handlers manage the form here. 
  (PRINC)
)


(defun c:MessageBox_MessageBoxes_ShowMessageBox_OnClicked ( / OptIconCSel OptButtonCSel msg title
					                      ButtonFlag IconFlag HelpFlag
					                  )


  (Setq OptIconCSel (dcl_OptionList_GetCurSel MessageBox_MessageBoxes_optIcons))
  (Setq OptButtonCSel (dcl_OptionList_GetCurSel MessageBox_MessageBoxes_optButtons))
  (setq HelpFlag (= 1 (dcl_Control_GetValue MessageBox_MessageBoxes_HelpButton)))
  (Setq msg (dcl_Control_GetText MessageBox_MessageBoxes_MsgText))
  (Setq Title (dcl_Control_GetText MessageBox_MessageBoxes_MsgTitle))

  (dcl_MessageBox msg title (1+ OptButtonCSel) OptIconCSel HelpFlag)
)

(defun c:MessageBox_MessageBoxes_cmdClose_OnClicked ()
    (dcl_Form_Close MessageBox_MessageBoxes)
)

(defun c:MessageBox_MessageBoxes_OnHelp ( /)
     (dcl_MessageBox (strcat "You requested help for this form!\r\n\r\n"
                              "Note that pressing the [Help] button in a child message box generates "
                              "a call to the OnHelp event handler in the message box parent form, and "
                              "the message box remains on the screen.") "MsgBox Sample")
)

(princ)
