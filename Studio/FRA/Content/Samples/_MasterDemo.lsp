;;;######################################################################
;;;
;;;OpenDCL Sample: MasterDemo
;;;
;;; This sample provides an interface for running the individual OpenDCL samples.
;;;

;; Main program
(DEFUN c:OpenDCLDemo (/ *error*)
  (DEFUN *error* (msg)
    (WHILE (< 0 (GETVAR "cmdactive")) (COMMAND))
    ;; do error stuff
    (IF (DCL_FORM_ISACTIVE _MasterDemo_Main)
      (DCL_FORM_CLOSE _MasterDemo_Main)
    )
    (PRINC
      (STRCAT "\nApplication Error: " (ITOA (GETVAR "errno")) " :- " msg)
    )
    (PRINC)
  )
  ;;------------------------

  ;; Ensure OpenDCL Runtime is loaded (without echoing to command line)
  (SETQ cmdecho (GETVAR "CMDECHO"))
  (SETVAR "CMDECHO" 0)
  (COMMAND "_OPENDCL")
  (SETVAR "CMDECHO" cmdecho)

  ;; Load the project
  (DCL_PROJECT_LOAD (*ODCL:Samples:FindFile "_MasterDemo.odcl"))

  ;; Show the main form
  (DCL_FORM_SHOW _MasterDemo_Main)
  ;; The Event handlers manage the form here.
  (PRINC)
)

;; Load the specified sample
(DEFUN *ODCL:RunSample (filename)
  (SETQ *ODCL:MasterDemo T) ; flag the sample to run on load
  (IF (NOT (LOAD (*ODCL:Samples:FindFile filename) NIL))
    ((ALERT (STRCAT "\"" filename "\" failed to load!")))
  )
  (SETQ *ODCL:MasterDemo nil)
)

;; Completely unload OpenDCL to allow upgrade installation
(DEFUN *ODCL:Vanish ()
  (foreach project (dcl_GetProjects) (dcl_Project_Unload project T))
  (foreach module (arx)
    (if (wcmatch module "opendcl`.*") (arxunload module))
  )
  (princ)
)

;; Check for newer version; if found, prompt to download and install
(DEFUN *ODCL:UpdateCheck (/ create_http request_http download_msi write_msi
                            parse_version <version get_curver get_curlang
                            append_status)
  (vl-load-com)
  (defun create_http () ; create and return WinHttpRequest object
    (cond
      ((vlax-create-object "WinHttp.WinHttpRequest.5.1"))
      ((vlax-create-object "WinHttp.WinHttpRequest.5"))
    )
  )
  (defun request_http (url return / http disp pos location filename) ; perform HTTP request
    (setq http (create_http))
    (setq result
      (vl-catch-all-apply
        (function
          (lambda ()
            (vlax-invoke-method http "Open" "GET" url :vlax-false)
            (vlax-invoke-method http "SetRequestHeader" "User-Agent" "OpenDCL MasterDemo")
            (vlax-invoke-method http "Send")
            (vlax-get-property http return)
          )
        )
      )
    )
    (vlax-release-object http)
    (if (vl-catch-all-error-p result)
      (progn (append_status (strcat "  ERROR: " (vl-catch-all-error-message result))) (setq result nil))
    )
    result
  )
  (defun download_msi (lang dev) ; download MSI via HTTP, return responsebody
    (append_status "Downloading OpenDCL Studio installation file...")
    ;; Construct file download URL and download file
    ;; Note: to download runtime MSI, use http://opendcl.com/go?runtime and ignore language
    (setq url "http://opendcl.com/go?studio")
    (if lang (setq url (strcat url "&" (strcase lang T))))
    (if dev (setq url (strcat url "&dev")))
    (request_http url "ResponseBody")
    (if result (append_status "  Downloaded successfully!"))
    result
  )
  (defun write_msi (lang dev filename / result fso) ; write downloaded MSI to temp folder, return file path
    (if (setq result (download_msi lang dev))
      (progn
        (setq fso (vlax-create-object "Scripting.FileSystemObject"))
        (setq result
          (vl-catch-all-apply
            (function
              (lambda (/ tempfolder filepath adostream filestream)
                (append_status "Writing installation file to disk...")
                (setq tempfolder (vlax-invoke-method fso "GetSpecialFolder" 2))
                (setq filepath (strcat (vlax-get-property tempfolder "Path") "\\" filename))
                (vlax-release-object tempfolder)
                (cond
                  ( (setq adostream (vlax-create-object "ADODB.Stream"))
                    (setq result
                      (vl-catch-all-apply
                        (function
                          (lambda ()
                            (if (= :vlax-true (vlax-invoke-method fso "FileExists" filepath))
                              (vlax-invoke-method fso "DeleteFile" filepath :vlax-true)
                            )
                            (vlax-put adostream "Type" 1) ;1 = binary
                            (vlax-invoke adostream "Open")
                            (vlax-invoke-method adostream "Write" result)
                            (vlax-put adostream "Position" 0)
                            (vlax-invoke adostream "SaveToFile" filepath)
                            (vlax-invoke-method adostream "Close")
                            filepath
                          )
                        )
                      )
                    )
                    (vlax-release-object adostream)
                  )
                  ( (setq filestream
                      (vlax-invoke-method fso "CreateTextFile" filepath :vlax-true :vlax-false)
                    )
                    (setq result
                      (vl-catch-all-apply
                        (function
                          (lambda ()
                            (foreach element (vlax-safearray->list (vlax-variant-value result))
                              (vlax-invoke filestream "Write" (chr (+ 256 (logand 255 element))))
                            )
                            (vlax-invoke-method filestream "Close")
                            filepath
                          )
                        )
                      )
                    )
                    (vlax-release-object filestream)
                  )
                )
                (if (vl-catch-all-error-p result)
                  (progn (append_status (strcat "  ERROR: " (vl-catch-all-error-message result))) (setq filepath nil))
                  (append_status "  File written successfully!")
                )
                filepath
              )
            )
          )
        )
        (if fso (vlax-release-object fso))
        (if (vl-catch-all-error-p result)
          (progn (append_status (strcat "  ERROR: " (vl-catch-all-error-message result))) (setq result nil))
        )
        result
      )
    )
  )
  (defun parse_version (version / token) ; helper to convert version string to list
    (if (setq token (vl-string-position 46 version))
      (cons (atoi (substr version 1 token)) (parse_version (substr version (+ 2 token))))
      (list (atoi version))
    )
  )
  (defun <version (left right) ; compare version lists
    (cond
      ( (null left)
        (not (null right))
      )
      ( (< (car left) (car right)))
      ( (= (car left) (car right))
        (<version (cdr left) (cdr right))
      )
    )
  )
  (defun get_curver (dev / url) ; get current build version from opendcl.com
    (setq url
      (if dev
        "http://opendcl.com/version/version_dev.txt"
        "http://opendcl.com/version/version.txt"
      )
    )
    (request_http url "ResponseText")
  )
  (defun get_curlang () ; get current installed language
    (cond
      ( (vl-registry-read
          "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
          "Language"
        )
      ) ;_ 32-bit location
      ( (vl-registry-read
          "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
          "Language"
          )
      ) ;_ 32-bit location
      ( (vl-registry-read
          "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
          "Language"
        )
      ) ;_ 64-bit location
      ( (vl-registry-read
          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
          "Language"
        )
      ) ;_ 64-bit location
      ( "ENU"
      )
    )
  )
  (defun append_status (status) ; append to dialog box status listbox
    (dcl_ListBox_AddString _MasterDemo_Update_lbxStatus status)
    (dcl_Control_Redraw _MasterDemo_Update)
  )

  (dcl_Form_Show _MasterDemo_Update)
  (princ)
)


