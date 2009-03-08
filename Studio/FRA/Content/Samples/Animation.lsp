(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL sample programs.\nEnter \"AVI\" to run the sample.\n")
)


(defun c:avi ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Animation.odcl")
  (dcl_FORM_SHOW Animation_DclForm1)
    
  ;; The Event handlers manage the form here.
  (PRINC)
)




(defun c:DclForm1_OnInitialize ( / fn avi)
    (setq avi "clock.avi")
    (cond
      ;; Search the support paths for the file
       ( (if (setq fn (findfile avi))
           (dcl_Animate_Load Animation_DclForm1_Animation1 fn)
       ))
      ;; Load the file from the default installed "Samples" folder.
       ( (if
           (or
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
           )
           (dcl_Animate_Load Animation_DclForm1_Animation1 (strcat fn avi))
       ))
      ;; Can't find it..
       (T (alert (strcat "Cant find \"" avi "\", you may need to add it to an Acad support path!"))
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
