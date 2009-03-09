(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"SEL\" ein, um das Beispiel zu starten.\n")
)

;; In diesem Beispiel wird der Aufruf einer modalen Dialogbox demonstriert,
;; die geschlossen wird, damit der Anwender Punkte oder Objekte wählen kann,
;; um hernach wieder geöffnet zu werden und den Inhalt der Listen zu aktualisieren
;; Drückt der Anwender während der Punkt- bzw. Objektwahl die ESC-Taste, wird der
;; Dialog anschließend nicht wieder geöffnet, sondern das Programm abgebrochen.
;;
;; Zum Testen können Sie den Dialog wie folgt zurücksetzen
;; (dcl_Project_Load "Selections" T "Selections")


(DEFUN c:sel (/                              fn
              bflag                          blipper
              echo                           PtList
              ObjList
              ;; lokale Unterfunktionen
              *error*                        ss->objlist
              ;; Lokale Ereignisfunktionen
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
    ;;   Standard-Fehler-Funktion
    ;;
    ;; Diese Funktion wird ausgeführt, wenn der Anwender im Programmverlauf die ESC-Taste drückt.
    (DEFUN *error* (msg)
        (COND
            ((NOT msg))                           ; kein Fehler, macht nichts
            ((VL-POSITION
                 (STRCASE msg T)                  ; Abbruch
                 '("console break" "function cancelled" "quit / exit abort")
             )
             ;; zu Demonstrationszwecken ...
             (dcl_MESSAGEBOX ".... Funktionsabbruch"
                              "Innerhalb der Ereignisfunktion" ; Titelzeile
                              2                   ; Nur eine OK-Taste
                              1                   ; Ausrufungszeichen im Dialog
             )
            )
            ((PRINC (STRCAT "\nProgrammfehler: " (itoa (GETVAR "errno")) " :- " msg "\n"))
            )
        )
        (SETVAR "errno" 0)
        ;;
        ;;        
        (SETQ bflag nil) ; Dialog soll nicht mehr geöffnet werden
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
        (SETQ bflag nil) ; Auf diesem Weg wird der Dialog nicht mehr geöffnet, wenn der Anwender rechts oben das Schließen-Symbol oder die ESC-Taste drückt
        (dcl_FORM_CLOSE Selections_Form)
    )
    ;;------------------------------------------------------------------
    ;;   
    ;; Wird ausgelöst, wenn die Schaltfläche PickPointButton geklickt wird
    ;;
    (DEFUN c:Selections_Form_PickPointButton_OnClicked (/ pt)
        ;;
        ;; Ruft die Methode Form_Close auf, um den Dialog zu schließen, wenn der Anwender einen Punkt klickt
        ;; bflag T setzen, damit die Schleife wieder durchlaufen und der Dialog wieder angezeigt wird
        ;; wenn der Punkt gepickt wurde.
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
    ;; Wird ausgelöst, wenn die Schaltfläche PickObjectButton geklickt wird
    (DEFUN c:Selections_Form_PickObjectButton_OnClicked (/ ss)
        ;;
        ;; bflag T setzen, damit die Schleife wieder durchlaufen und der Dialog wieder angezeigt wird
        ;; wenn das Objekt gewählt wurde.
        (SETQ bflag T
              ObjList '()
        )
        ;;
        ;; Ruft die Methode Form_Close auf, um den Dialog zu schließen, wenn der Anwender ein Objekt wählt
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
    ;;  Haupteinstiegspunkt
    ;;------------------------------------------------------------------

    (LoadODCLProj "Selections.odcl")

    ;; Dialog anzeigen und alle nötigen Einstellungen zur Initialiiserung
    ;; setzen, bevor dcl_Form_Show aufgerufen wird
    
    (SETQ bflag T)
    (WHILE bflag                                  ; Beginn der Schleife
        (SETQ bflag nil)			  ; Auf diesem Weg wird der Dialog nicht mehr geöffnet, wenn der Anwender rechts oben das Schließen-Symbol oder die ESC-Taste drückt

        ;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
        ;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog.
        (dcl_FORM_SHOW Selections_Form)
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
