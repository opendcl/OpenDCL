(defun c:DistSample1

    ;;==========================================================================
    ;;
    ;;  DistSample1.lsp
    ;;
    ;;  Quick sample to demonstrate how to use an embedded OpenDCL file.
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  History:
    ;;
    ;;  1.0 2007/06/xx - Original code - Michael Puckett (MP).
    ;;
    ;;  1.1 2007/06/xx - ODCL_* functions renamed to DCL_* to reflect
    ;;                   changes in OpenDCL 4.0 beta 18+. Owen Wengerd (OW).
    ;;
    ;;  1.2 2007/06/24 - Renamed _Load_ODCL_Runtimes to _Load_ODCL_Runtime.
    ;;                   Modified _Load_ODCL_Runtime to use demand loading
    ;;                   path etc. (MP)
    ;;
    ;;  1.3 2009/10/18 - Replaced runtime load logic with OPENDCL command
    ;;                   for Bricscad compatibility; added function to search
    ;;                   for dependent files in OpenDCL Studio samples folder;
    ;;                   renamed some functions for consistency with other
    ;;                   OpenDCl Studio samples. (OW)
    ;;
    ;;  1.4 2014/02/10 - Simplified code by removing some error condition
    ;;                   checks and added OpenDCL sample boilerplate. (OW)
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Dependencies:
    ;;
    ;;  1.  AutoCAD 2004+, OpenDCL 5.0 or newer.
    ;;
    ;;  2.  DistSample.odcl | .lsp. With a form varnamed DistSample/MainForm
    ;;      with a child button varnamed DistSample/MainForm/OkButton
    ;;      with the event OnClick enabled. The project is not passworded.
    ;;
    ;;  3.  If run as a .lsp file expects to find DistSample.odcl in the
    ;;      support path or in the OpenDCL Studio samples folder.
    ;;
    ;;  4.  If run as a .vlx expects to find DistSample.odcl.lsp as described
    ;;      above embedded as a text file resource in the the vlx.
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Notable:
    ;;
    ;;  1.  See function _Load_ODCL_Embedded_Project which attempts to load
    ;;      the OpenDCL project data from the vlx text resources.
    ;;
    ;;==========================================================================

    (	/

        ;;  Locals.

        c:DistSample/MainForm/OkButton#OnClicked
        _Load_ODCL_Embedded_Project
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Embedded_Project ( projname password alias / bytes rtype )

        ;;  Attempt to retrieve an embedded ODCL project (lisp) from
        ;;  the text resources in the current vlx file. If successful
        ;;  attempt to import via odcl_project_import, returning it's
        ;;  result to the caller, otherwise nil.
        (cond
            ;;  Trap unsuccesful retrieval of project from the vlx
            ;;  text resources; alert and bail.
            (	  (or
                    (null (setq bytes (vl-get-resource projname)))
                    (not (eq 'str (setq rtype (type bytes))))
                    (eq "" bytes)
                )

                (princ (strcat "Failed to load <" projname "> ODCL resource from vlx file.\n"))
                nil
            )

            ;;  Call dcl-project-import and return the result to the
            ;;  caller if successful ...
            (	(dcl-project-import bytes password alias)  )
        )
    ) ;;------------------------------------------------------------------------

    (defun c:DistSample/MainForm/OkButton#OnClicked ( )
        (dcl-form-close DistSample/MainForm)
    ) ;;------------------------------------------------------------------------

    (defun _Main ( / odclProjName )
        ;;  Wrap up and call functionality defined in this file.

        ;; Ensure OpenDCL Runtime is (quietly) loaded
        (setq cmdecho (getvar "CMDECHO"))
        (setvar "CMDECHO" 0)
        (command "_OPENDCL")
        (setvar "CMDECHO" cmdecho)

        (setq odclProjName "DistSample.odcl") ;; don't include .lsp extension
        (if
            ;;  Attempt to load the ODCL project.
            (or
                ;;  ODCL project retrieved from vlx resources
                (_Load_ODCL_Embedded_Project odclProjName nil nil)

                ;;  If it couldn't be loaded from resources, try
                ;;  loading from a separate file (this could be
                ;;  useful during development, but you may wish
                ;;  to remove this option before shipping).
                (dcl-Project-Load (*ODCL:Samples-FindFile odclProjName))
            )

            (dcl-Form-Show DistSample/MainForm)
        )

        (princ)
    ) ;;------------------------------------------------------------------------

    (_Main) ;; Invoke _Main ...
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
( (lambda (demoname)
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
  "DistSample1"
)
(princ)
