;;;
;;; Message Box Sample
;;;
;;; This sample demonstrates the (dcl_MessageBox) function.
;;;

;; Main program
(defun c:MsgBox (/ cmdecho)

    ;; Ensure OpenDCL Runtime is (quietly) loaded
    (setq cmdecho (getvar "CMDECHO"))
    (setvar "CMDECHO" 0)
    (command "_OPENDCL")
    (setvar "CMDECHO" cmdecho)

    ;; Load the project
    (dcl_Project_Load (*ODCL:Samples:FindFile "MessageBox.odcl"))

    ;; Show the main form
    (dcl_Form_Show MessageBox_MessageBoxes)

    ;; This is a modal form, so (dcl_Form_Show) does not return until
    ;; the modal form is closed. In the meantime, the event handlers
    ;; manage the form.

    (princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:MessageBox_MessageBoxes_ShowMessageBox_OnClicked
       (/ OptIconCSel OptButtonCSel msg title ButtonFlag IconFlag HelpFlag)
    (Setq OptIconCSel
             (dcl_OptionList_GetCurSel MessageBox_MessageBoxes_optIcons)
    )
    (Setq OptButtonCSel (dcl_OptionList_GetCurSel
                            MessageBox_MessageBoxes_optButtons
                        )
    )
    (setq HelpFlag
             (= 1 (dcl_Control_GetValue MessageBox_MessageBoxes_HelpButton))
    )
    (Setq msg (dcl_Control_GetText MessageBox_MessageBoxes_MsgText))
    (Setq Title (dcl_Control_GetText MessageBox_MessageBoxes_MsgTitle))
    (dcl_MessageBox msg title (1+ OptButtonCSel) OptIconCSel HelpFlag)
)

(defun c:MessageBox_MessageBoxes_cmdClose_OnClicked ()
    (dcl_Form_Close MessageBox_MessageBoxes)
)

(defun c:MessageBox_MessageBoxes_OnHelp (/)
    (dcl_MessageBox
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
    "MsgBox"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
