(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"ViewDWG\" ein, um das Beispiel zu starten.\n")
)

;; Diese Funktion lädt das Projekt und zeigt den Dialog an
(DEFUN c:ViewDwg ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "ViewDWG.odcl")
    
;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog. 
  (dcl_FORM_SHOW ViewDWG_DwgPreview)
  (PRINC)
)


(defun c:DwgPreview_Cancel_Clicked ()
   (dcl_Form_Close ViewDWG_DwgPreview)
)

(defun c:DwgPreview_Browse_Clicked ( / )
   (setq sFileName (GetFiled "Eine Zeichnung wählen" "" "dwg" 8));_ Zeichnung wählen und kompletten Pfad erhalten
   (if sFileName
     (progn
       (dcl_DwgPreview_LoadDwg ViewDWG_DwgPreview_ViewDwg sFileName);_ Zeichnungsvorschau in das Steuerlement laden
       (dcl_BlockView_LoadDwg ViewDWG_DwgPreview_BlockView1 sFileName);_ Zeichnung selbst laden
       (dcl_Control_SetCaption ViewDWG_DwgPreview_Label1 (dcl_DwgPreview_GetDwgName ViewDWG_DwgPreview_ViewDwg));_ Pfad der Zeichnungsdatei anzeigen
       (dcl_Control_SetFocus ViewDWG_DwgPreview_BlockView1)
     )
   )
)
(princ)
