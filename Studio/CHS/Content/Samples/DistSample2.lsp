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

        c:DistSample_MainForm_OkButton_OnClicked
        _Load_ODCL_Runtime
        _Load_ODCL_Stream
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Runtime ( / )

        (or

            ;;  Already loaded, vacate now (return t to caller).

            dcl_getversionex

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

                dcl_getversionex
            )

            ;;  If we got here the opendcl runtime was not loaded.
            ;;  Inform the user of the sad news.

            (princ "Error: OpenDCL Runtime could not be loaded.\n")
        )

        dcl_getversion

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Stream ( password alias / project rtype )

        ;;  This is a reformatted copy of the content from
        ;;  the OpenDCL project file DistSample.odcl.lsp.
        ;;  The only reformatting is a change in the line
        ;;  length so it looks tidy.

        (setq project
            '(  "YWt6AxkbAADwrWUpBuKTJkUxai9qgPiZezdaVvf77Fv+PCr8wk+4Zjpt63shz85fzv5etTx+b1U3"
                "UlKS01JTA1hAbDVVk4J/SAhY9CSn1tKhx83NzQ1obzs7vKK348ehQ/EFsUOZ5aeSmJocbNcgovsv"
                "Mh14Ga7xDzJc8F9GfiBrfDsC9CwLeRwWuV5krvwYShxRhV45hUYU2B91+EeU2cLzmEypsMdfSi/R"
                "/xamNByQ3DePpvYolHIuYN0YeRN404FwFaO474YQ2cKjBGrDOPDk10s+9SjQqDrjIhJs2cwdpci/"
                "RraPbjtYNLMvWg5qbeYc9phldCwcOTKzU/sZawDwAnG1s8IojLV0aUY/gju3mWpINIF7tMKD2XxP"
                "2e/58hjQhH/BmwfkTGGhaIctYSWDuXRBL4bvvCGDizoe0N2uvf381Xw5os4qczsu9SAKn07ER+Cc"
                "skptDvfd5utwLw3+CTfFol1IN2+yrATwsR8cjy35BObp/4vIUssgYcxsI5U5C+BdJVbHoNeQt1Tq"
                "IPLf7UXin0s/Qdha7takZogdG1WJP4XUnDL8GW/ZIPJM96aa1bqhKfsPKZ8FdKGHN9+4rScKj+a8"
                "nZeND0A1azNWhzv7EdaLaKyZmZw7IaEos3kbgUAjO1DQKIhhg4Mmh2w6tv+P2ObRCgSVeml6rOcB"
                "ekUnNt0y/gfelo0u6wnPvPf+2exWrWMfYR+knMJcUv1GUF7tCEscCS2HV4mbKXrhio9lv9nqgiMe"
                "I+ZkIQAwmkNAkG8CPK/IJZ/WvLlr+G2iEe4YpQHJprNQC3BnOmI/9PbM7O4vlmNaHC9JKWbOGZPT"
                "29aGMdcdXSb4ROGoXwtMwGEIhQeXjiMHeMtsttyoqM0y3eo4+gPsVwyhlr3IXqIJA/zbLoGJ9omy"
                "oaH8hIDr9eGjAELk9mt2bI/+bVrc61E9ujR69wZzAf72P5yTJleP6F60TKrVDiwNDsGcx0aok86i"
                "jAbrjJw/WFWTIyQZyksbWbEkiD2S26gRumvqXNWzTX7RNbW6VZenYKkp5XGdcZ2hvo9ZKaeoBIHV"
                "EzdHfNHFf4MsIZWD0cj7j/mP+Y/5j/mP+Y/5j/mP+Y8R0oZF3XktXQd4gUsOgfbmgf+bgY6dgUXO"
                "SfQ3"
            )
        )

        (cond

            ;;  At this point OpenDCL runtimes should have already
            ;;  been loaded by initializing code, so either the
            ;;  initialization failed (and this function is being
            ;;  called inappropriately) or older OpenDCL runtimes
            ;;  have been loaded. Either way alert and bail.

            (	(null dcl_project_import)

                (princ "OpenDCL version 5.0 or newer is required.\n")

                nil
            )

            ;;  All appears ok, call dcl_project_import with our
            ;;  data, returning the result to the caller.

            (	(dcl_project_import project password alias)   )
        )

    ) ;;------------------------------------------------------------------------

    (defun c:DistSample_MainForm_OkButton_OnClicked ( )

        (dcl_MessageBox "Press [Ok] to terminate ..." "About to exit ...")

        (dcl_form_close DistSample_MainForm)

    ) ;;------------------------------------------------------------------------

    (defun _Main ( / odclProjName )

        ;;  Wrap up and call functionality defined in this file.

        (if

            (and

                (_Load_ODCL_Runtime)

                ;;  Attempt to load the ODCL project from an inline stream.

                (_Load_ODCL_Stream nil nil)

            )

            (if
                (null
                    (dcl_Form_Show DistSample_MainForm)
                )

                (princ "Failed to show form: DistSample_MainForm\n")
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

(princ "OpenDCL DistSample2 (ver 1.3) loaded. Enter \"DistSample2\" to execute.\n")

(princ)