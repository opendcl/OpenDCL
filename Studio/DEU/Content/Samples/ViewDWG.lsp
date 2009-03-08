(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"ViewDWG\" ein, um das Beispiel zu starten.\n")
)

;; this function loads the project & shows the form
(DEFUN c:ViewDwg ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "ViewDWG.odcl")
  (dcl_FORM_SHOW ViewDWG_DwgPreview)
  ;; The Event handlers manage the form here. 
  (PRINC)
)


(defun c:DwgPreview_Cancel_Clicked ()
   (dcl_Form_Close ViewDWG_DwgPreview)
)

(defun c:DwgPreview_Browse_Clicked ( / )
   (setq sFileName (GetFiled "Eine Zeichnung wählen" "" "dwg" 8));_ get the path to a dwg file
   (if sFileName
     (progn
       (dcl_DwgPreview_LoadDwg ViewDWG_DwgPreview_ViewDwg sFileName);_ load the DWG Thumbnail preview into the control
       (dcl_BlockView_LoadDwg ViewDWG_DwgPreview_BlockView1 sFileName);_ load the DWG
       (dcl_Control_SetCaption ViewDWG_DwgPreview_Label1 (dcl_DwgPreview_GetDwgName ViewDWG_DwgPreview_ViewDwg));_ show the path of the DWG
       (dcl_Control_SetFocus ViewDWG_DwgPreview_BlockView1)
     )
   )
)
(princ)
