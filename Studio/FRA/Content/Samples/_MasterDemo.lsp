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
    (IF (dcl-Form-ISACTIVE _MasterDemo/Main)
      (dcl-Form-CLOSE _MasterDemo/Main)
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
  (dcl-Project-LOAD (*ODCL:Samples:FindFile "_MasterDemo.odcl"))

  ;; Show the main form
  (dcl-Form-SHOW _MasterDemo/Main)
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
  (foreach project (dcl-GetProjects) (dcl-Project-Unload project T))
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
  (defun request_http (url return / http disp pos location filename result) ; perform HTTP request
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
  (defun download_msi (lang dev / result) ; download MSI via HTTP, return responsebody
    (append_status "Downloading OpenDCL Studio installation file...")
    ;; Construct file download URL and download file
    ;; Note: to download runtime MSI, use http://opendcl.com/go?runtime and ignore language
    (setq url "http://opendcl.com/go?studio")
    (if lang (setq url (strcat url "&" (strcase lang T))))
    (if dev (setq url (strcat url "&dev")))
    (setq result (request_http url "ResponseBody"))
    (if result (append_status "  Downloaded successfully!"))

    (if (and (setq result (request_http url "ResponseBody"))
             (= (type result) 'VARIANT)
             (not (zerop (vlax-variant-type result))))
      (append_status "  Downloaded successfully!")
      (progn
        (append_status "  ERROR: The server's response did not contain any data!")
        (setq result nil)
      );
    );

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
    (dcl-ListBox-AddString _MasterDemo/Update/lbxStatus status)
    (dcl-Control-Redraw _MasterDemo/Update)
  )

  (dcl-Form-Show _MasterDemo/Update)
  (princ)
)


;|<<OpenDCL Event Handlers>>|;

(DEFUN c:_MasterDemo/Update#OnInitialize (/ curlang myver myver_string curver curver_string)
  (dcl-ListBox-Clear _MasterDemo/Update/lbxStatus)
  (dcl-Control-SetVisible _MasterDemo/Update/btnUpdate nil)

  (append_status "Checking for an update. Please wait...")

  ;; Display installed OpenDCL version
  (setq curlang (get_curlang))
  (setq myver (parse_version (setq myver_string (dcl-GETVERSIONEX))))
  (dcl-Control-SetCaption _MasterDemo/Update/lblLanguage curlang)
  (dcl-Control-SetCaption _MasterDemo/Update/lblVersion myver_string)

  ;; Display latest available OpenDCL version
  (setq curver (parse_version (setq curver_string (get_curver nil))))
  (if (<version curver myver)
    (progn
      (setq curver_string (get_curver T)) ; now getting dev build version
      (setq curver (parse_version curver_string))
      (dcl-Control-SetVisible _MasterDemo/Update/lblDevBuild T)
      (dcl-Control-SetVisible _MasterDemo/Update/lblDevBuildAvail T)
    )
    (progn
      (dcl-Control-SetVisible _MasterDemo/Update/lblDevBuild nil)
      (dcl-Control-SetVisible _MasterDemo/Update/lblDevBuildAvail nil)
    )
  )
  (dcl-Control-SetCaption _MasterDemo/Update/lblLanguageAvail curlang)
  (dcl-Control-SetCaption _MasterDemo/Update/lblVersionAvail curver_string)

  ;; Decide whether installed version can be updated
  (dcl-Control-SetVisible _MasterDemo/Update/btnClose T)
  (if (<version myver curver)
    (progn
      (append_status "A newer version of OpenDCL Studio is available!")
      (dcl-Control-SetEnabled _MasterDemo/Update/btnUpdate T)
      (dcl-Control-SetVisible _MasterDemo/Update/btnUpdate T)
    )
    (append_status "OpenDCL Studio is up to date!")
  )
)

(DEFUN c:_MasterDemo/Update/btnClose#OnClicked (/)
  (dcl-Form-Close _MasterDemo/Update 2)
)

(DEFUN c:_MasterDemo/Update/btnUpdate#OnClicked (/ msipath curlang)
  (dcl-Control-SetEnabled _MasterDemo/Update/btnUpdate nil)
  (dcl-Control-SetVisible _MasterDemo/Update/btnClose nil)
  (setq msipath
    (write_msi
      (setq curlang (dcl-Control-GetCaption _MasterDemo/Update/lblLanguageAvail))
      (dcl-Control-GetVisible _MasterDemo/Update/lblDevBuildAvail)
      (strcat
        "OpenDCL.Studio."
        (strcase curlang)
        "."
        (dcl-Control-GetCaption _MasterDemo/Update/lblVersionAvail)
        ".msi"
      )
    )
  )
  (if msipath
    (progn
      (dcl-SendString "(*ODCL:Vanish)\n")
      (startapp (strcat "msiexec.exe /i " msipath))
      (dcl-Form-Close _MasterDemo/Update 1)
    )
    (dcl-Control-SetVisible _MasterDemo/Update/btnClose T)
  )
)

(DEFUN c:_MasterDemo/Main/btnMisc#OnClicked ()
  (*ODCL:RunSample "Misc.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnMethods#OnClicked ()
  (*ODCL:RunSample "Methods.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnEventHandling#OnClicked ()
  (*ODCL:RunSample "EventHandling.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnMessageBox#OnClicked ()
  (*ODCL:RunSample "MessageBox.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnTree#OnClicked ()
  (*ODCL:RunSample "Tree.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnViewDwg#OnClicked ()
  (*ODCL:RunSample "ViewDwg.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnModeless#OnClicked ()
  (*ODCL:RunSample "Modeless.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnAllControls#OnClicked ()
  (*ODCL:RunSample "AllControls.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnHTML#OnClicked ()
  (*ODCL:RunSample "HTML.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnGrid#OnClicked ()
  (*ODCL:RunSample "GRID.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnAnimation#OnClicked ()
  (*ODCL:RunSample "Animation.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnHatches#OnClicked ()
  (*ODCL:RunSample "Hatches.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnToolTip#OnClicked ()
  (*ODCL:RunSample "ToolTip.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnDragNDrop#OnClicked ()
  (*ODCL:RunSample "DragNDrop.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnSelections#OnClicked ()
  (*ODCL:RunSample "Selections.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnListView#OnClicked ()
  (*ODCL:RunSample "ListView.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnListBoxCopyPaste#OnClicked ()
  (*ODCL:RunSample "ListBoxCopyPaste.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnSplitter#OnClicked ()
  (*ODCL:RunSample "Splitter.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnDwgList#OnClicked ()
  (*ODCL:RunSample "DwgList.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnListBox#OnClicked ()
  (*ODCL:RunSample "ListBox.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnDistSample#OnClicked (/ readme)
  (SETQ readme (*ODCL:Samples:FindFile "DistSampleReadMe.txt"))
  (IF readme
    (STARTAPP "notepad" readme)
    (ALERT (STRCAT "Cant find \"DistSampleReadMe.txt\"!"))
  )
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnOptionsTab#OnClicked ()
  (*ODCL:RunSample "OptionsTab.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnSplash#OnClicked (/)
  (*ODCL:RunSample "Splash.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnFormMover#OnClicked (/)
  (*ODCL:RunSample "FormMover.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnSlider#OnClicked (/)
  (*ODCL:RunSample "Slider.lsp")
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnReadme#OnClicked (/ readme)
  (SETQ readme (*ODCL:Samples:FindFile "_ReadME.txt"))
  (IF readme
    (STARTAPP "notepad" readme)
    (ALERT (STRCAT "Cant find \"_ReadME.txt\"!"))
  )
  (PRINC)
)

(DEFUN c:_MasterDemo/Main/btnUpdateCheck#OnClicked ()
  (dcl-SENDSTRING "(*ODCL:UpdateCheck)\n")
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
