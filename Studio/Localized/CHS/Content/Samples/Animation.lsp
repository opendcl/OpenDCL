;;;
;;; Animation Sample
;;;
;;; This sample demonstrates the Animation control and its events.
;;;

;; Main program
(defun c:Animation (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl-Project-Load (*ODCL:Samples-FindFile "Animation.odcl"))

	;; Show the main form
	(dcl-Form-Show Animation/Form1)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|*OpenDCL Event Handlers*|;

(defun c:Animation/Form1#OnInitialize (/ fn avi)
	(dcl-Animation-Load Animation/Form1/Animation1 (*ODCL:Samples-FindFile "Clock.avi"))
	(princ)
)

(defun c:Animation/Form1/cmdRestart#OnClicked (/)
	(dcl-Animation-Play Animation/Form1/Animation1)
	(princ)
)

(defun c:Animation/Form1/cmdStop#OnClicked (/)
	(dcl-Animation-Stop Animation/Form1/Animation1)
	(princ)
)

(defun c:Animation/Form1/cboFrame#OnSelChanged (nSelection sSelText /)
	(if (zerop (dcl-OptionList-GetCurSel Animation/Form1/optSeekType))
		(dcl-Animation-Seek Animation/Form1/Animation1 nSelection)
		(dcl-Animation-Play Animation/Form1/Animation1 nSelection)
	)
	(princ)
)

(defun c:Animation/Form1/cmdClose#OnClicked (/)
	(dcl-Form-Close Animation/Form1)
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
	"Animation"
)
(princ)

;;;######################################################################
;;;######################################################################
