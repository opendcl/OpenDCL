(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"AVI\" ein, um das Beispiel zu starten.\n")
)


(defun c:avi ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Animation.odcl")

;; An dieser Stelle bleibt der Ablauf dieses Programms stehen bis der Dialog geschlossen wird
;; In der Zwischenzeit verwalten die Ereignisfunktionen den Dialog.  
  (dcl_FORM_SHOW Animation_DclForm1)
  (PRINC)
)




(defun c:DclForm1_OnInitialize ( / fn avi)
    (setq avi "clock.avi")
    (cond
      ;; Supportpfade nach der Datei durchsuchen
       ( (if (setq fn (findfile avi))
           (dcl_Animate_Load Animation_DclForm1_Animation1 fn)
       ))
      ;; Datei aus dem Ordner der Beispieldateien des Installationsverzeichnisses von OpenDCL-Studio laden
       ( (if
           (or
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit Position
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit Position
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit Position
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit Position
           )
           (dcl_Animate_Load Animation_DclForm1_Animation1 (strcat fn avi))
       ))
      ;; Kann's nicht finden
       (T (alert (strcat "Ich kann die Videodatei \"" avi "\", fügen Sie den Pfad zu den AutoCAD-Supportpfaden hinzu!"))
         (EXIT)
       )
    )
)

(defun c:Animation_DclForm1_cmdRestart_OnClicked ( /)
    (dcl_Animate_Play Animation_DclForm1_Animation1)
)


(defun c:Animation_DclForm1_cmdStop_OnClicked ( /)
    (dcl_Animate_Stop Animation_DclForm1_Animation1)
)


(defun c:Animation_DclForm1_cboFrame_OnSelChanged (nSelection sSelText /)
    (if (zerop (dcl_OptionList_GetCurSel Animation_DclForm1_optSeekType))
        (dcl_Animate_Seek Animation_DclForm1_Animation1 nSelection)
        (dcl_Animate_Play Animation_DclForm1_Animation1 nSelection)
    )
)

(defun c:Animation_DclForm1_cmdClose_OnClicked ( /)
     (dcl_Form_Close Animation_DclForm1)
)



(princ)
;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
