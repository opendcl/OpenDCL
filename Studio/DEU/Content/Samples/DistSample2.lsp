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
            '("YWt6AykbAACPzuumBuKTJjUxLT9qgFBxx173ev1VMzR6v9Z6B2gd9j8cPiU+MvYPPd5TjFa7XoYm"
	      "N5y8+wUpUCOEMgJaUCOXsGGeuOP33NKi1qqRshMFkUM7NI5vdwnkxwnARxmwskmZr9jFrNUXzili"
	      "uX6Y7jx8oxY8bOkPt7KJHPSde+K9ZhhH6dr4e54TZy6wyVarC1/4TpwWOEWkrvxdkg/w8AyquPuT"
	      "5p4W8owwUOnHTplsFtsgTa/677oecO1o9VOW3rBGm08UCA8zoqaRTNVFR2ylH7S9uNyWyw6qONMP"
	      "0JaeENOqV7gwT1RZSBNxEnU7JzrvY9uYavfhXIMHLMoh3URVsVakvVb/2adJIv1hBiDFpLuJOoC8"
	      "uziB57yxawDUjgkJQW+myS7ouw+/hQNqZHmX6XpXipxaXMLyX2znvUqZGJbH7t9pXORMgOOZBvoE"
	      "mQ0NaYdgGSGeOeCIZeGDoxW7vlaMUNCvGw//XYV5CBo39nGSxXQhL4ZrNEPaH8EtjNV0AS2G5GFl"
	      "uWldnbu9B/BhmpOKHq/O6LYhtY87zg0wZwqXY2rCvRGQfvNb8mWcmBfpJICdQyg/ijpHxzyf6rEA"
	      "G+qpYRzh2otaQV/ERTepB/3RIKijLwgUIocLvz1QAFGG0uwBipN1xaDr8fYCvrt9/LHWnvNWYiuG"
	      "yrzOgpmLeaGg83uhVoyip96Qv23g7fxVA5+sZr1PEghR5zdPh+yq9S/oHnKm8SoGQC6G37yJddo9"
	      "TLm5v010sZiGJyHbVrNXh8q8yasAtccFCY7U1RGQkc0kgVcp6mIi2agBD7Fdce3w3F9Xt9Fm6/Cb"
	      "b6CMxveTkgNpNDYxayBvvFvHdQ2a4PozX1EfZYTG9y1zG6V3zdvtxF94/0YM7N0AQJlHVddZqw+v"
	      "0jfmruZLpEylipGsg9u1FCcDiar24z2QU5aK6dWN2IHOmzGBdO9pDMVwbSNQ6qV1+PfZwqq8ISAQ"
	      "nHHqaQFHp8mvaiFdYqlXVgMoID+H1Xu+ksEwfyanOn9OSJ+CvFY4AbrMWj01Lrtxs9sVsy0kMcpd"
	      "zS6g85JZptXyn+5d1jwgJYwKZI+f84rToOhujrGI/oGInVHiAdkRi+iICJ/amXSK06B8gH+B/gG+"
	      "QZ5hjnGGeYJ9gKX1ATHKRU3syYEhfZnx9dsh0oJhAIOBerXL8IY="
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