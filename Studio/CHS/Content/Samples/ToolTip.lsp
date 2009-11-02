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
	(dcl_Project_Load (*ODCL:Samples:FindFile "ToolTip.odcl"))

	;; Show the main form
	(dcl_Form_Show ToolTip_dclToolTip)

	;; This is a modal form, so (dcl_Form_Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:ToolTip_dclToolTip_OnInitialize ( /)
	(dcl_ComboBox_SetCurSel ToolTip_dclToolTip_cboColor 1)
	(dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "<None>" -1 -1)
	(dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "Question" 1 1)
	(dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "Information" 0 0)
	(dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "Exclamation" 2 2)
	(dcl_ImageComboBox_AddString ToolTip_dclToolTip_cboIcon "Error" 3 3)
	(dcl_ImageComboBox_SetCurSel ToolTip_dclToolTip_cboIcon 0)
)


(defun c:ToolTip_dclToolTip_cmdOK_OnClicked ( / AviPath)

;;;	(if (dcl_Control_GetValue ToolTip_dclToolTip_chkAVI)
;;;		(progn
;;;			(setq AviPath (findfile "FileCopy.avi"))
;;;			(if AviPath
;;;				(dcl_Control_SetToolTipAviFileName  ToolTip_dclToolTip_cmdOK AviPath)
;;;				(alert "Sorry can't find FileCopy.avi, make sure it's in a support path")
;;;			);_ if
;;;		);_ progn
;;;		(dcl_Control_SetToolTipAviFileName ToolTip_dclToolTip_cmdOK nil)
;;;	); _if

	(dcl_Control_SetToolTipTitle ToolTip_dclToolTip_cmdOK (dcl_Control_GetText ToolTip_dclToolTip_txtTitle))
	(dcl_Control_SetToolTipMainText ToolTip_dclToolTip_cmdOK (dcl_Control_GetText ToolTip_dclToolTip_txtMain))
	(dcl_Control_SetToolTipLine ToolTip_dclToolTip_cmdOK (= 1 (dcl_Control_GetValue ToolTip_dclToolTip_chkLine)))
	(dcl_Control_SetToolTipTitleColor ToolTip_dclToolTip_cmdOK (dcl_ComboBox_GetItemData ToolTip_dclToolTip_cboColor (dcl_ComboBox_GetCurSel ToolTip_dclToolTip_cboColor)))
	(dcl_Control_SetToolTipPicture ToolTip_dclToolTip_cmdOK (- (dcl_ImageComboBox_GetCurSel ToolTip_dclToolTip_cboIcon)))
	(dcl_Control_SetToolTipBalloon ToolTip_dclToolTip_cmdOK (= 1 (dcl_Control_GetValue ToolTip_dclToolTip_chkBalloon)))
	(dcl_Control_ShowToolTip ToolTip_dclToolTip_cmdOK)
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
