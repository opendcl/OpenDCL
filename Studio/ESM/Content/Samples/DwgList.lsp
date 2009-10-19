;;;
;;; DWG List Sample
;;;
;;; This sample demonstrates the DWG List control and all its events.
;;;

;; Main program
(defun c:DwgList (/ cmdecho)

    ;; Ensure OpenDCL Runtime is (quietly) loaded
    (setq cmdecho (getvar "CMDECHO"))
    (setvar "CMDECHO" 0)
    (command "_OPENDCL")
    (setvar "CMDECHO" cmdecho)

    ;; Load the project
    (dcl_Project_Load (*ODCL:Samples:FindFile "DwgList.odcl"))

    ;; Show the main form
    (dcl_Form_Show DwgList_DwgListForm)

    ;; This is a modeless form, so (dcl_Form_Show) returns immediately,
    ;; leaving the event handlers to manage the form.

    (princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:DwgListForm_DwgList1_OnDblClicked ()
    (Setq fileName (dcl_DwgList_GetFileName DwgList_DwgListForm_DwgList1))
    (if (equal (strcase (substr fileName (- (strlen fileName) 3) 4)) ".DWG")
        (progn                                    ;(dcl_SetCmdBarFocus)
            (dcl_sendstring (strcat "_-INSERT " fileName "\n"))
        )
    )
    (princ)
)

(defun c:DwgList_DwgListForm_cmdUP_OnClicked (/)
    (setq sPath (strcat (dcl_ComboBox_GetDir DwgList_DwgListForm_ComboBox1)
                        "\\.."
                )
    )
    (dcl_ComboBox_Dir DwgList_DwgListForm_ComboBox1 sPath)
    (dcl_DwgList_Dir DwgList_DwgListForm_DwgList1 sPath)
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
    "DwgList"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
