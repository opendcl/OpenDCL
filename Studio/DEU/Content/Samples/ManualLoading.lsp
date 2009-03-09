;;; ManualLoading.lsp
;;; für OpenDCL Ver 4.0
;;; Verändert kwb 20070225 GMT00:00:00
;;; Verändert orw 20070518 GMT07:30:xx :: added 64 bit arx file selection
;;;                                       returns "OpenDCL.x64.17.ARX"
;;; Verändert Edit kwb as suggested by MichaelP 20070608 GMT00:30:xx
;;;                :: change location of dcl_GETVERSIONEX test.
;; 

;; Dieser Quellcode-Ausschnitt lädt die OpenDCL.##.arx-Dateien, sofern sie nicht bereits geladen sind.
;; Die Funktion gibt T zurück, wenn die ARX-Dateien geladen sind.
;;
;; Diese Funktion braucht nicht ausgeführt zu werden,
;; wenn die OpenDCL.##.arx bereits beim Start oder per OPENDCL-Befehl geladen wird
(OR dcl_GETVERSIONEX
    (   (lambda ( / proc_arch arxname arxpath )

            ;;  Ermittle die geeignete Laufzeitumgebung
            ;;  aus der Prozessorarchitektur und AutoCAD-Version

            (setq arxname
                (strcat "OpenDCL"
                    (if
                        (and
                            (setq proc_arch (getenv "PROCESSOR_ARCHITECTURE"))
                            (< 1 (strlen proc_arch))
                            (eq "64" (substr proc_arch (1- (strlen proc_arch))))
                        )
                        ".x64."
                        "."
                    )
                    (substr (getvar "acadver") 1 2)
                    ".arx"
                )
            )            
            ;;  Den Anwender darauf aufmerksam machen, dass er
            ;;
            ;;      (A) die ARX-Datei suchen soll
            ;;      (B) die ARX-Datei laden soll
            ;;
            ;;      und NIL zurückgeben
            ;;
            ;;  Ansonsten ohne Meldung T zurückgeben
            
            (cond
                (   (null (setq arxpath (findfile arxname)))
                    (alert (strcat "Ich kann die Datei " arxname " nicht finden.\nMöglicherweise müssen Sie den Pfad zu den AutoCAD-Supportpfaden hinzufügen."))
                )
                (   (null (arxload arxpath 'nil))
                    (alert (strcat "Ich kann die Datei " arxname " nicht laden."))
                )
                (   t   ) 
            )
        )
    )
)

