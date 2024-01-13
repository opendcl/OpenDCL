;;;
;;; Selections Sample
;;;
;; This sample demonstrates how to handle a modal dialog box that closes to
;; allow the user to select objects or pick points, then reopens and updates
;; its state based on the user's actions. If the users presses the [Esc] key
;; while interacting with AutoCAD, the dialog box will not reopen.

;; Main program
(DEFUN c:Sel (/

              ;; local variables
              lstPoints lstObjects doContinue intResult

              ;; local Sub Functions
              point_selection object_selection

              ;; local Sub Event handlers for Modal Form.
              ;; Commands and functions, which show modal form, won't continue after the line
              ;; (dcl-Form-show ...) until the form will be closed. That's why the event
              ;; functions are only needed in this command. Therefore the event function
              ;; can be defined as local sub functions.

              c:Selections/Form#OnInitialize
              c:Selections/Form/Close#OnClicked
              c:Selections/Form/PickPointButton#OnClicked
              c:Selections/Form/PickObjectButton#OnClicked
              c:Selections/Form/PointListBox#OnDblClicked
              c:Selections/Form/ObjectListBox#OnDblClicked
              )

    

    ;|<<Additional sub functions>>|;


    ;; point selection

    (defun point_selection (/ intBlip ptPoint strPoint doSel)

        ;; activate BLIPMODE
        (setq intBlip (getvar "BLIPMODE"))
        (setvar "BLIPMODE" 1)

        (setq doSel T)
        (while doSel

            ;; secure point picking
            (setq ptPoint (vl-catch-all-apply 'getpoint (list "\nPick a point (or press ENTER to return to the form): ")))

            (cond
                ;; ENTER was pressed, set doContinue to T,
                ;; to re-show the form
                ((not ptPoint)
                 (setq ptPoint nil
                       doSel nil
                       doContinue T))

                ;; ESC was pressed, set doContinue to nil,
                ;; to cancel the loop and cancel the command
                ((vl-catch-all-error-p ptPoint)
                 (setq ptPoint nil
                       doSel nil
                       doContinue nil))

                ;; Convert the point to string
                ((not (setq strPoint (vl-prin1-to-string ptPoint)))
                 (setq ptPoint nil
                       doSel T))

                ;; Add a point to the list if it doesnt already exist
                ;; and return to the form after that
                ((not (member strPoint lstPoints))
                 (setq lstPoints (reverse (cons strPoint (reverse lstPoints)))
                       doSel T))
            ); cond
        ); while
        
        ;; restore BLIPMODE
        (setvar "BLIPMODE" intBlip)
    ); point_selection


    ;; object selection

    (defun object_selection (/ intBlip ssAusw intLen entObj vlaObj strObj)

        ;; activate BLIPMODE
        (setq intBlip (getvar "BLIPMODE"))
        (setvar "BLIPMODE" 1)

        ;; secure object picking
        (princ "\nSelect objects (or press ENTER to return to the form): ")
        (setq ssAusw (vl-catch-all-apply 'ssget nil))
        
        (cond
            ;; ENTER was pressed, set doContinue to T,
            ;; to re-show the form
            ((not ssAusw)
             (setq ssAusw nil
                   doContinue T))

            ;; ESC was pressed, set doContinue to nil,
            ;; to cancel the loop and cancel the command
            ((vl-catch-all-error-p ssAusw)
             (setq ssAusw nil
                   doContinue nil))

            ;; check if selection set has objects
            ((zerop (setq intLen (sslength ssAusw)))
             (setq ssAusw nil
                   doContinue T))

            ;; Add objects to list, if they're still not in
            ;; and return to the form after that
            (T (repeat intLen
                   (setq entObj (ssname ssAusw (setq intLen (1- intLen))))
                   (setq vlaObj (vlax-ename->vla-object entObj))
                   (setq strObj (strcat (vla-get-ObjectName vlaObj) " (" (vla-get-Handle vlaObj) ")"))
                   (if (not (member strObj lstObjects))
                       (setq lstObjects (reverse (cons strObj (reverse lstObjects))))
                   ); if
               ); repeat
             (setq doContinue T))
        ); cond
        
        ;; restore BLIPMODE
        (setvar "BLIPMODE" intBlip)
    ); object_selection

    

    ;|*OpenDCL Event Handlers*|;



    ;; The event OnInitialize will be called each time
    ;; the form is going to be shown
    
    (defun c:Selections/Form#OnInitialize (/)
        
        ;; clean if necessary
        (dcl-ListBox-Clear Selections/Form/PointListBox)
        (dcl-ListBox-Clear Selections/Form/ObjectListBox)

        ;; fill pointlist
        (if lstPoints
            (dcl-ListBox-AddList Selections/Form/PointListBox lstPoints)
        ); if

        ;; fill objectlist
        (if lstObjects
            (dcl-ListBox-AddList Selections/Form/ObjectListBox lstObjects)
        ); if
        
    ); c:Selections/Form#OnInitialize

    
    
    ;; This event will be called when the user picks the close-button
    ;; In that case the value 1 is given to close method as an argument.
    ;; This is the return value for the show method (see there).
    
    (defun c:Selections/Form/Close#OnClicked (/)
        (dcl-Form-close Selections/Form 1)
    ); c:Selections/Form/Close#OnClicked

    
    
    ;; This event will be called when the user picks the pointselection-button
    ;; In that case the value 3 is given to close method as an argument.
    ;; This is the return value for the show method (see there).
    
    (defun c:Selections/Form/PickPointButton#OnClicked (/)
        (dcl-Form-close Selections/Form 3)
    ); c:Selections/Form/PickPointButton#OnClicked

    
    
    ;; This event will be called when the user picks the objectselection-button
    ;; In that case the value 14is given to close method as an argument.
    ;; This is the return value for the show method (see there).
    
    (defun c:Selections/Form/PickObjectButton#OnClicked (/)
        (dcl-Form-close Selections/Form 4)
    ); c:Selections/Form/PickObjectButton#OnClicked

    
    
    ;; This event will be called when the user doubleclicks an item
    
    (defun c:Selections/Form/PointListBox#OnDblClicked (/ intRow)
        (if (not (minusp (setq intRow (dcl-ListBox-GetCurSel Selections/Form/PointListBox))))
            (progn
                
                ;; remove an item from the lists
                
                (setq lstPoints (vl-remove (dcl-ListBox-GetItemText Selections/Form/PointListBox intRow) lstPoints))
                (dcl-ListBox-DeleteItem Selections/Form/PointListBox intRow)
            ); progn
        ); if
    ); c:Selections/Form/PointListBox#OnDblClicked

    
    
    ;; This event will be called when the user doubleclicks an item
    
    (defun c:Selections/Form/ObjectListBox#OnDblClicked (/ intRow)
        (if (not (minusp (setq intRow (dcl-ListBox-GetCurSel Selections/Form/ObjectListBox))))
            (progn

                ;; remove an item from the lists
                
                (setq lstObjects (vl-remove (dcl-ListBox-GetItemText Selections/Form/ObjectListBox intRow) lstObjects))
                (dcl-ListBox-DeleteItem Selections/Form/ObjectListBox intRow)
            ); progn
        ); if
    ); c:Selections/Form/ObjectListBox#OnDblClicked

    ;; load COM
    (vl-load-com)

    ;; Ensure OpenDCL Runtime is (quietly) loaded
    (setq cmdecho (getvar "CMDECHO"))
    (setvar "CMDECHO" 0)
    (command "_OPENDCL")
    (setvar "CMDECHO" cmdecho)

    ;; Load the project
    (dcl-Project-Load (*ODCL:Samples-FindFile "Selections.odcl"))

    ;; The repeating calls to show the modal form can be easily achieved
    ;; by a while loop
    (setq doContinue T)
    (while doContinue
        ;; However, to avoid an endless loop, the condition for repeating the loop, must negate at first
        ;; The condition will be "activated" again after dcl-Form-show for some cases.
        (setq doContinue nil)

        ;; if the dialog get closed, the function returns a value
        ;; This is 1 for OK (reserved value), 2 for ESC or Cancel (reserved value)
        ;; or the value, which was given to dcl-Form-close
        (setq intResult (dcl-Form-show Selections/Form))

        ;; This is a modal form, so (dcl-Form-Show) does not return until
        ;; the modal form is closed. In the meantime, the event handlers
        ;; manage the form.

        ;; Now the return value can be interpreted
        (cond
            
            ;; close-button
            ;; Here something can be done with the selected points ans objects
            ((= intResult 1) (setq doContinue nil))

            ;; ESC key
            ((= intResult 2) (setq doContinue nil))

            ;; point selection
            ((= intResult 3) (point_selection))

            ;; objectselection
            ((= intResult 4) (object_selection))

        ); cond
    ); while

    (redraw)
    (princ)
); c:Sel

(princ)

;|*OpenDCL Samples Epilog*|;

;;;######################################################################
;;;######################################################################
;;; The following section of code is designed to locate OpenDCL Studio
;;; sample files in the samples folder by prefixing the filename with
;;; the path prefix that was saved in the registry by the installer.
;;; The global *ODCL:Prefix and function *ODCL:Samples-FindFile
;;; are used throughout the samples.
;;;
(or *ODCL:Samples-FindFile
  (defun *ODCL:Samples-FindFile (file)
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

;; If AllSamples is active, run the main function immediately; otherwise
;; display a banner. The extra gymnastics allow the sample name to be
;; specified in only one place, thus making it easier to reuse this code.
(	(lambda (demoname)
    (if *ODCL:AllSamples
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
