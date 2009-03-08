

(defun LoadRunTime ( / )
    ;; Demand load the OpenDCL.##.ARX. This requires the OpenDCL Runtime or Studio to be installed on each PC first.
    (cond
      ( (= 'EXRXSUBR (type dcl_getversionex)) )
      ( (= 2 (boole 1 (getvar "DEMANDLOAD") 2))
        (command "_OPENDCL")
        (if (/= 'EXRXSUBR (type dcl_getversionex))
          (progn
            (alert "Die OpenDCL Laufzeitumgebung kann nicht geladen werden. Reparieren Sie bitte die OpenDCL-Installation und versuchen Sie es erneut.")
            (exit)
          )
        )
      )
      ( (progn
          (alert "Die OpenDCL Laufzeitumgebung kann nicht geladen werden, weil das DemandLoading deaktiviert ist.\nLaden Sie die OpenDCL-ARX-Datei manuell oder aktivieren Sie das DemandLoading\nfür Befehle durch Ändern der Systemvariable DEMANDLOAD auf die Werte 2 oder 3.\nBeachten Sie den Inhalt der Datei \"ManualLoading.lsp\" zur Demonstration.")
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
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
           )
           (dcl_PROJECT_LOAD (strcat fn proj))
       ))
      ;; The project failed to load, so report or log the error exit now (or take corrective action and try again)
      (T (alert (strcat "\"" proj "\" kann nicht geladen werden, fügen Sie den Pfad zu den AutoCAD-Supportpfaden hinzu, damit es geladen werden kann!"))
        (EXIT)
      )
    )
)


(princ)