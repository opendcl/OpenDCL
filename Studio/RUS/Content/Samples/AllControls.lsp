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

(defun C:ALLCONTROLS (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl-Project-Load (*ODCL:Samples:FindFile "AllControls.odcl"))

	;; Show the main form
	(dcl-Form-Show AllControls/Form1)

	(princ)
)

(defun C:ALLCONTROLS2 (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project and show the alternate form
	(and
		(dcl-Project-Load (*ODCL:Samples:FindFile "AllControls.odcl"))
		(not (dcl-Form-IsActive AllControls/Form2))
		(dcl-Form-show AllControls/Form2)
	)

	(princ)
)

(defun c:AllControls/Form1#OnInitialize (/ parent)
  (setq parent (dcl-Tree-AddParent AllControls/Form1/TreeControl1 "Tree Control" 0 -1 1))
  (dcl-Tree-AddChild AllControls/Form1/TreeControl1
    (list (list parent "Child 1" 2) (list parent "Child 2" 2) (list parent "Child 3" 2)))
  (dcl-Tree-AddParent AllControls/Form1/TreeControl1 '(("Item 2" 0 -1 1) ("Item 3" 0 -1 1)))
  (dcl-ListView-AddColumns AllControls/Form1/ListView1
    '(("List View" 0 100) ("Col 2" 0 50) ("Col 3" 0 50)))
  (dcl-ListView-AddString AllControls/Form1/ListView1
    "Item 1\tItem 2\tItem 3")
  (dcl-Grid-AddString AllControls/Form1/Grid1
    "Item 1\tItem 2\tItem 3")
  (dcl-Animation-Load AllControls/Form1/Animation1 (*ODCL:Samples:FindFile "AllControls.avi"))
  (dcl-BlockView-PreLoadDwg AllControls/Form1/BlockView1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-BlockView-DisplayBlockToScale AllControls/Form1/BlockView1 "Block List")
  (dcl-SlideView-Load AllControls/Form1/SlideView1 (*ODCL:Samples:FindFile "AllControls.sld"))
  (dcl-Html-UpdateHtmlCode AllControls/Form1/Html1
    "<HTML><HEAD></HEAD><BODY><h4>Html</h4></BODY></HTML>")
  (dcl-DWGPreview-LoadDwg AllControls/Form1/DwgPreview1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-BlockList-LoadDwg AllControls/Form1/BlockList1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-Hatch-SetPattern AllControls/Form1/Hatch1 "LINE")
  (princ)
)

(defun c:AllControls/Form2#OnInitialize (/ parent)
  (setq parent (dcl-Tree-AddParent AllControls/Form2/TreeControl1 "Tree Control" 0 -1 1))
  (dcl-Tree-AddChild AllControls/Form2/TreeControl1
    (list (list parent "Child 1" 2) (list parent "Child 2" 2) (list parent "Child 3" 2)))
  (dcl-Tree-AddParent AllControls/Form2/TreeControl1 '(("Item 2" 0 -1 1) ("Item 3" 0 -1 1)))
  (dcl-ListView-AddColumns AllControls/Form2/ListView1
    '(("List View" 0 100) ("Col 2" 0 50) ("Col 3" 0 50)))
  (dcl-ListView-AddString AllControls/Form2/ListView1
    "Item 1\tItem 2\tItem 3")
  (dcl-Grid-AddString AllControls/Form2/Grid1
    "Item 1\tItem 2\tItem 3")
  (dcl-Animation-Load AllControls/Form2/Animation1 (*ODCL:Samples:FindFile "AllControls.avi"))
  (dcl-BlockView-PreLoadDwg AllControls/Form2/BlockView1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-BlockView-DisplayBlockToScale AllControls/Form2/BlockView1 "Block List")
  (dcl-SlideView-Load AllControls/Form2/SlideView1 (*ODCL:Samples:FindFile "AllControls.sld"))
  (dcl-Html-UpdateHtmlCode AllControls/Form2/Html1
    "<HTML><HEAD></HEAD><BODY><h4>Html</h4></BODY></HTML>")
  (dcl-DWGPreview-LoadDwg AllControls/Form2/DwgPreview1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-BlockList-LoadDwg AllControls/Form2/BlockList1 (*ODCL:Samples:FindFile "AllControls.dwg"))
  (dcl-Hatch-SetPattern AllControls/Form2/Hatch1 "LINE")
  (princ)
)

(princ)

;|<<OpenDCL Samples Epilog>>|;

;;;######################################################################
;;;######################################################################
;;; The following section of code is designed to locate OpenDCL Studio
;;; sample files in the samples folder by prefixing the filename with
;;; the path prefix that was saved in the registry by the installer.
;;; The global *ODCL:Prefix and function *ODCL:Samples:FindFile
;;; are used throughout the samples.
;;;
(or *ODCL:Samples:FindFile
	(defun *ODCL:Samples:FindFile (file)
		(setq *ODCL:Prefix
			(cond
				(	*ODCL:Prefix
				) ;_ already defined
				(	(vl-registry-read
						 "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 32-bit location
				(	(vl-registry-read
						 "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 32-bit location
				(	(vl-registry-read
						 "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 64-bit location
				(	(vl-registry-read
						 "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 64-bit location
			)
		)
		(cond
			((findfile file)) ; check the support path first
			(*ODCL:Prefix (findfile (strcat *ODCL:Prefix file)))
			(file)
		)
	)
)

;; If master demo is active, run the main function immediately; otherwise
;; display a banner. The extra gymnastics allow the sample name to be
;; specified in only one place, thus making it easier to reuse this code.
(	(lambda (demoname)
		(if *ODCL:MasterDemo
			(progn
				(princ (strcat "'" demoname "\n"))
				(apply (read (strcat "C:" demoname)) nil)
			)
			(progn
				(princ (strcat "\n" demoname " OpenDCL sample loaded"))
				(princ (strcat " (Enter " (strcase demoname) " command to run)\n"))
			)
		)
	)
	"AllControls"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
