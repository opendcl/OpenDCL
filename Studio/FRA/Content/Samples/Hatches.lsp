;;;
;;; Hatch Sample
;;;
;;; This sample demonstrates the Hatch control and all its events.
;;;

;; Main program
(defun c:Hatches (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl-Project-Load (*ODCL:Samples-FindFile "Hatches.odcl"))

	;; Show the main form
	(dcl-Form-Show Hatches/Hatch)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|«OpenDCL Event Handlers»|;

(defun c:Hatches/Hatch#OnInitialize (/ scale idx)
	(setq scale (rtos (dcl-Control-GetHatchScale Hatches/Hatch/Hatch1) 2 0))
	(setq idx (dcl-ComboBox-FindString Hatches/Hatch/ComboBox2 scale))
	(if (< idx 0) (setq idx 0))
	(dcl-ComboBox-SetCurSel Hatches/Hatch/ComboBox2 idx)
)

(defun c:Hatches/Hatch/ComboBox1#OnSelChanged (nSelection sSelText /)
	(dcl-Hatch-setpattern Hatches/Hatch/Hatch1 sSelText)
	(dcl-Hatch-setpattern Hatches/Hatch/Hatch2 sSelText)
)

(defun c:Hatches/Hatch/ComboBox2#OnSelChanged (nSelection sSelText / hatpat)
	;; set the selected scale for each hatch display tile
	(dcl-Control-SetHatchScale Hatches/Hatch/Hatch1 (atof sSelText))
	(dcl-Control-SetHatchScale Hatches/Hatch/Hatch2 (atof sSelText))
	;; get the current hatch pattern name from the ComboBox
	(setq hatpat (dcl-ComboBox-GetLBText
					 Hatches/Hatch/ComboBox1
					 (dcl-ComboBox-GetCurSel Hatches/Hatch/ComboBox1)
				 )
	) 
	;; Use the previously defined function to re-draw the patterns
	(c:Hatches/Hatch/ComboBox1#OnSelChanged nil hatpat)
)

(princ)

;|«OpenDCL Samples Epilog»|;

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
	"Hatches"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
