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
    ;;      varnamed DistSample_MainForm with a child button varnamed
    ;;      DistSample_MainForm_OkButtonOkButton with the event OnClick enabled.
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

    (   /

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
            '(
                "YWt6A20SAADAoypBBuKTKTUSKztmgFDDvn83WlYjKwzT3hP22gg0l0u45lXk1sPKQLS8Mz0RRSxX"
                "U3/a5uamutpQoubmSue6e3VKT29h8/G6K5281uP71Qg9adp1MnsXSdh364+FmaKgDbHKexIeP7xy"
                "DbCC8YeBEYC5l5s/Cb3fwshgOkEwn0oguW9iuXdy3nFnfgESeFwv7L+JkEI0Fj1R5M5hUxa47SLD"
                "okO7I3VxkfmYkHabeRY8BhOaiz8v0ffqiOwskqb3uxjKLNn5jGxLjUKtlZOZg5d9oveTSLSlwjuv"
                "FZjQ+yRDQq80EfRESwY+8hguYMv2TKbOMi9qa9en8T/C9l/imkvdIc8dJyAo8iA9aCtdBEFPhlmP"
                "OEVshMK2ATIW0YEDPoFurqF7iiycMXqv+abCMol0gyaUdDPVm1f5eAqeXuA5RYlZt0e7dcC9I7c1"
                "hdanvdn6K+BHYfpisI6KJ8BLF8+t9gP2ADWHBRZLAaMmb7b4migRDGmD9TO16eziOuF4iuf+gGdl"
                "1fz+lhxeYXCWlX8DSDOFPNY/R9UFKWo8efDfvnbjmGKcQyhV+nP2VDeUNtCpVorY6sfzUZ4PeMzh"
                "uNwR/IyHSDaRqP5gMiekKBreDSI5Z5h/sGKtRP7JLiMIBZqiJh4ZGVwBKZaw+sBaitX/gtHqIToF"
                "bIcf72E0lj08wWzqEwU+aX2Em/44WPNJIxgLySkYoybvxln8RiprnC/fydt03d/62c8gIbxnGu1z"
                "LtE658osigsu9ml8hNX+gIxo+0EuxzEyYXWWneuFG4Dvsa8uXgFF3kF0giPTukPYEVqEQAABeZnR"
                "KkA5T8NFRgFTv11xoQbcX7O50HZkETVF3aKdFQVHoMkUw9SRkcIiRiV3JlLRFLM/0haz+aEeZzbE"
                "DSJZlkyAXBThmoYlR7aKkMUF84Ut/JEUjokd2aan5aJZEpojNkmMTecKXQUHK51A9AqdAP5FrsaX"
                "OAOaCOUZ6zqh2isJGyjKlCMTU5GBWNNLwfCojEhvgavQvzWTZwS3DBCZ2kfJu0EbavTTGzWMEJ6F"
                "R9CToA7Hji3LJkM4gkHy8rpR56IyMjIenJx6mAGB4kH7GtYG7NbT0zsjI2OUlZWvpqb20NBQavU7"
                "anR7mo0H8sKZrqeXjnfYntAmgkwLcy3pUZ2dUVBOmxwzDWdQdREi8lSMagc0BlAor5oVEEqH5AKT"
                "nB34Aj3AXx+2SbDl/Fuh7BG3yRomjGMXOaQnEaiHJRZQvTtgR3w3V8dlh7re9j8vpz1ceDty9GZq"
                "T1Ycjk73P2/9XHg7cvRmQgocLrre9j9v/Vx4O3n0RmoPVvG9u6mNNhpkULWbnFVt9dgNMznkOEvP"
                "hk4u1sevutz3u/n7RkoPFpyuOAZQcGsPFhyuut73PzzZ9z1t/Vh4VYmYrfnuAJwXtZ8rYO8vPMVG"
                "qooT9reu1999PTL4wHP7+DjybyVQMPmJM0CDZYRJilFXTIoR+jFy+GbyDUATD1jKiVC2WsPoZlKD"
                "JxzNuh+XsdkoDk546EbSnp67AsQHtmz1W2g1buhwA3du9Zs/NVs1+FtoV3VVJuZtNVIonJ/PNks2"
                "aPJRWzly8GZiT0YcDrqetfdmUk9GHA66Hve/6WZPRhwOup72v299Mw3vMXLwZmJPRlWBL99zZA1C"
                "ltbay5r2BwqcPRL18Jhphk0KCQkdBgZm3d3dZmVlvZqaWri5uU9DQ/Pn52eysLAcEBBwdXV1JiIi"
                "uoIL+Y2ZsM/x290Drw3HhlstJgIazSfp02QVA1+oZImEB2nygQUAFxe3UyvzhZmR4cHpsGn0segJ"
                "jDCBFPYBxvLSDwxjswy5uQG0K+fe"
            )
        )

        (cond

            ;;  At this point OpenDCL runtimes should have already
            ;;  been loaded by initializing code, so either the
            ;;  initialization failed (and this function is being
            ;;  called inappropriately) or older OpenDCL runtimes
            ;;  have been loaded. Either way alert and bail.

            (   (null dcl_project_import)

                (princ "OpenDCL version 5.0 or newer is required.\n")

                nil
            )

            ;;  All appears ok, call dcl_project_import with our
            ;;  data, returning the result to the caller.

            (   (dcl_project_import project password alias)   )
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