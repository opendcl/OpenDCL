;;;
;;; ToolTip Sample
;;;
;;; This sample demonstrates tool tips.
;;;

;; Main program
(defun c:ToolTip (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl-Project-Load (*ODCL:Samples:FindFile "ToolTip.odcl"))

	;; Show the main form
	(dcl-Form-Show ToolTip/dclToolTip)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:ToolTip/dclToolTip#OnInitialize ( /)
	(dcl-ComboBox-SetCurSel ToolTip/dclToolTip/cboColor 1)
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "<None>" -1 -1)
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Question" 1 1)		;; predefined icon, not from project's image list
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Information" 0 0)	;; predefined icon, not from project's image list
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Exclamation" 2 2)	;; predefined icon, not from project's image list
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Error" 3 3)		;; predefined icon, not from project's image list
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Picture" 4 4)		;; icons from project's image list
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Folder" 5 5)		;; icons from project's image list
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Camera" 6 6)		;; icons from project's image list
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Sound" 7 7)		;; icons from project's image list
	(dcl-ImageComboBox-AddString ToolTip/dclToolTip/cboIcon "Video" 8 8)		;; icons from project's image list
	(dcl-ImageComboBox-SetCurSel ToolTip/dclToolTip/cboIcon 0)
	(dcl-Control-SetText ToolTip/dclToolTip/txtMain "This is a <b>Tooltip</b>.\r\nThis is the second line.")
)


(defun c:ToolTip/dclToolTip/cmdOK#OnClicked ( / AviPath)

	(dcl-Control-SetToolTipTitle ToolTip/dclToolTip/cmdOK (dcl-Control-GetText ToolTip/dclToolTip/txtTitle))
	(dcl-Control-SetToolTipMainText ToolTip/dclToolTip/cmdOK (dcl-Control-GetText ToolTip/dclToolTip/txtMain))
	(dcl-Control-SetToolTipLine ToolTip/dclToolTip/cmdOK (= 1 (dcl-Control-GetValue ToolTip/dclToolTip/chkLine)))
	(dcl-Control-SetToolTipTitleColor ToolTip/dclToolTip/cmdOK (dcl-ComboBox-GetItemData ToolTip/dclToolTip/cboColor (dcl-ComboBox-GetCurSel ToolTip/dclToolTip/cboColor)))
	(if (< (abs (setq intPos (dcl-ImageComboBox-GetCurSel ToolTip/dclToolTip/cboIcon))) 5)
            (dcl-Control-SetToolTipPicture ToolTip/dclToolTip/cmdOK (- intPos))    ;; predefined icons (-1 -2 -3 -4)
            (dcl-Control-SetToolTipPicture ToolTip/dclToolTip/cmdOK (+ 96 intPos)) ;; icons from project's image list
        ); if
	(dcl-Control-SetToolTipBalloon ToolTip/dclToolTip/cmdOK (= 1 (dcl-Control-GetValue ToolTip/dclToolTip/chkBalloon)))
	(dcl-Control-ShowToolTip ToolTip/dclToolTip/cmdOK)
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
	"ToolTip"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
