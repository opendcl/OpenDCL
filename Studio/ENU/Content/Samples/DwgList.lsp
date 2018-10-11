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
  (dcl-Project-Load (*ODCL:Samples-FindFile "DwgList.odcl"))

  ;; Show the main form
  (dcl-Form-Show DwgList/DwgListForm)

  ;; This is a modeless form, so (dcl-Form-Show) returns immediately,
  ;; leaving the event handlers to manage the form.

  (princ)
)

;|*OpenDCL Event Handlers*|;

(defun c:DwgList/DwgListForm#OnInitialize (/)
  (dcl-ComboBox-AddPath DwgList/DwgListForm/ComboBox1
    (dcl-DWGList-GetDir DwgList/DwgListForm/DwgList1)
  )
  (princ)
)

(defun c:DwgList/DwgListForm/DwgList1#OnDblClicked (/ filename)
  (if (and (Setq filename (dcl-DWGList-GetFileName DwgList/DwgListForm/DwgList1))
                 (equal (strcase (substr filename (- (strlen filename) 3) 4)) ".DWG"))
    (dcl-sendstring (strcat "_-INSERT " filename "\n"))
  )
  (princ)
)

(defun c:DwgList/DwgListForm/cmdUP#OnClicked (/ path)
  (setq path (dcl-DWGList-GetDir DwgList/DwgListForm/DwgList1))
  (cond
    ( (= path ""))
    ( (wcmatch path "::*"))
    ( (dcl-DWGList-Dir DwgList/DwgListForm/DwgList1 (strcat path "\\.."))
      (dcl-ComboBox-AddPath DwgList/DwgListForm/ComboBox1
        (dcl-DWGList-GetDir DwgList/DwgListForm/DwgList1)
      )
    )
  )
  (princ)
)

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
  "DwgList"
)
(princ)

;;;######################################################################
;;;######################################################################
