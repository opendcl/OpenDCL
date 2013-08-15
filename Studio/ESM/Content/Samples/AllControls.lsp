;; ALLCONTROLS
;; 2009-03-14
;; 
;; Displays an OpenDCL palette form containing one of every OpenDCL control
;;
;; Author: Owen Wengerd (owenw@manusoft.com)
;;
;;
;; This file is placed in the public domain, and may be redistributed or used
;; for any purpose. Changes to this file should be clearly identified as such.
;; 
;;


(defun c:AllControls_Form1_OnInitialize (/ parent)
  (setq parent (dcl_Tree_AddParent AllControls_Form1_TreeControl1 "Tree Control" 0 -1 1))
  (dcl_Tree_AddChild AllControls_Form1_TreeControl1
    (list (list parent "Child 1" 2) (list parent "Child 2" 2) (list parent "Child 3" 2)))
  (dcl_Tree_AddParent AllControls_Form1_TreeControl1 '(("Item 2" 0 -1 1) ("Item 3" 0 -1 1)))
  (dcl_ListView_AddColumns AllControls_Form1_ListView1
    '(("List View" 0 100) ("Col 2" 0 50) ("Col 3" 0 50)))
  (dcl_ListView_AddString AllControls_Form1_ListView1
    "Item 1\tItem 2\tItem 3")
  (dcl_Grid_AddString AllControls_Form1_Grid1
    "Item 1\tItem 2\tItem 3")
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
  (setq parent (dcl_Tree_AddParent AllControls_Form2_TreeControl1 "Tree Control" 0 -1 1))
  (dcl_Tree_AddChild AllControls_Form2_TreeControl1
    (list (list parent "Child 1" 2) (list parent "Child 2" 2) (list parent "Child 3" 2)))
  (dcl_Tree_AddParent AllControls_Form2_TreeControl1 '(("Item 2" 0 -1 1) ("Item 3" 0 -1 1)))
  (dcl_ListView_AddColumns AllControls_Form2_ListView1
    '(("List View" 0 100) ("Col 2" 0 50) ("Col 3" 0 50)))
  (dcl_ListView_AddString AllControls_Form2_ListView1
    "Item 1\tItem 2\tItem 3")
  (dcl_Grid_AddString AllControls_Form2_Grid1
    "Item 1\tItem 2\tItem 3")
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

(princ "\nALLCONTROLS\n") ; show the user the name of the command that is running
(C:ALLCONTROLS) ; run the command
(if dcl_SendString (dcl_SendString "ALLCONTROLS\n")) ; put the command in the command history

(princ)
