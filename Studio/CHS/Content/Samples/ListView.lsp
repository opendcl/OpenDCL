;;;
;;; List View Sample
;;;
;;; This sample demonstrates the List View control and all its events.
;;;

;; Main program
(defun c:ListView (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl_Project_Load (*ODCL:Samples:FindFile "ListView.odcl"))

	;; Show the main form
	(dcl_Form_Show ListView_ListViewDlg)

	;; This is a modal form, so (dcl_Form_Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

;; This function will run everytime the form is loaded into AutoCAD
(defun c:ListViewDlg_OnInitialize ( / Col0Width Col1Width Col2Width Col3Width)
	;; calculate the required column widths
	(setq Col0Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Column 0     ")
		  Col1Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Column 1     ")
		  Col2Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Column 2     ")
		  Col3Width (dcl_ListView_CalcColWidth ListView_ListViewDlg_ListView1 "Column 3     ")
	)
	;; add columns of the calculated widths
	(dcl_ListView_AddColumns ListView_ListViewDlg_ListView1
							  (list (list "Column 0" 0 Col0Width)
									(list "Column 1" 0 Col1Width)
									(list "Column 2" 0 Col2Width)
									(list "Column 3" 0 Col3Width)
							  )
	)
	;; clear and fill the list
	(dcl_ListView_FillList
		ListView_ListViewDlg_ListView1
		(list ;; add row 1 items
			  (list "List 1a" 0 "List 2a" 0 "List 3a" 0 "List 4a" 0)
			  ;; add row 2 items
			  (list "List 1b" 1 "List 2b" 1 "List 3b" 1 "List 4b" 1)
			  ;; add row 3 items
			  (list "List 1c" 2 "List 2c" 2 "List 3c" 2 "List 4c" 2)
		)
	)
)

;;-----------------------------------------------------------

(defun c:ListView_ListViewDlg_ListView1_OnClicked (nRow nCol /)
	 (dcl_MessageBox (strcat "Column " (itoa nCol) ", Row " (itoa nRow))
					  "Clicked Location"
	 )
)

;;-----------------------------------------------------------

;; the user has clicked the "Close" button, so lets close the dialog now.
(defun c:ListViewDlg_Close_Clicked () (dcl_Form_Close ListView_ListViewDlg))

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
	"ListView"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
