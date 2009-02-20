(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL sample programs.\nEnter \"ConfigTab\" to run the sample.\n ")
)

(DEFUN c:ConfigTab ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Config.odcl")

  (dcl_MESSAGEBOX "Please navigate to the \"Demo Config Page\" tab" "OpenDCL")
  (COMMAND "._OPTIONS")
  (PRINC)
)


(defun c:DemoConfigPage_OnOK ( / tmp)
  (dcl_MessageBox (strcat "You typed: " (dcl_Control_GetText Config_DemoConfigPage_txtOptTabDemo)) "OpenDCL")
  (setq tmp (dcl_Control_GetValue Config_DemoConfigPage_chkOptTabDemo))
  (if (= 1 tmp)
    (setq OnOff "ON")
    (setq OnOff "OFF")
  )
  (dcl_MessageBox (strcat "The check box is set to: " OnOff))
)

(princ)