;|<<OpenDCL Event Handlers>>|;

(DEFUN c:_MasterDemo_Update_OnInitialize (/ curlang myver myver_string curver curver_string)
  (dcl_ListBox_Clear _MasterDemo_Update_lbxStatus)
  (dcl_Control_SetVisible _MasterDemo_Update_btnUpdate nil)

  (append_status "Checking for an update. Please wait...")

  ;; Display installed OpenDCL version
  (setq curlang (get_curlang))
  (setq myver (parse_version (setq myver_string (DCL_GETVERSIONEX))))
  (dcl_Control_SetCaption _MasterDemo_Update_lblLanguage curlang)
  (dcl_Control_SetCaption _MasterDemo_Update_lblVersion myver_string)

  ;; Display latest available OpenDCL version
  (setq curver (parse_version (setq curver_string (get_curver nil))))
  (if (<version curver myver)
    (progn
      (setq curver_string (get_curver T)) ; now getting dev build version
      (setq curver (parse_version curver_string))
      (dcl_Control_SetVisible _MasterDemo_Update_lblDevBuild T)
      (dcl_Control_SetVisible _MasterDemo_Update_lblDevBuildAvail T)
    )
    (progn
      (dcl_Control_SetVisible _MasterDemo_Update_lblDevBuild nil)
      (dcl_Control_SetVisible _MasterDemo_Update_lblDevBuildAvail nil)
    )
  )
  (dcl_Control_SetCaption _MasterDemo_Update_lblLanguageAvail curlang)
  (dcl_Control_SetCaption _MasterDemo_Update_lblVersionAvail curver_string)

  ;; Decide whether installed version can be updated
  (dcl_Control_SetVisible _MasterDemo_Update_btnClose T)
  (if (<version myver curver)
    (progn
      (append_status "A newer version of OpenDCL Studio is available!")
      (dcl_Control_SetEnabled _MasterDemo_Update_btnUpdate T)
      (dcl_Control_SetVisible _MasterDemo_Update_btnUpdate T)
    )
    (append_status "OpenDCL Studio is up to date!")
  )
)

(DEFUN c:_MasterDemo_Update_btnClose_OnClicked (/)
  (dcl_Form_Close _MasterDemo_Update 2)
)

