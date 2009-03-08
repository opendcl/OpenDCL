(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"Modeless\" ein, um das Beispiel zu starten.\n")
)


(DEFUN c:Modeless ( /  )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Modeless.odcl")
  (dcl_FORM_SHOW Modeless_DemoModeless)
  (PRINC)
)




(defun c:DemoModeless_cmdDrawLine_OnClicked ()
  (command "._LINE" "0,0" "10,10" "")
)

(defun c:DemoModeless_cmdZoomWin_OnClicked ()
  (command "._ZOOM" "_W" PAUSE PAUSE)
)

(defun c:DemoModeless_cmdZoomExt_OnClicked ()
  (command "._ZOOM" "_E" )
)


(defun c:DemoModeless_CloseButton_Clicked ()
   (dcl_Form_Close Modeless_DemoModeless)   
)

(princ)

