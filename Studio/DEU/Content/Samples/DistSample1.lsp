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
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Dependencies:
    ;;
    ;;  1.  AutoCAD 2004+, OpenDCL 4.0, beta 18+.
    ;;
    ;;  2.  DistSample.odcl | .lsp. With a form varnamed DistSample_MainForm
    ;;      with a child button varnamed DistSample_MainForm_OkButtonOkButton
    ;;      with the event OnClick enabled. The project is not passworded.
    ;;
    ;;  3.  If run as a .lsp file expects to find DistSample.odcl as described
    ;;      above in the support path.
    ;;
    ;;  4.  If run as a .vlx expects to find DistSample.odcl.lsp as described
    ;;      above embedded as a text file resource in the the vlx.
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Noteable:
    ;;
    ;;  1.  See function _Load_ODCL_Embedded_Project which attempts to load 
    ;;      the OpenDCL project data from the vlx text resources.    
    ;;
    ;;==========================================================================

    (   /

        ;;  Locals.

        c:DistSample_MainForm_OkButton_OnClicked
        _Load_ODCL_Runtime
        _Load_ODCL_Embedded_Project
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Runtime ( / loaderp proc_arch arxname arxpath errmsg )
    
        ;;  The loading attempt order (if not already loaded):
        ;;
        ;;  1.  If OpenDCL has been installed on this machine via the
        ;;      installer use the path to the runtime as specified in 
        ;;      the registry for demand loading -- but load via arxload
        ;;      regardles the demand load setting.
        ;;
        ;;  2.  If the OpenDCL runtime is found in the support path use 
        ;;      arxload to load said runtime.
        ;;
        ;;  3.  If OpenDCL has been installed on this machine via the
        ;;      installer *AND* demand loading is enabled force the load-
        ;;      ing of the runtime via invoking the OpenDCL command.
        ;;
        ;;  Verbosely / gratuitously annotated, which makes this function 
        ;;  appear far more ambitious than it actually is.

        (or

            ;;  Already loaded, vacate now (return t to caller).

            dcl_getversionex
            
            ;;  SECTION 1. Attempt to load the runtime via arxload using 
            ;;  the path associated with the opendcl demand loader (the 
            ;;  intent: Use said path but do not use the OpenDCL command 
            ;;  to induce loading the arx). In other words don't use 
            ;;  command / vl-cmdf.
            
            (and 
            
                ;;  Is demand loading is specified for OpenDCL?
            
                (setq arxpath
                    (vl-registry-read
                        (strcat
                            "HKEY_LOCAL_MACHINE\\"
                            (vlax-product-key)
                            "\\Applications\\OpenDCL"
                        )
                        "Loader"
                    )                
                )
                
                ;;  We'll use the following variable in SECTION 3.
                
                (setq loaderp t) 
                
                ;;  Is she an arx file?
                
                (wcmatch (strcase arxpath) "*`.ARX")          
                
                ;;  Was the load attempt successful?
                
                (arxload arxpath nil)
                
                ;;  Is core OpenDCL functionality now defined?
                
                dcl_getversionex
            )                    

            ;;  SECTION 2. Attempt to arxload the runtime explicitly by 
            ;;  finding it in the support path.
            
            (and

                ;;  Determine the appropriate arx module for the pro-
                ;;  cessor and the AutoCAD version.

                (setq arxname
                    (strcat "OpenDCL"
                        (if
                            (and
                                (setq proc_arch
                                    (getenv "PROCESSOR_ARCHITECTURE")
                                )
                                (< 1 (strlen proc_arch))
                                (eq "64"
                                    (substr
                                        proc_arch
                                        (1- (strlen proc_arch))
                                    )
                                )
                            )
                            ".x64."
                            "."
                        )
                        (substr (getvar "acadver") 1 2)
                        ".arx"
                    )
                )
                
                ;;  Is the arxfile in the support path somewhere?
                
                (setq arxpath (findfile arxname))
                
                ;;  Was the load attempt successful?                
                
                (arxload arxpath nil)
                
                ;;  Is core OpenDCL functionality now defined?
                
                dcl_getversionex                
            )
            
            ;;  SECTION 3. If demand loading is enabled use the OpenDCL 
            ;;  command to induce the loading of the OpenDCL runtime. 
            ;;  If we got here it possibly means the runtime is no lon-
            ;;  ger an arx file (maybe an exe) or the arxload attempt in
            ;;  SECTION 1 failed for some reason. Regardless, it appears
            ;;  a demand loader is still present, so let's try to use it 
            ;;  to load the OpenDCL runtime.

            (and
                
                ;;  The following variable 'loaderp' is initialized from 
                ;;  the first attempt to load the runtime in SECTION 1. 
                ;;  If null it means no OpenDCL loader is present on this 
                ;;  machine so there's no point in invoking the OpenDCL 
                ;;  command.
                
                loaderp 

                ;;  A loader is defined but let's ensure demand loading 
                ;;  is enabled, lest the invocation of the OpenDCL com-
                ;;  mand be pointless.

                (= 2 (boole 1 (getvar "DEMANDLOAD") 2))
                
                ;;  We're good, invoke the OpenDCL command ...
                                                
                (vl-catch-all-apply 'vl-cmdf '("opendcl"))
                
                ;;  Is core OpenDCL functionality now defined?
                
                dcl_getversionex
            )
            
            ;;  If we got here the opendcl runtime was not loaded.

            (progn
            
                ;;  Inform the user of the sad news.
            
                (princ 
                    (strcat
                        "Fehler: "
                        (if arxpath 
                            (strcat arxpath " <kann nicht geladen werden>")
                            "OpenDCL Laufzeitumgebung <wurde nicht gefunden>"
                        )
                        ".\n" 
                    )
                )
                
                nil
            )
        )
        
        dcl_getversion

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Embedded_Project ( projname password alias / bytes rtype )

        ;;  Attempt to retrieve an embedded ODCL project (lisp) from 
        ;;  the text resources in the current vlx file. If successful
        ;;  attempt to import via odcl_project_import, returning it's
        ;;  result to the caller, otherwise nil.

        (cond
        
            ;;  At this point OpenDCL runtimes should have already 
            ;;  been loaded by initializing code, so either the 
            ;;  initialization failed (and this function is being 
            ;;  inappropriately called) or older OpenDCL runtimes 
            ;;  have been loaded. Either way alert and bail.

            (   (null dcl_project_import)

                (princ "Es wird mindestens die Version OpenDCL Version 4.0, Beta 18 oder höher benötigt.\n")

                nil
            )
            
            ;;  Trap unsuccesful retrieval of project from the vlx
            ;;  text resources; alert and bail.

            (   (or
                    (null (setq bytes (vl-get-resource projname)))
                    (not (eq 'str (setq rtype (type bytes))))
                    (eq "" bytes)
                )

                (princ
                    (strcat
                        "kann nicht geladen werden <"
                        projname
                        "> OpenDCL-Projektdaten aus einer VLX-Datei.\n"
                    )
                )

                nil

            )

            ;;  We've got data, call odcl_project_import and
            ;;  return result the result to the caller ...

            ;;  Caller provided credentials, possibly an alias.

            (   (eq 'str (type password))
                (if (eq 'str (type alias))
                    (dcl_project_import bytes password alias)
                    (dcl_project_import bytes password)
                )
            )

            ;;  Ahhh, trusting soul (no password).

            (   (dcl_project_import bytes)  )
        )

    ) ;;------------------------------------------------------------------------

    (defun c:DistSample_MainForm_OkButton_OnClicked ( )

        (dcl_MessageBox "Drücken Sie die Schaltfläche OK, um abzubrechen" "Info-Dialog schließen ...")

        (dcl_form_close DistSample_MainForm)

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

                    ;;  ODCL project retrieved from .odcl file for
                    ;;  example, during the .lsp development (you may
                    ;;  wish to remove the following if your product is
                    ;;  ready for final delivery to customers / clients).

                    (dcl_project_load odclProjName t)
                )
            )

            (if
                (null
                    (or
                        (dcl_Form_IsActive DistSample_MainForm)
                        (dcl_Form_Show DistSample_MainForm)
                    )
                )

                (princ "Dialog konnte nicht angezeigt werden: DistSample_MainForm")
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

(princ "DistSample1 (ver 1.2) geladen, geben Sie DistSample1 ein, um das Beispiel zu starten.")

(princ)