(DEFUN c:_MasterDemo_Update_btnUpdate_OnClicked (/ msipath curlang)
  (dcl_Control_SetEnabled _MasterDemo_Update_btnUpdate nil)
  (dcl_Control_SetVisible _MasterDemo_Update_btnClose nil)
  (setq msipath
    (write_msi
      (setq curlang (dcl_Control_GetCaption _MasterDemo_Update_lblLanguageAvail))
      (dcl_Control_GetVisible _MasterDemo_Update_lblDevBuildAvail)
      (strcat
        "OpenDCL.Studio."
        (strcase curlang)
        "."
        (dcl_Control_GetCaption _MasterDemo_Update_lblVersionAvail)
        ".msi"
      )
    )
  )
  (dcl_SendString "(*ODCL:Vanish)\n")
  (startapp (strcat "msiexec.exe /i " msipath))
  (dcl_Form_Close _MasterDemo_Update 1)
)

(DEFUN c:_MasterDemo_Main_btnMisc_OnClicked ()
  (*ODCL:RunSample "Misc.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnMethods_OnClicked ()
  (*ODCL:RunSample "Methods.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnEventHandling_OnClicked ()
  (*ODCL:RunSample "EventHandling.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnMessageBox_OnClicked ()
  (*ODCL:RunSample "MessageBox.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnTree_OnClicked ()
  (*ODCL:RunSample "Tree.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnViewDwg_OnClicked ()
  (*ODCL:RunSample "ViewDwg.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnModeless_OnClicked ()
  (*ODCL:RunSample "Modeless.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnAllControls_OnClicked ()
  (*ODCL:RunSample "AllControls.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnHTML_OnClicked ()
  (*ODCL:RunSample "HTML.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnGrid_OnClicked ()
  (*ODCL:RunSample "GRID.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnAnimation_OnClicked ()
  (*ODCL:RunSample "Animation.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnHatches_OnClicked ()
  (*ODCL:RunSample "Hatches.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnToolTip_OnClicked ()
  (*ODCL:RunSample "ToolTip.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnDragNDrop_OnClicked ()
  (*ODCL:RunSample "DragNDrop.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnSelections_OnClicked ()
  (*ODCL:RunSample "Selections.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnListView_OnClicked ()
  (*ODCL:RunSample "ListView.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnListBoxCopyPaste_OnClicked ()
  (*ODCL:RunSample "ListBoxCopyPaste.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnSplitter_OnClicked ()
  (*ODCL:RunSample "Splitter.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnDwgList_OnClicked ()
  (*ODCL:RunSample "DwgList.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnListBox_OnClicked ()
  (*ODCL:RunSample "ListBox.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnDistSample_OnClicked (/ readme)
  (SETQ readme (*ODCL:Samples:FindFile "DistSampleReadMe.txt"))
  (IF readme
    (STARTAPP "notepad" readme)
    (ALERT (STRCAT "Cant find \"DistSampleReadMe.txt\"!"))
  )
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnOptionsTab_OnClicked ()
  (*ODCL:RunSample "OptionsTab.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnSplash_OnClicked (/)
  (*ODCL:RunSample "Splash.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnFormMover_OnClicked (/)
  (*ODCL:RunSample "FormMover.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnSlider_OnClicked (/)
  (*ODCL:RunSample "Slider.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnReadme_OnClicked (/ readme)
  (SETQ readme (*ODCL:Samples:FindFile "_ReadME.txt"))
  (IF readme
    (STARTAPP "notepad" readme)
    (ALERT (STRCAT "Cant find \"_ReadME.txt\"!"))
  )
  (PRINC)
)

(DEFUN c:_MasterDemo_Main_btnUpdateCheck_OnClicked ()
  (DCL_SENDSTRING "(*ODCL:UpdateCheck)\n")
  (PRINC)
)

(princ)

;;;######################################################################
;;;######################################################################
;;; The following section of code is designed to locate OpenDCL Studio
;;; sample files in the samples folder by prefixing the filename with
;;; the path prefix that was saved in the registry by the installer.
;;; The global *ODCL:Prefix and function *ODCL:Samples:FindFile
;;; are used throughout the samples.
;;;
(or *ODCL:Samples:FindFile
  (defun *ODCL:Samples:FindFile (file)
    (setq *ODCL:Prefix
      (cond
        (	*ODCL:Prefix
        ) ;_ already defined
        (	(vl-registry-read
            "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
            "SamplesFolder"
          )
        ) ;_ 32-bit location
        (	(vl-registry-read
            "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
            "SamplesFolder"
           )
        ) ;_ 32-bit location
        (	(vl-registry-read
            "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
            "SamplesFolder"
          )
        ) ;_ 64-bit location
        (	(vl-registry-read
            "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
            "SamplesFolder"
          )
        ) ;_ 64-bit location
      )
    )
    (cond
      ((findfile file)) ; check the support path first
      (*ODCL:Prefix (findfile (strcat *ODCL:Prefix file)))
      (file)
    )
  )
)

(princ "\nOPENDCLDEMO\n")
(C:OpenDCLDemo)

;;;######################################################################
;;;######################################################################

 ;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 2 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
