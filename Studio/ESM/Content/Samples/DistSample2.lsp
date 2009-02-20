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
                        "Error: "
                        (if arxpath 
                            (strcat arxpath " <failed to load>")
                            "OpenDCL runtime <not found>"
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
            '(
                "YWt6A94IAAAHNxDkBuLD60URrS5qufHlyvrUUl"
                "Oj3kxcU1e2751EM/zmFh/+2n/8ZiZcucAvM2JZ"
                "9K1sWHg42vAy8vJCW2D8MX5lYf77Z3SSQvwRxk"
                "lJx1l0LDuPboeCCqCJgONnduEWrHXkrZUQrlAn"
                "i9g56u7DQX5Od5L3mZLS3dkzPU1iTdvnKMTUJw"
                "T4FhhIMyhyo9KUZ2XEo8bP5NNmCETZR/DgrbLy"
                "4KzMOOgW+BNurdfaKdPrarNg/2azNSEJaiAbcL"
                "zkjm3Y5tOKkoXsw1OnYVqF2BoB6ZLwGlVOv2X2"
                "a1KpQaEWq1cBdc2O6fnEw22fDRz9m78yKV5UOb"
                "M8AZR5csNs9gXDvpZbOLFPhF9UVHJjaQOU3DFL"
                "LyIhtdVTbErIJEgvN8M8YvlG1JXVpSA508RgJz"
                "CshvrWDEMClWMhepaPj8JZBQsYugRtiJdvZVdp"
                "FRnJpJP4IVCEFeAVqfY9neA2uYw2zGE1iD23iI"
                "RdVQJ1S/KUg1fqZj3R+2mhK315oL6WkOb1uhmi"
                "pTtETBATqgmcsYAHFi/is0w9LVylmsfb8KXYjl"
                "fA9rGOM+e4tmmncGs0FLeMfcs6JBArxLX1soFg"
                "ME9RwhBPCjfe8XIjuCDF3AixgPsNAb8NA+zcQI"
                "kA+S4iDIf0C8pWrkv4SHqkHpZccLIdKr6WfDhZ"
                "jgp9tKV62EKkh/BsHwY3KGCCHTuPKLhVlCXJpa"
                "NRO2OyJWucL0+SMyw5OTcUVtna16UclQLQ1oY9"
                "miB9tI1/V5GOqzNJLv3YawAn1fMohueLpbeDbZ"
                "xlr7JQFQc+5Y1Su1dPu3YTUH6smGd7F+0Fc5ad"
                "Zl4Dov+B0/H7XwH+oRPtx1w="
            )        
        )
        
        (cond
        
            ;;  At this point OpenDCL runtimes should have already 
            ;;  been loaded by initializing code, so either the 
            ;;  initialization failed (and this function is being 
            ;;  called inappropriately) or older OpenDCL runtimes 
            ;;  have been loaded. Either way alert and bail.

            (   (null dcl_project_import)

                (princ "OpenDCL version 4.0, beta 18 or newer is required.\n")

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
                    (or
                        (dcl_Form_IsActive DistSample_MainForm)
                        (dcl_Form_Show DistSample_MainForm)
                    )
                )

                (princ "Failed to show form: DistSample_MainForm")
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

(princ "DistSample2 (ver 1.2) loaded, type DistSample2 to execute.")

(princ)