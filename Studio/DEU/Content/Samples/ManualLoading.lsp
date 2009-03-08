;;; ManualLoading.lsp
;;; For OpenDCL Ver 4.0
;;; Edit kwb 20070225 GMT00:00:00
;;; Edit orw 20070518 GMT07:30:xx :: added 64 bit arx file selection
;;;                                       returns "OpenDCL.x64.17.ARX"
;;; Last Edit kwb as suggested by MichaelP 20070608 GMT00:30:xx
;;;                :: change location of dcl_GETVERSIONEX test.
;; 

;; This code block loads the OpenDCL.##.arx files if not already loaded
;; Note, Loader will return T if loaded or nil otherwise.
;;
;; If the OpenDCL.##.arx is loaded at startup or demand loaded
;; this routine need never run.
(OR dcl_GETVERSIONEX
    (   (lambda ( / proc_arch arxname arxpath )

            ;;  Determine the appropriate arx module for
            ;;  the processor and the AutoCAD version.

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
            ;;  Alert the user of a failure to:
            ;;
            ;;      (A) Find the arxfile, or 
            ;;      (B) Load the arxfile.
            ;;
            ;;      and return nil.
            ;;
            ;;  Otherwise just quietly return t.
            
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

