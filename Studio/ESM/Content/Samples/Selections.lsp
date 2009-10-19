;;;
;;; Selections Sample
;;;
;; This sample demonstrates how to handle a modal dialog box that closes to
;; allow the user to select objects or pick points, then reopens and updates
;; its state based on the user's actions. If the users presses the [Esc] key
;; while interacting with AutoCAD, the dialog box will not reopen.

;; Main program
(DEFUN c:Sel (/                              fn
              bflag                          blipper
              echo                           PtList
              ObjList
              ;; local Sub Functions
              *error*                        ss->objlist
              ;; local Sub Event handlers for Modal Form.
              c:Selections_Form_OnInitialize
              c:Selections_Form_Close_OnClicked
              c:Selections_Form_GraphicButton1_OnClicked
              c:Selections_Form_GraphicButton2_OnClicked
             )

    (SETQ blipper (GETVAR "BLIPMODE")
          echo    (GETVAR "CMDECHO")
    )
    (vl-load-com)
    (SETVAR "CMDECHO" 0)
    ;;------------------------------------------------------------------
    ;;   Standard error trap
    ;;
    ;; This sub function is called by AutoCAD when the user presses
    ;; the [Esc] key during the getpoint prompt
    (DEFUN *error* (msg)
        (COND
            ((NOT msg))                           ; no error, do nothing
            ((VL-POSITION
                 (STRCASE msg T)                  ; cancel
                 '("console break" "function cancelled" "quit / exit abort")
             )
             ;; just for the Demo ...
             (dcl_MESSAGEBOX ".... Quit/Exit/Abort/*Cancelled*"
                              "Inside Error Handler" ; Form Caption
                              2                   ; OK Button Only
                              1                   ; exclamation point icon is displayed in the message box.
             )
            )
            ((PRINC (STRCAT "\nApplication Error: " (itoa (GETVAR "errno")) " :- " msg "\n"))
            )
        )
        (SETVAR "errno" 0)
        ;;
        ;;
        (SETQ bflag nil) ; So the dialog box doesn't reopen
        (SETVAR "BLIPMODE" blipper)
        (VL-CMDF "._REDRAW")
        (SETVAR "CMDECHO" echo)
    )
    ;;------------------------------------------------------------------
    ;;
    (DEFUN ss->objlist (ss / i returnval)
        (IF (AND ss (< 0 (SSLENGTH ss)))
            (PROGN (SETQ i 0)
                   (REPEAT (SSLENGTH ss)
                       (SETQ returnval (CONS (VLAX-ENAME->VLA-OBJECT (SSNAME ss i))
                                             returnval
                                       )
                             i         (1+ i)
                       )
                   )
            )
        )
        (IF returnval
            (REVERSE returnval)
            nil
        )
    )
    ;;------------------------------------------------------------------
    ;;
    (DEFUN c:Selections_Form_Close_OnClicked ()
        (SETQ bflag nil) ; So the dialog box doesn't reopen
        (dcl_FORM_CLOSE Selections_Form)
    )
    ;;------------------------------------------------------------------
    ;;
    ;; this sub function receives the clicked event of the PickPointButton
    ;;
    (DEFUN c:Selections_Form_PickPointButton_OnClicked (/ pt)
        ;;
        ;; Call the Form_Close method to close the dialog while the user picks the point.
        ;; Set bflag to true so the while loop in Main will reshow the dialog box
        ;; once the point has been picked.
        (SETQ bflag T)
        (dcl_FORM_CLOSE Selections_Form)
        (SETVAR "BLIPMODE" 1)
        (SETQ pt     (GETPOINT "\nSelect point: ")
              ptList '()
        )
        (WHILE pt
            (SETQ ptList (CONS (VL-PRINC-TO-STRING pt) ptList))
            (PROMPT (STRCAT "\n Selected another Point : "
                            (VL-PRINC-TO-STRING pt)
                            "\nEnter to Return to Form or ESC to cancel. "
                    )
            )
            (SETQ pt (GETPOINT "\nSelect other point."))
        )
        (SETVAR "BLIPMODE" 0)
        (VL-CMDF "._REDRAW")
    )
    ;;------------------------------------------------------------------
    ;;
    ;; this sub function receives the clicked event of the PickObjectButton
    (DEFUN c:Selections_Form_PickObjectButton_OnClicked (/ ss)
        ;;
        ;; set bflag to true so the while loop in Main will reshow the dialog box
        ;; once the objects have been selected.
        (SETQ bflag T
              ObjList '()
        )
        ;;
        ;; Call the method to close the dialog box
        (dcl_FORM_CLOSE Selections_Form)
        (PROMPT "\n Select Objects, or \n Enter to Return to Form.")
        (SETQ ss (SSGET))
        ;;
        (FOREACH obj (SS->OBJLIST ss)
            (SETQ ObjList (CONS (VLA-GET-OBJECTNAME obj) ObjList))
        )
        (SETQ ObjList (REVERSE ObjList))
    )
    ;;------------------------------------------------------------------
    ;;
    (DEFUN c:Selections_Form_OnInitialize (/)
        (IF PtList
            (dcl_LISTBOX_ADDLIST Selections_Form_PointListBox PtList)
        )
        (IF ObjList
            (dcl_LISTBOX_ADDLIST Selections_Form_ObjectListBox ObjList)
        )
    )
    ;;------------------------------------------------------------------
    ;;
    ;;  Main Entry Point
    ;;------------------------------------------------------------------

    ;; Ensure OpenDCL Runtime is loaded
    (command "_OPENDCL")

    ;; Load the project
    (dcl_Project_Load (*ODCL:Samples:FindFile "Selections.odcl"))

    ;; show the form here, and make any calls required to
    ;; initialize/reinitialize the dialog box controls before dcl_Form_Show
    (SETQ bflag T)
    (WHILE bflag                                  ; To fire it up
        (SETQ bflag nil)
        (dcl_FORM_SHOW Selections_Form)
        ;; The Event handlers manage the form here.
        ;;
    )
    ;;------------------------------------------------------------------
    ;;
    (*error* nil)
    (PRINC)
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
    "Sel"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
