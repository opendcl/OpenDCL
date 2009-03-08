(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"ConfigTab\" ein, um das Beispiel zu starten.\n")
)

(DEFUN c:ConfigTab ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Config.odcl")

  (dcl_MESSAGEBOX "Bewegen Sie sich bitte zur Karteikarte \"OpenDCL Demokarteikarte\"" "OpenDCL")
  (COMMAND "._OPTIONS")
  (PRINC)
)


(defun c:DemoConfigPage_OnOK ( / tmp)
  (dcl_MessageBox (strcat "Ihre Eingabe: " (dcl_Control_GetText Config_DemoConfigPage_txtOptTabDemo)) "OpenDCL")
  (setq tmp (dcl_Control_GetValue Config_DemoConfigPage_chkOptTabDemo))
  (if (= 1 tmp)
    (setq OnOff "EIN")
    (setq OnOff "AUS")
  )
  (dcl_MessageBox (strcat "Die Einstellung des Kontrollkästchen ist: " OnOff))
)

(princ)
