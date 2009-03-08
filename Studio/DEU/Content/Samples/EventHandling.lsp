(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"Events\" ein, um das Beispiel zu starten.\n")
)


(DEFUN c:Events (/)
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "EventHandling.odcl")
  (dcl_FORM_SHOW EventHandling_EventHandling)
  ;; The Event handlers manage the form here.   
  (PRINC)
)



(defun c:EventHandling_TextBox1_EditChanged (sText /)
  (setq sVariable sText)
)

(defun c:EventHandling_OK_Clicked ()
  (if (null sVariable)
    (setq sVariable "ist leer")
  )
  (alert (strcat "sVariable=" sVariable))
)

(princ)
