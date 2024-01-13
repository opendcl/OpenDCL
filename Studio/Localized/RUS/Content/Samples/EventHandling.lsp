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
	(dcl-Project-Load (*ODCL:Samples-FindFile "EventHandling.odcl"))

	;; Show the main form
	(setq result (dcl-Form-Show EventHandling/Main))

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form. The return value from (dcl-Form-Show) is used
	;; to determine what action to take next.

	(if (= result 10)
		(dcl-Form-Show EventHandling/Congrats)
	)

	(princ)
)

;; ask user if they're sure they want to give up
;; return T if yes, NIL if no
(defun EventHandling_PromptClose (/)
	(=
		6 ; 'Yes' result
		(dcl-MessageBox
			"Here's a hint: the answer has three letters. Still want to give up?"
			"OpenDCL Samples"
			15 ; Yes/No with 'No' as default
			3 ; 'question' icon
		)
	)
)

;|*OpenDCL Event Handlers*|;

(defun c:EventHandling/Main#OnInitialize (/)
	(dcl-Control-SetText EventHandling/Main/txtPassword "")
	(princ)
)

(defun c:EventHandling/Main/txtPassword#OnEditChanged (sText /)
	(if (= (strcase sText) "TON")
		(dcl-Form-Close EventHandling/Main 10) ; return 10 to signal a correct answer
	)
	(princ)
)

(defun c:EventHandling/Main/btnGiveUp#OnClicked ()
	(if (EventHandling_PromptClose)
		(dcl-Form-Close EventHandling/Main)
	)
	(princ)
)

(defun c:EventHandling/Main#OnCancelClose (Canceling /)
	;; if this event handler returns T, the dialog will not close
	(not (EventHandling_PromptClose))
)

(defun c:EventHandling/Congrats/btnExit#OnClicked ()
	(dcl-Form-Close EventHandling/Congrats)
	(princ)
)

(princ)

;|*OpenDCL Samples Epilog*|;

;;;######################################################################
;;;######################################################################
;;; The following section of code is designed to locate OpenDCL Studio
;;; sample files in the samples folder by prefixing the filename with
;;; the path prefix that was saved in the registry by the installer.
;;; The global *ODCL:Prefix and function *ODCL:Samples-FindFile
;;; are used throughout the samples.
;;;
(or *ODCL:Samples-FindFile
	(defun *ODCL:Samples-FindFile (file)
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

;; If AllSamples is active, run the main function immediately; otherwise
;; display a banner. The extra gymnastics allow the sample name to be
;; specified in only one place, thus making it easier to reuse this code.
(	(lambda (demoname)
		(if *ODCL:AllSamples
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
