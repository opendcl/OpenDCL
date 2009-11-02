;;;
;;; Event Sample
;;;
;;; This sample demonstrates basic event handling.
;;;

;; Main program
(defun c:Events (/ cmdecho result)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl_Project_Load (*ODCL:Samples:FindFile "EventHandling.odcl"))

	;; Show the main form
	(setq result (dcl_Form_Show EventHandling_Main))

	;; This is a modal form, so (dcl_Form_Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form. The return value from (dcl_Form_Show) is used
	;; to determine what action to take next.

	(if (= result 10)
		(dcl_Form_Show EventHandling_Congrats)
	)

	(princ)
)

;; ask user if they're sure they want to give up
;; return T if yes, NIL if no
(defun EventHandling_PromptClose (/)
	(=
		6 ; 'Yes' result
		(dcl_MessageBox
			"Here's a hint: the answer has three letters. Still want to give up?"
			"OpenDCL Samples"
			15 ; Yes/No with 'No' as default
			3 ; 'question' icon
		)
	)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:EventHandling_Main_OnInitialize (/)
	(dcl_Control_SetText EventHandling_Main_txtPassword "")
	(princ)
)

(defun c:EventHandling_Main_txtPassword_OnEditChanged (sText /)
	(if (= (strcase sText) "TON")
		(dcl_Form_Close EventHandling_Main 10) ; return 10 to signal a correct answer
	)
	(princ)
)

(defun c:EventHandling_Main_btnGiveUp_OnClicked ()
	(if (EventHandling_PromptClose)
		(dcl_Form_Close EventHandling_Main)
	)
	(princ)
)

(defun c:EventHandling_Main_OnCancelClose (Canceling /)
	;; if this event handler returns T, the dialog will not close
	(not (EventHandling_PromptClose))
)

(defun c:EventHandling_Congrats_btnExit_OnClicked ()
	(dcl_Form_Close EventHandling_Congrats)
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
	"Events"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
