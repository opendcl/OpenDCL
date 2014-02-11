(defun c:DistSample2

    ;;==========================================================================
    ;;
    ;;  DistSample2.lsp
    ;;
    ;;  Quick sample to demonstrate how to use an embedded OpenDCL stream.
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
    ;;  2.  DistSample.odcl (OpenDCL project) saved as DistSample.odcl.lsp
    ;;      (Base64 encoded stream wrapped in lisp). Said project has a form
    ;;      varnamed DistSample/MainForm with a child button varnamed
    ;;      DistSample/MainForm/OkButtonOkButton with the event OnClick enabled.
    ;;      The project is not passworded.
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Notable:
    ;;
    ;;  1.  See function _Load_ODCL_Stream which loads the Base64 encoded
    ;;      OpenDCL project data in-line.
    ;;
    ;;==========================================================================

    (	/

        ;;  Locals.

        c:DistSample/MainForm/OkButton#OnClicked
        _Load_ODCL_Stream
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Stream ( password alias / project rtype )

        ;;  This is a reformatted copy of the content from
        ;;  the OpenDCL project file DistSample.odcl.lsp.
        ;;  The only reformatting is a change in the line
        ;;  length so it looks tidy.

        (setq project
            '(  "YWt6A3gaAAAZqQ4nBuKTJjUxbS9qgPiMxuxQZFz3VSx8Fvd0ts1mHXLc96T80cYOOSVUNld6bg9b"
                "93VpkCPHCpdNj+/GCttFAf5lCVj0xAzdLMAF2U2tRzvq3lwfkJqwopHgDZBDoUOZskrzauh1Voik"
                "P/IdaA1G+W8yHGRH8rx8jth41pstVUuJXfNGTJY+ccSv94OSZ/BwDqozvWAaR2TfwvGYBFCwxy8K"
                "L9H9VhVaFwWt/CDPbcOqNy/g2BhBE0wYgwAdo7nvIEUwB6LS1otER5TYKPRow6LT9URf+svJCKV7"
                "G9+8P6I4W9dI8ELwHXRNmtkA3sRWyMpNlPC+jXzBOuv9izPDa4zVfCklAIIztml6KDyBa5RigfU6"
                "qicq3NrXlQ5nwZkf1ERxoauMmWBIhDF7AFiObf/BBYUX/8T3ZHkZShCMYw+5FHdqO3K9ZrovWkbg"
                "n7JSfQ773crv8CxNu8Xwx8A5M+7c5tmLfOEnHMMwVaTeIIWI/fQHgCCaVmIMsRQhJ4L5T/9/EEK5"
                "FfQl1s6Rgh0XYroUeqRfTdljDNNYsJo3TznDShvk9tRsjwJWnAMhnnl8oYU7gJuinqYGPJ43iQ7S"
                "0URH2PqCzL7JugA1kIenBqetSbeON6YBCu29uLO8gM2Z8e/R7KFrLrBqnZu4yCudFCFG1fAfSY0d"
                "4n8u8TorQmgnJVUOoMksRrcOOae5Zshi0VatXvk1VtPYaBMJJdItkbfRFwKkvZFKQF2aYTqIyAaM"
                "dSRAmp9f8zH3FDJ24mqG2r7OgoC0lKddhuoqrWYFJhX5chLtq8pdv+Q2530N1D2qrUGzAZ2AZIeB"
                "G3WYAbfFo8Yhl6zJm1uL7zknA5JCsWaU6SfqA+RfzKHIJ410zaG9B6gwpaHV4GngATWcpSrRqaeV"
                "j++rqn4BS2PMgqsamhUqLV2Cqwp9Ad7kqC9DcygPcIgJTPptd/U3LbdVvPW5LxZKCaN47MM7RYPy"
                "z2Ef8eYL3che/MzINkni0CVXgOMgwJyoFPV/gf6BSJghph6lEoFJpHSEduIQbSQpoRKBSaR8gH+B"
                "/gG+QZ5hjnGGeYJ9gKX1gT6MDg/a4YABc5JBXGgBHIdBgoYhf8MIHXU="
            )
        )

        ;;  Call dcl_project_import with our
        ;;  data, returning the result to the caller.
        (dcl-project-import project password alias)
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
                (_Load_ODCL_Stream nil "DistSample")

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
  "DistSample2"
)
(princ)
