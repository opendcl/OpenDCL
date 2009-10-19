;;;
;;; Options Tab Sample
;;;
;;; This sample demonstrates the Options Tab control.
;;;

;; Main program
(defun c:OptionsTab (/ cmdecho)

    ;; Ensure OpenDCL Runtime is (quietly) loaded
    (setq cmdecho (getvar "CMDECHO"))
    (setvar "CMDECHO" 0)
    (command "_OPENDCL")
    (setvar "CMDECHO" cmdecho)

    ;; Load the project
    (dcl_Project_Load (*ODCL:Samples:FindFile "OptionsTab.odcl"))

    ;; Options tabs are registered when the project loads. All registered
    ;; options tabs will be created automatically when the OPTIONS dialog
    ;; opens.
    (dcl_messagebox "Navigate to the \"OpenDCL Demo\" tab" "OpenDCL Options Tab Demo")
    (command "._OPTIONS") ; open the options dialog

    ;; This is a modeless form, so (dcl_Form_Show) returns immediately,
    ;; leaving the event handlers to manage the form.

    (princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:OptionsTab_OptionsTabDemo_OnOK (/)
    (princ "\n[OpenDCL Options Tab Demo] You entered \"")
    (princ (dcl_Control_GetText OptionsTab_OptionsTabDemo_txtOptTabDemo))
    (princ "\" and left the check box ")
    (if (eq 1 (dcl_Control_GetValue OptionsTab_OptionsTabDemo_chkOptTabDemo))
        (princ "CHECKED")
        (princ "UNCHECKED")
    )
    (princ "\n")
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
    "OptionsTab"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
