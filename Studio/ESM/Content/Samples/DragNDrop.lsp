;;;
;;; ListView and ListBox DragNDrop Sample
;;;
;;; This sample demonstrates the DragnDrop Methods and Events.
;;;

;; Main program
(defun c:DragNDrop (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl_Project_Load (*ODCL:Samples:FindFile "DragNDrop.odcl"))

	;; Show the main form
	(dcl_Form_Show DragNDrop_Form1)

	;; This is a modeless form, so (dcl_Form_Show) returns immediately,
	;; leaving the event handlers to manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:DragNDrop_Form1_ListView1_OnDragnDropToAutoCAD (3dPoint Viewport / rValue)
	(Setq rValue (dcl_ListView_GetSelectedItems DragNDrop_Form1_ListView1))
	(dcl_MessageBox (strcat "You dropped \""
							(car rValue)
							"\" at ("
							(rtos (car 3dPoint) 2 4)
							", "
							(rtos (cadr 3dPoint) 2 4)
							", "
							(rtos (caddr 3dPoint) 2 4)
							") in viewport number "
							(itoa Viewport)
							"."
					)
					"Drag-N-Drop"
	)
	(princ)
)

(defun c:DragNDrop_Form1_OnInitialize (/)
	(dcl_ListView_AddColumns DragNDrop_Form1_ListView1 (list "Column 1" 0 140))
	(dcl_ListView_FillList DragNDrop_Form1_ListView1
						   '(("List 1a" 0) ("List 2a" 1) ("List 3a" 2))
	)
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
	"DragNDrop"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
