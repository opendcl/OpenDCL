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
    (dcl_Project_Load (*ODCL:Samples:FindFile "Hatches.odcl"))

    ;; Show the main form
    (dcl_Form_Show Hatches_Hatch)

    ;; This is a modal form, so (dcl_Form_Show) does not return until
    ;; the modal form is closed. In the meantime, the event handlers
    ;; manage the form.

    (princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:Hatches_Hatch_OnInitialize (/ scale idx)
    (setq scale (rtos (dcl_Control_GetHatchScale Hatches_Hatch_Hatch1) 2 0))
    (setq idx (dcl_ComboBox_FindString Hatches_Hatch_ComboBox2 scale))
    (if (< idx 0) (setq idx 0))
    (dcl_ComboBox_SetCurSel Hatches_Hatch_ComboBox2 idx)
)

(defun c:Hatches_Hatch_ComboBox1_OnSelChanged (nSelection sSelText /)
    (dcl_hatch_setpattern Hatches_Hatch_Hatch1 sSelText)
    (dcl_hatch_setpattern Hatches_Hatch_Hatch2 sSelText)
)

(defun c:Hatches_Hatch_ComboBox2_OnSelChanged (nSelection sSelText / hatpat)
    ;; set the selected scale for each hatch display tile
    (dcl_Control_SetHatchScale Hatches_Hatch_Hatch1 (atof sSelText))
    (dcl_Control_SetHatchScale Hatches_Hatch_Hatch2 (atof sSelText))
    ;; get the current hatch pattern name from the ComboBox
    (setq hatpat (dcl_ComboBox_GetLBText
                     Hatches_Hatch_ComboBox1
                     (dcl_ComboBox_GetCurSel Hatches_Hatch_ComboBox1)
                 )
    ) 
    ;; Use the previously defined function to re-draw the patterns
    (c:Hatches_Hatch_ComboBox1_OnSelChanged nil hatpat)
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
                 (   *ODCL:Prefix
                 ) ;_ already defined
                 (   (vl-registry-read
                         "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 32-bit location
                 (   (vl-registry-read
                         "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 32-bit location
                 (   (vl-registry-read
                         "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 64-bit location
                 (   (vl-registry-read
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
(   (lambda (demoname)
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
    "Hatches"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
