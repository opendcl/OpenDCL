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
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Dependencies:
    ;;
    ;;  1.  AutoCAD 2004+, OpenDCL 4.0, beta 18+.
    ;;
    ;;  2.  DistSample.odc (OpenDCL project) saved as DistSample.odcl.lsp 
    ;;      (Base64 encoded stream wrapped in lisp). Said project has a form 
    ;;      varnamed DistSample_MainForm with a with a child button varnamed 
    ;;      DistSample_MainForm_OkButtonOkButton with the event OnClick enabled. 
    ;;      The project is not passworded.
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Noteable:
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

    (defun _Load_ODCL_Stream ( password alias / project rtype )
    
        ;;  This is a reformatted copy of the content from 
        ;;  the OpenDCL project file DistSample.odcl.lsp.
        ;;  The only reformatting is a change in the line
        ;;  length so it looks tidy.
    
        (setq project
            '("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")   
        )
        
        (cond
        
            ;;  At this point OpenDCL runtimes should have already 
            ;;  been loaded by initializing code, so either the 
            ;;  initialization failed (and this function is being 
            ;;  called inappropriately) or older OpenDCL runtimes 
            ;;  have been loaded. Either way alert and bail.

            (   (null dcl_project_import)

                (princ "Es wird mindestens die Version OpenDCL Version 4.0, Beta 18 oder höher benötigt.\n")

                nil
            )
            
            ;;  All appears ok, call dcl_project_import with our
            ;;  data, returning the result to the caller.

            (   (dcl_project_import project password alias)   )
        )

    ) ;;------------------------------------------------------------------------

    (defun c:DistSample_MainForm_OkButton_OnClicked ( )

        (dcl_MessageBox "Drücken Sie die Schaltfläche OK, um abzubrechen" "Info-Dialog schließen ...")

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

(princ "DistSample2 (ver 1.2) geladen, geben Sie DistSample2 ein, um das Beispiel zu starten.")

(princ)