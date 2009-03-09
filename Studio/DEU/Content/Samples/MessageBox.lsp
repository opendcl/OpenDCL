(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"MsgBox\" ein, um das Beispiel zu starten.\n")
)

(DEFUN c:MsgBox (/ )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit)) 
  (LoadRunTime)
  (LoadODCLProj "MessageBox.odcl")
  
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog. 
  (dcl_FORM_SHOW MessageBox_MessageBoxes)
    
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
     (dcl_MessageBox (strcat "Sie haben für diesen Dialog eine Hilfe angefordert!\r\n\r\n"
                              "Beachten Sie bitte, dass das Drücken der Hilfe-Schaltfläche in einem untergeordneten"
			      "Meldungsdialog das Ereignis OnHelp in der übergeordneten Dialogbox auslöst und"
			      "der Meldungssdialog am Bildschirm stehen bleibt.") "MsgBox Beispiel")
)

(princ)
