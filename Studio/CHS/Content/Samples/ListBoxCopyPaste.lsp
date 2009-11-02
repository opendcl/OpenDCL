;;;
;;; ListBox Copy/Paste Sample
;;;
;;; This sample demonstrates copying and pasting items in a ListBox control.
;;;

;; Main program
(defun c:CopyPaste (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl_Project_Load (*ODCL:Samples:FindFile "ListBoxCopyPaste.odcl"))

	;; Show the main form
	(dcl_Form_Show ListBoxCopyPaste_Form)

	;; This is a modal form, so (dcl_Form_Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:ListBoxCopyPaste_Form_TextButton1_OnClicked (/ items len DelLst x cnt)
	;; Get the selection from the left list box
	(setq items (dcl_ListBox_GetSelectedItems ListBoxCopyPaste_Form_ListBox1))
	(setq len (length items))
	(if (> len 0)
		;; if there's one or more items selected then...
		(progn
			;; query the status of the check box and if ticked,
			;; continue to append items to the right list box
			(if (/= 1 (dcl_Control_GetValue ListBoxCopyPaste_Form_CheckBox1))
				(dcl_ListBox_Clear ListBoxCopyPaste_Form_ListBox2)
			)
			;; and add the selected items to the right list box
			(dcl_ListBox_AddList ListBoxCopyPaste_Form_ListBox2 items)
			;; query the status of the check box and if ticked,
			;; remove items to the left list box
			(if (= 1 (dcl_Control_GetValue ListBoxCopyPaste_Form_CheckBox2))
				(progn
					(Setq DelLst (dcl_ListBox_GetSelectedNths
									 ListBoxCopyPaste_Form_ListBox1
								 )
					)
					(Setq cnt 0)
					(while (< cnt (length DelLst))
						(setq x (nth cnt DelLst))
						(dcl_ListBox_DeleteString ListBoxCopyPaste_Form_ListBox1
												  (- x cnt)
						)
						(setq cnt (1+ cnt))
					)
				)
			)
		)
	)
)

;; if the user double clicks on items in the the left hand list
;; we perform the same action as if the button was clicked...
(defun c:ListBoxCopyPaste_Form_ListBox1_OnDblClicked (/)
	;; run the function above...
	(c:ListBoxCopyPaste_Form_TextButton1_OnClicked)
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
	"CopyPaste"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
