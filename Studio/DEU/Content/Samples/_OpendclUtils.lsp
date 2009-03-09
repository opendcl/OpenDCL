

(defun LoadRunTime ( / )
    ;; Laden der Laufzeitumgebung über den OPENDCL-Befehl. Das setzt aber voraus,
    ;; dass an jedem PC vor dem Start von AutoCAD die Laufzeitumgebung bzw. das Studio
    ;; installiert wurde. Wurde AutoCAD nach der OpenDCL Laufzeitumgebung installiert,
    ;; muss die Installation der Laufzeitumgebung "repariert" werden, wobei nur zusätzliche
    ;; Registry-Einträge für die neue AutoCAD-Version ergänzt werden.
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
    ;; Aufruf von (dcl_PROJECT_LOAD mit dem  <CONTROL>. Kein Neuladen, kein Projektalias
    ;; Beachten Sie, dass ohne das Neuladenargument, das Projekt nur einmal geladen wird
    ;; Bei jedem erneuten Laden wird zuerst geprüft, ob das Projekt bereits geladen ist.
    (cond
      ;; Supportpfade nach der .ODCL-Datei durchsuchen und neuladen
       ( (if (setq fn (findfile proj))
           (dcl_PROJECT_LOAD fn)
       ))
      ;; .ODCL-Datei aus dem Ordner der Beispieldateien des Installationsverzeichnisses von OpenDCL-Studio laden
       ( (if 
           (or
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit Position
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit Position
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit Position
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit Position
           )
           (dcl_PROJECT_LOAD (strcat fn proj))
       ))
      ;; Die Projektdatei kann nicht geladen werden, Ausgabe einer Fehlermeldung mit Hinweis auf die notwendigen Änderungen
      (T (alert (strcat "\"" proj "\" kann nicht geladen werden, fügen Sie den Pfad zu den AutoCAD-Supportpfaden hinzu, damit es geladen werden kann!"))
        (EXIT)
      )
    )
)


(princ)