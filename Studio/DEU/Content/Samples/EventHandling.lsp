(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"Events\" ein, um das Beispiel zu starten.\n")
)


(DEFUN c:Events (/)
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "EventHandling.odcl")
  
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog.   
  (dcl_FORM_SHOW EventHandling_EventHandling)
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
