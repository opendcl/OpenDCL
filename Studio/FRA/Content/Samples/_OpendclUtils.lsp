

(defun LoadRunTime ( / )
    ;; Demand load the OpenDCL.##.ARX. This requires the OpenDCL Runtime or Studio to be installed on each PC first.
    (cond
      ( (= 'EXRXSUBR (type dcl_getversionex)) )
      ( (= 2 (boole 1 (getvar "DEMANDLOAD") 2))
        (command "_OPENDCL")
        (if (/= 'EXRXSUBR (type dcl_getversionex))
          (progn
            (alert "The OpenDCL Runtime module failed to load. Please repair the OpenDCL installation and try again.")
            (exit)
          )
        )
      )
      ( (progn
          (alert "The OpenDCL Runtime module cannot be loaded because demand loading is disabled.\nLoad the OpenDCL ARX file manually, or enable demand loading for commands by setting the DEMANDLOAD system variable to 2 or 3.\nSee the \"ManualLoading.lsp\" sample for a demonstration.")
          (exit)
        )
      )
    )
)


(defun LoadODCLProj (proj / fn)
    ;; Call (dcl_PROJECT_LOAD with the <CONTROL>. No re-load, no ProjectAliasName
    ;; Note that without the re-load flag the ODCL is only loaded once, each time (dcl_PROJECT_LOAD is called it tests first if the sProject is already loaded.
    (cond
      ;; Search the support paths for the .ODCL file & load it.
       ( (if (setq fn (findfile proj))
           (dcl_PROJECT_LOAD fn)
       ))
      ;; Load the .ODCL file from the default installed "Samples" folder.
       ( (if 
           (or
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
           )
           (dcl_PROJECT_LOAD (strcat fn proj))
       ))
      ;; The project failed to load, so report or log the error exit now (or take corrective action and try again)
      (T (alert (strcat "\"" proj "\" failed to load, you may need to add it to an Acad support path for it to load correctly!"))
        (EXIT)
      )
    )
)


(princ)