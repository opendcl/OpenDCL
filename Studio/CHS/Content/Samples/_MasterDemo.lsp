(PRINC "\nOpenDCL sample programs.\nEnter \"DEMO\" to run the sample.\n")


(defun LspLoader (lspFileName / fn)
    (cond
      ;; Search the support paths for the .LSP file & load it.
       ( (if (setq fn (findfile lspFileName))
           (LOAD fn)
       ))
      ;; Load the .LSP file from the default installed "Samples" folder.
       ( (if
           (or
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
           )
           (LOAD (strcat fn lspFileName))
       ))
      ;; The lsp failed to load, so report or log the error exit now (or take corrective action and try again)
       (T (alert (strcat "\"" lspFileName "\" failed to load, you may need to add it to an Acad support path for it to load correctly!"))
         (EXIT)
       )
    )
)



(DEFUN c:Demo (/ project-name *error*)
    (DEFUN *error* (msg)
        (WHILE (< 0 (GETVAR "cmdactive")) (COMMAND))
        ;; do error stuff
        (IF _MasterDemo_DCLMaster (dcl_FORM_CLOSE _MasterDemo_DCLMaster))
        (PRINC (STRCAT "\nApplication Error: " (itoa (GETVAR "errno")) " :- " msg))        
        (PRINC)
    )

    (LspLoader "_OpendclUtils.LSP")
    (LoadRunTime)
    (LoadODCLProj "_MasterDemo.odcl")

    
    ;; The Dialog is based on a dockable Modeless form,
    ;; so test if it is already active .. otherwise show the form.  
    (IF (NOT (dcl_FORM_ISACTIVE _MasterDemo_DCLMaster))
          (dcl_FORM_SHOW _MasterDemo_DCLMaster) 
        ;; The Event handlers manage the form here.
        (PROMPT "\nForm is already active.")
    )
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd1_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:misc) (LspLoader "Misc.lsp"))
        (C:misc)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd2_OnClicked ()
    (setq *MasterDemo* T)   
    (IF (OR (VL-SYMBOL-VALUE 'C:Methods) (LspLoader "Methods.lsp"))
        (C:Methods)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd3_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:Events) (LspLoader "EventHandling.lsp"))
        (C:Events)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd4_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:MsgBox) (LspLoader "MessageBox.lsp"))
        (C:MsgBox)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd5_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:Tree) (LspLoader "TreeView.lsp"))
        (C:Tree)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd6_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:ViewDwg) (LspLoader "ViewDwg.lsp"))
        (C:ViewDwg)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd7_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:Modeless) (LspLoader "Modeless.lsp"))
        (C:Modeless)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd8_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:ConfigTab) (LspLoader "Config.lsp"))
        (C:ConfigTab)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd9_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:HTML) (LspLoader "HTML.lsp"))
        (C:HTML)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd10_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:GRD) (LspLoader "GRID.lsp"))
        (C:GRD)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd11_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'C:AVI) (LspLoader "Animation.lsp"))
        (C:AVI)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd12_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:Hatches) (LspLoader "Hatches.lsp"))
        (c:Hatches)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)


(DEFUN c:_MasterDemo_DCLMaster_cmd13_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:Tool) (LspLoader "ToolTip.lsp"))
        (c:Tool)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd14_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:Drag) (LspLoader "DragNDrop.lsp"))
        (c:Drag)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)


(DEFUN c:_MasterDemo_DCLMaster_cmd15_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:sel) (LspLoader "Selections.lsp"))
        (c:sel)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd16_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:ListView) (LspLoader "ListView.lsp"))
        (c:ListView)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(DEFUN c:_MasterDemo_DCLMaster_cmd17_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:CopyPaste) (LspLoader "ListBoxCopyPaste.lsp"))
        (c:CopyPaste)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(defun c:_MasterDemo_DCLMaster_cmd18_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:SPLIT) (LspLoader "Splitter.lsp"))
        (c:splitter)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)


(defun c:_MasterDemo_DCLMaster_cmd19_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:DwgList) (LspLoader "DwgList.lsp"))
        (PROGN (dcl_FORM_CLOSE _MasterDemo_DCLMaster) (c:DwgList))
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(defun c:_MasterDemo_DCLMaster_cmd20_OnClicked ()
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:ListBox) (LspLoader "ListBox.lsp"))
        (c:ListBox)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(defun c:_MasterDemo_DCLMaster_cmd21_OnClicked ( / txt fn)
    (setq txt "DistSampleReadMe.txt")
    (cond
      ;; Search the support paths for the file
       ( (if (setq fn (findfile txt))
           (startapp "notepad" fn)
       ))
      ;; Load the file from the default installed "Samples" folder.
       ( (if 
           (or
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
           )
           (startapp "notepad" (strcat fn txt))
       ))
      ;; Can't find it..
       (T (alert (strcat "Cant find \"" txt "\", you may need to add it to an Acad support path!"))
         (EXIT)
       )
    )
    (princ)
)

(defun c:_MasterDemo_DCLMaster_cmd22_OnClicked ()
   (LspLoader "ManualLoading.lsp")
   (princ)
)

(defun c:_MasterDemo_DCLMaster_cmd23_OnClicked ( /)
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:OpenTree) (LspLoader "TMaCAD.lsp"));;_ BR FIX THIS...
        (c:OpenTree)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)


(defun c:_MasterDemo_DCLMaster_cmd24_OnClicked ( /)
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:splash) (LspLoader "splash.lsp"))
        (c:splash)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)

(defun c:_MasterDemo_DCLMaster_cmd25_OnClicked ( /)
    (setq *MasterDemo* T)
    (IF (OR (VL-SYMBOL-VALUE 'c:Mover) (LspLoader "FormMover.lsp"))
        (c:Mover)
    )
    (setq *MasterDemo* nil)
    (PRINC)
)



(defun c:_MasterDemo_DCLMaster_cmdReadme_OnClicked ( / txt fn)
    (setq txt "_ReadME.txt")
    (cond
      ;; Search the support paths for the file
       ( (if (setq fn (findfile txt))
           (startapp "notepad" fn)
       ))
      ;; Load the file from the default installed "Samples" folder.
       ( (if 
           (or
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL" "SamplesFolder")) ;_ 32-bit location
             (setq fn (vl-registry-read "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL" "SamplesFolder")) ;_ 64-bit location
           )
           (startapp "notepad" (strcat fn txt))
       ))
      ;; Can't find it..
       (T (alert (strcat "Cant find \"" txt "\", you may need to add it to an Acad support path!"))
         (EXIT)
       )
    )
    (princ)
 )


(defun c:_MasterDemo_DCLMaster_txtCheckVar_OnClicked ( )
    (dcl_updatecheck)
    (princ)
)


(C:demo)
(PRINC)

 ;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 2 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
