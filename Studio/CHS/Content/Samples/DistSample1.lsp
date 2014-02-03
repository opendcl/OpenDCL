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
        _Load_ODCL_Runtime
        _Load_ODCL_Embedded_Project
        _Load_ODCL_File_Project
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Runtime ( / )

        (or

            ;;  Already loaded, vacate now (return t to caller).

            dcl-getversionex

            ;;  If demand loading is enabled, use the OPENDCL command
            ;;  to induce the loading of the OpenDCL Runtime. If demand
            ;;  loading is disabled, assume it was disabled intentionally
            ;;  and honor the intent by not loading anything.

            (and

                ;;  Let's ensure demand loading is enabled, lest the
                ;;  invocation of the OpenDCL command be pointless.

                (= 2 (boole 1 (getvar "DEMANDLOAD") 2))

                ;;  We're good, invoke the OpenDCL command ...

                (vl-catch-all-apply 'vl-cmdf '("OPENDCL"))

                ;;  Is core OpenDCL functionality now defined?

                dcl-getversionex
            )

            ;;  If we got here the opendcl runtime was not loaded.
            ;;  Inform the user of the sad news.

            (princ "Error: OpenDCL Runtime could not be loaded.\n")
        )

        dcl-getversion

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Embedded_Project ( projname password alias / bytes rtype )

        ;;  Attempt to retrieve an embedded ODCL project (lisp) from
        ;;  the text resources in the current vlx file. If successful
        ;;  attempt to import via odcl_project_import, returning it's
        ;;  result to the caller, otherwise nil.

        (cond

            ;;  At this point the OpenDCL Runtime should already
            ;;  have been loaded; if not, then either the
            ;;  initialization failed (and this function is being
            ;;  inappropriately called) or older OpenDCL runtimes
            ;;  have been loaded. Either way alert and bail.

            (	(null dcl-project-import)

                (princ "OpenDCL version 5.0 or newer is required.\n")

                nil
            )

            ;;  Trap unsuccesful retrieval of project from the vlx
            ;;  text resources; alert and bail.

            (	(or
                    (null (setq bytes (vl-get-resource projname)))
                    (not (eq 'str (setq rtype (type bytes))))
                    (eq "" bytes)
                )

                (princ
                    (strcat
                        "Failed to load <"
                        projname
                        "> ODCL resource from vlx file.\n"
                    )
                )

                nil

            )

            ;;  Call dcl-project-import and return the result to the
            ;;  caller if successful ...

            (	(dcl-project-import bytes password alias)  )
        )

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_File_Project ( projname reload password alias / samples )

        ;;  Attempt to load an ODCL project file by trying to load
        ;;  it from the support path; if that fails, try to load it
        ;;  from the OpenDCL Studio samples folder.

        (cond

            ;;  At this point the OpenDCL Runtime should already
            ;;  have been loaded; if not, then either the
            ;;  initialization failed (and this function is being
            ;;  inappropriately called) or older OpenDCL runtimes
            ;;  have been loaded. Either way alert and bail.

            (	(null dcl-project-load)

                (princ "OpenDCL version 5 or later is required.\n")

                nil
            )

            ;;  Call dcl_project_load and return the result to the
            ;;  caller if successful ...

            (	(dcl-project-load projname reload password alias)  )

            ;;  Since this file is installed along with the other
            ;;  OpenDCL samples, and since the samples folder is not
            ;;  in the support path, try loading from there...

            ;;  Unless you're writing an OpenDCL sample,
            ;;  don't leave this part in your own code!

            (	(setq samples
                    (cond
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

                (dcl-project-load (strcat samples projname) reload password alias)
            )
        )

    ) ;;------------------------------------------------------------------------

    (defun c:DistSample/MainForm/OkButton#OnClicked ( )

        (dcl-MessageBox "Press [Ok] to terminate ..." "About to exit ...")

        (dcl-form-close DistSample/MainForm)

    ) ;;------------------------------------------------------------------------

    (defun _Main ( / odclProjName )

        ;;  Wrap up and call functionality defined in this file.

        (setq odclProjName "DistSample.odcl") ;; don't include .lsp extension

        (if

            (and

                (_Load_ODCL_Runtime)

                ;;  Attempt to load the ODCL project.

                (or
                    ;;  ODCL project retrieved from vlx resources

                    (_Load_ODCL_Embedded_Project odclProjName nil nil)

                    ;;  If it couldn't be loaded from resources, try
                    ;;  loading from a separate file (this could be
                    ;;  useful during development, but you may wish
                    ;;  to remove this option before shipping).

                    (_Load_ODCL_File_Project odclProjName t)
                )
            )

            (if
                (null
                    (dcl-Form-Show DistSample/MainForm)
                )

                (princ "Failed to show form: DistSample/MainForm\n")
            )
        )

        (princ)

    ) ;;------------------------------------------------------------------------

    ;;==========================================================================
    ;;
    ;;  Invoke _Main ...
    ;;
    ;;==========================================================================

    (_Main)

)

(princ "OpenDCL DistSample1 (ver 1.3) loaded. Enter \"DistSample1\" to execute.\n")

(princ)