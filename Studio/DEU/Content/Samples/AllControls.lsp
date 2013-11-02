;; ALLCONTROLS
;; 2009-03-14
;; 
;; Zeigt eine OpenDCL Palette mit allen OpenDCL Steuerelementen
;;
;; Author: Owen Wengerd (owenw@manusoft.com)
;;
;;
;; Diese Datei wird als frei verfügbar bereitgestellt und kann weitergegeben oder für jeden 
;; anderen Zweck genutzt werden. Änderungen an dieser Datei müssen klar als solche erkennbar sein.
;; 
;;


(defun c:AllControls_Form1_OnInitialize (/ parent)
  (setq parent (dcl_Tree_AddParent AllControls_Form1_TreeControl1 "Baumstruktur" 0 -1 1))
  (dcl_Tree_AddChild AllControls_Form1_TreeControl1
    (list (list parent "Element 1" 2) (list parent "Element 2" 2) (list parent "Element 3" 2)))
  (dcl_Tree_AddParent AllControls_Form1_TreeControl1 '(("Element 2" 0 -1 1) ("Element 3" 0 -1 1)))
  (dcl_ListView_AddColumns AllControls_Form1_ListView1
    '(("Liste" 0 100) ("Spalte 2" 0 50) ("Spalte 3" 0 50)))
  (dcl_ListView_AddString AllControls_Form1_ListView1
    "Element 1\tElement  2\tElement  3")
  (dcl_Grid_AddString AllControls_Form1_Grid1
    "Element 1\tElement 2\tElement 3")
  (dcl_Animation_Load AllControls_Form1_Animation1 "AllControls.avi")
  (dcl_BlockView_PreLoadDwg AllControls_Form1_BlockView1 "AllControls.dwg")
  (dcl_BlockView_DisplayBlockToScale AllControls_Form1_BlockView1 "Block List")
  (dcl_SlideView_Load AllControls_Form1_SlideView1 "AllControls.sld")
  (dcl_Html_UpdateHtmlCode AllControls_Form1_Html1
    "<HTML><HEAD></HEAD><BODY><h4>Html</h4></BODY></HTML>")
  (dcl_DWGPreview_LoadDwg AllControls_Form1_DwgPreview1 "AllControls.dwg")
  (dcl_BlockList_LoadDwg AllControls_Form1_BlockList1 "AllControls.dwg")
  (dcl_Hatch_SetPattern AllControls_Form1_Hatch1 "LINE")
  (princ)
)



(defun c:AllControls_Form2_OnInitialize (/ parent)
  (setq parent (dcl_Tree_AddParent AllControls_Form2_TreeControl1 "Baumstruktur" 0 -1 1))
  (dcl_Tree_AddChild AllControls_Form2_TreeControl1
    (list (list parent "Element 1" 2) (list parent "Element 2" 2) (list parent "Element 3" 2)))
  (dcl_Tree_AddParent AllControls_Form2_TreeControl1 '(("Element 2" 0 -1 1) ("Element 3" 0 -1 1)))
  (dcl_ListView_AddColumns AllControls_Form2_ListView1
    '(("Liste" 0 100) ("Spalte 2" 0 50) ("Spalte 3" 0 50)))
  (dcl_ListView_AddString AllControls_Form2_ListView1
    "Element 1\tElement 2\tElement 3")
  (dcl_Grid_AddString AllControls_Form2_Grid1
    "Element 1\tElement 2\tElement 3")
  (dcl_Animation_Load AllControls_Form2_Animation1 "AllControls.avi")
  (dcl_BlockView_PreLoadDwg AllControls_Form2_BlockView1 "AllControls.dwg")
  (dcl_BlockView_DisplayBlockToScale AllControls_Form2_BlockView1 "Block List")
  (dcl_SlideView_Load AllControls_Form2_SlideView1 "AllControls.sld")
  (dcl_Html_UpdateHtmlCode AllControls_Form2_Html1
    "<HTML><HEAD></HEAD><BODY><h4>Html</h4></BODY></HTML>")
  (dcl_DWGPreview_LoadDwg AllControls_Form2_DwgPreview1 "AllControls.dwg")
  (dcl_BlockList_LoadDwg AllControls_Form2_BlockList1 "AllControls.dwg")
  (dcl_Hatch_SetPattern AllControls_Form2_Hatch1 "LINE")
  (princ)
)


(defun C:ALLCONTROLS (/ cmdecho)
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)
  (and
    (dcl_project_load "AllControls")
    (not (dcl_Form_IsActive AllControls_Form1))
    (dcl_form_show AllControls_Form1)
  )
  (princ)
)


(defun C:ALLCONTROLS2 (/ cmdecho)
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)
  (and
    (dcl_project_load "AllControls")
    (not (dcl_Form_IsActive AllControls_Form2))
    (dcl_form_show AllControls_Form2)
  )
  (princ)
)

(princ "\nALLCONTROLS\n") ; zeigt dem Anwender den Namen des Befehls an
(C:ALLCONTROLS) ; Startet das Programm
(if dcl_SendString (dcl_SendString "ALLCONTROLS\n")) ; Fügt den Befehl zur Liste verwendeter Befehle hinzu.

(princ)
