;;;
;;; Splitter Sample
;;;
;;; This sample demonstrates the splitter control.
;;;

;; Main program
(defun c:Splitter (/ cmdecho
                     set_style
                     set_thickness
                     c:splitter_splitter_pb_1_OnClicked
                     c:splitter_splitter_pb_2_OnClicked c:splitter_splitter_pb_3_OnClicked
                     c:splitter_splitter_pb_4_OnClicked c:splitter_splitter_pb_5_OnClicked
                     c:splitter_splitter_pb_6_OnClicked c:splitter_splitter_pb_8_OnClicked
                     c:splitter_splitter_pb_10_OnClicked c:splitter_splitter_pb_12_OnClicked
                     c:splitter_splitter_pb_raised_OnClicked c:splitter_splitter_pb_doubled_OnClicked
                     c:splitter_splitter_pb_etched_OnClicked c:splitter_splitter_pb_flat_OnClicked
                     c:splitter_splitter_pb_sunken_OnClicked c:splitter_splitter_pb_bump_OnClicked
                  )

    (defun set_style (n)
        (dcl_Control_SetEdgeStyle splitter_splitter_spl_hor n)
        (dcl_Control_SetEdgeStyle splitter_splitter_spl_ver n)
    ); set_style

    (defun set_thickness (n)
        (dcl_Control_SetHeight splitter_splitter_spl_hor n)
        (dcl_Control_SetWidth splitter_splitter_spl_ver n)
    ); set_thickness

    ;|<<OpenDCL Event Handlers>>|;

    (defun c:splitter_splitter_pb_1_OnClicked ( /) (set_thickness 1))
    (defun c:splitter_splitter_pb_2_OnClicked ( /) (set_thickness 2))
    (defun c:splitter_splitter_pb_3_OnClicked ( /) (set_thickness 3))
    (defun c:splitter_splitter_pb_4_OnClicked ( /) (set_thickness 4))
    (defun c:splitter_splitter_pb_5_OnClicked ( /) (set_thickness 5))
    (defun c:splitter_splitter_pb_6_OnClicked ( /) (set_thickness 6))
    (defun c:splitter_splitter_pb_8_OnClicked ( /) (set_thickness 8))
    (defun c:splitter_splitter_pb_10_OnClicked ( /) (set_thickness 10))
    (defun c:splitter_splitter_pb_12_OnClicked ( /) (set_thickness 12))

    (defun c:splitter_splitter_pb_raised_OnClicked ( /) (set_style 0))
    (defun c:splitter_splitter_pb_doubled_OnClicked ( /) (set_style 1))
    (defun c:splitter_splitter_pb_etched_OnClicked ( /) (set_style 2))
    (defun c:splitter_splitter_pb_flat_OnClicked ( /) (set_style 3))
    (defun c:splitter_splitter_pb_sunken_OnClicked ( /) (set_style 4))
    (defun c:splitter_splitter_pb_bump_OnClicked ( /) (set_style 5))

    ;|<<Show the form>>|;

    ;; Ensure OpenDCL Runtime is (quietly) loaded
    (setq cmdecho (getvar "CMDECHO"))
    (setvar "CMDECHO" 0)
    (command "_OPENDCL")
    (setvar "CMDECHO" cmdecho)

    ;; Load the project
    (dcl_Project_Load (*ODCL:Samples:FindFile "Splitter.odcl"))

    ;; Show the main form
    (dcl_Form_Show splitter_splitter)

    ;; This is a modal form, so (dcl_Form_Show) does not return until
    ;; the modal form is closed. In the meantime, the event handlers
    ;; manage the form.

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
    "Splitter"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
