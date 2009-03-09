(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"DRAG\" ein, um das Beispiel zu starten.\n")
)


(defun c:Drag ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "DragNDrop.odcl")
  
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog. 
  (dcl_FORM_SHOW DragNDrop_Form1)
  (PRINC)
)


(defun c:DragNDrop_DclForm1_ListView1_OnDragnDropToAutoCAD (3dPoint intViewPoint /)
  (Setq rValue (dcl_ListView_GetSelectedItems DragNDrop_Form1_ListView1))
  (dcl_MessageBox (strcat "Sie haben \"" (car rValue) "\" ins AutoCAD-Fenster ins Ansichtsfenster Nummer " (itoa intViewPoint)
                          "\nan den Punkt " (rtos (car 3dPoint) 2 4) "," (rtos (cadr 3dPoint) 2 4) " verschoben.") "Drag&Drop")
  (princ)
)



(defun c:DragNDrop_Form1_OnInitialize ( /)
  (dcl_ListView_AddColumns DragNDrop_Form1_ListView1
    (list "Spalte 1" 0 140)
  )
  (dcl_ListView_FillList DragNDrop_Form1_ListView1
    '(("Reihe 1a" 0) ("Reihe 2a" 1) ("Reihe 3a" 2))
  )
  (princ)
)


(princ)