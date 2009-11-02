;;;
;;; ListBox Sample
;;;
;;; This sample demonstrates the Listbox control and all its events.
;;;

;; Main program
(defun c:ListBox (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl_Project_Load (*ODCL:Samples:FindFile "ListBox.odcl"))

	;; Show the main form
	(dcl_Form_Show listbox_form1)

	;; This is a modal form, so (dcl_Form_Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

;;----------------------------------------------------
(DEFUN c:listbox_form1_selectfiles_onclicked (/ bs_filelist fn)
	(SETQ bs_filelist (DCL_MULTIFILEDIALOG "Drawing files (*.dwg)|*.dwg||" ; Filters
										   "Select files" ; Title
										   (IF g:lastfolder
											   g:lastfolder
											   (GETVAR "DWGPREFIX")
										   ) ; Default Folder
					  )
	)
	(IF bs_filelist
		(PROGN
			(SETQ g:lastfolder (VL-FILENAME-DIRECTORY (CAR bs_filelist)))
			(FOREACH fn bs_filelist
				(IF (< (DCL_LISTBOX_FINDSTRINGEXACT ListBox_Form1_ListBox fn) 0)
					(DCL_LISTBOX_ADDSTRING ListBox_Form1_ListBox fn)
				)
			)
		)
	)
)
;;----------------------------------------------------
(DEFUN c:listbox_form1_clearlist_onclicked (/)
	(DCL_LISTBOX_CLEAR listbox_form1_listbox)
)
;;----------------------------------------------------
(DEFUN c:listbox_form1_remove_onclicked (/ rsel item)
	(IF (SETQ rsel (DCL_LISTBOX_GETSELECTEDNTHS ListBox_Form1_ListBox))
		(PROGN
			(FOREACH item (REVERSE rsel)
				(DCL_LISTBOX_DELETEITEM ListBox_Form1_ListBox item)
			)
			(SETQ g:bs_filelist '())
			(SETQ item (DCL_LISTBOX_GETCOUNT ListBox_Form1_ListBox))
			(WHILE (>= (SETQ item (1- item)) 0)
				(SETQ
					g:bs_filelist
					(CONS
						(DCL_LISTBOX_GETITEMTEXT ListBox_Form1_ListBox item)
						g:bs_filelist
					)
				)
			)
		)
	)
)
;;----------------------------------------------------
(DEFUN c:listbox_form1_exitdialog_onclicked (/) (DCL_FORM_CLOSE listbox_form1))

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
	"ListBox"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
