(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL sample programs.\nEnter \"DRAG\" to run the sample.\n ")
)


(defun c:Drag ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "DragNDrop.odcl")
  (dcl_FORM_SHOW DragNDrop_Form1)
  ;; The Event handlers manage the form here. 
  (PRINC)
)


(defun c:DragNDrop_DclForm1_ListView1_OnDragnDropToAutoCAD (3dPoint /)
  (Setq rValue (dcl_ListView_GetSelectedItems DragNDrop_Form1_ListView1))
  (dcl_MessageBox (strcat "You drug \"" (car rValue) "\" into the Acad window\nand dropped it at " (rtos (car 3dPoint) 2 4) "," (rtos (cadr 3dPoint) 2 4)) "Drag-N-Drop")
  (princ)
)



(defun c:DragNDrop_Form1_OnInitialize ( /)
  (dcl_ListView_AddColumns DragNDrop_Form1_ListView1
    (list "Column 1" 0 140)
  )
  (dcl_ListView_FillList DragNDrop_Form1_ListView1
    '(("List 1a" 0) ("List 2a" 1) ("List 3a" 2))
  )
  (princ)
)


(princ)