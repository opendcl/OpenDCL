;;;
;;; OpenDCL Sample: Modeless
;;;
;;; This sample demonstrates the modeless form.
;;;

;; Main program
(defun c:Modeless (/ cmdecho)

  ;; Ensure OpenDCL Runtime is (quietly) loaded
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Load the project
  (dcl-Project-Load (*ODCL:Samples-FindFile "Modeless.odcl"))

  ;; Show the main form
  (dcl-Form-Show Modeless/DemoModeless)

  ;; This is a modeless form, so (dcl-Form-Show) returns immediately,
  ;; leaving the event handlers to manage the form.

  (princ)
)

;|«OpenDCL Event Handlers»|;

(defun c:DemoModeless_cmdDrawLine_OnClicked ( / strOldText ptStart ptEnd)
  (setq strOldText (dcl-Control-GetCaption Modeless/DemoModeless/Label1))
  (dcl-Control-SetCaption Modeless/DemoModeless/Label1 "Pick two points in the drawing. The form will stay open but it will be disabled!")
  (dcl-Form-Enable Modeless/DemoModeless nil)
  (if (and (setq ptStart (vl-catch-all-apply 'getpoint (list "\nStart point: ")))
           (not (vl-catch-all-error-p ptStart))
           (setq ptEnd (vl-catch-all-apply 'getpoint (list ptStart "\nDestination point: ")))
           (not (vl-catch-all-error-p ptEnd)))
    (progn
      (command "_LINE" ptStart ptEnd "")
      (command "_ZOOM" "_W" ptStart ptEnd)
    ); progn
  ); if
  (dcl-Form-Enable Modeless/DemoModeless T)
  (dcl-Control-SetCaption Modeless/DemoModeless/Label1 strOldText)
  (princ)
)

(defun c:DemoModeless_cmdZoomWin_OnClicked ()
  (command "_ZOOM" "_W" PAUSE PAUSE)
  (princ)
)

(defun c:DemoModeless_cmdZoomExt_OnClicked ()
  (command "_ZOOM" "_E")
  (princ)
)

(defun c:DemoModeless_CloseButton_Clicked ()
  (dcl-Form-Close Modeless/DemoModeless)
  (princ)
)

(princ)

;|«OpenDCL Samples Epilog»|;

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
  "Modeless"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
