(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"SEL\" ein, um das Beispiel zu starten.\n")
)

;; This function is used to demonstrate how to create a modal dialog box
;; that closes to allow the user to select objects or pick points, then reopens
;; and updates its state based on the user's actions.
;; If the users presses the [Esc] key while interacting with AutoCAD, the
;; dialog box will not reopen.
;;
;; For Testing, reset the <Project_Form> VarName
;; (dcl_Project_Load "Selections" T "Selections")
;;
(DEFUN c:sel (/                              fn
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

    (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
    (LoadRunTime)
    
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
             (dcl_MESSAGEBOX ".... Funktionsabbruch"
                              "Innerhalb der Ereignisfunktion" ; Form Caption
                              2                   ; OK Button Only
                              1                   ; exclamation point icon is displayed in the message box.
             )
            )
            ((PRINC (STRCAT "\nProgrammfehler: " (itoa (GETVAR "errno")) " :- " msg "\n"))
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
        (SETQ pt     (GETPOINT "\nPunkt wählen: ")
              ptList '()
        )
        (WHILE pt
            (SETQ ptList (CONS (VL-PRINC-TO-STRING pt) ptList))
            (PROMPT (STRCAT "\nEinen anderen Punkt wählen: "
                            (VL-PRINC-TO-STRING pt)
                            "\nDrücken Sie die Eingabetexte, um zum Dialog zurückzukrehren, oder ESC um abzubrechen. "
                    )
            )
            (SETQ pt (GETPOINT "\nEinen anderen Punkt wählen: "))
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
        (PROMPT "\nObjekte wählen oder Eingabetaste drücken, um zum Dialog zurückzukehren")
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

    (LoadODCLProj "Selections.odcl")
    
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
(PRINC)
 ;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 2 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
