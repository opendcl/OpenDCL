;;;
;;; Message Box Sample
;;;
;;; This sample demonstrates the (dcl-MessageBox) function.
;;;

;; Main program
(defun c:MsgBox (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl-Project-Load (*ODCL:Samples:FindFile "MessageBox.odcl"))

	;; Show the main form
	(dcl-Form-Show MessageBox/MessageBoxes)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:MessageBox/MessageBoxes/ShowMessageBox#OnClicked
		(/ OptIconCSel OptButtonCSel msg title ButtonFlag IconFlag HelpFlag)
	(Setq OptIconCSel
			 (dcl-OptionList-GetCurSel MessageBox/MessageBoxes/optIcons)
	)
	(Setq OptButtonCSel (dcl-OptionList-GetCurSel
							MessageBox/MessageBoxes/optButtons
						)
	)
	(setq HelpFlag
			(= 1 (dcl-Control-GetValue MessageBox/MessageBoxes/HelpButton))
	)
	(Setq msg (dcl-Control-GetText MessageBox/MessageBoxes/MsgText))
	(Setq Title (dcl-Control-GetText MessageBox/MessageBoxes/MsgTitle))
	(dcl-MessageBox msg title (1+ OptButtonCSel) OptIconCSel HelpFlag)
)

(defun c:MessageBox/MessageBoxes/cmdClose#OnClicked ()
	(dcl-Form-Close MessageBox/MessageBoxes)
)

(defun c:MessageBox/MessageBoxes#OnHelp (/)
	(dcl-MessageBox
		(strcat
			"You requested help for this form!\r\n\r\n"
			"Note that pressing the [Help] button in a child message box generates "
			"a call to the OnHelp event handler in the message box parent form, and "
			"the message box remains on the screen."
		)
		"MsgBox Sample"
	)
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
	"MsgBox"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
