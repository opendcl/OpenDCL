;;;######################################################################
;;;
;;;OpenDCL Sample: AllSamples
;;;
;;; This sample provides an interface for running the individual OpenDCL samples.
;;;


;|*Global Constants*|;

;; List of samples as (tab-name sample-name lisp-filename description)
(setq *OdclAllSamples '(
  ("Basic" "Misc." "Misc.lsp" "Block View, Slide files, Combo Box styles, misc. controls")
  ("Basic" "Methods" "Methods.lsp" "Picture Box, Slide controls, manipulating controls")
  ("Basic" "Events" "EventHandling.lsp" "Event handling")
  ("Basic" "Message box" "MessageBox.lsp" "Using the message box")
  ("Basic" "View DWG" "ViewDWG.lsp" "Working with the DWG Preview control")
  ("Basic" "Modeless" "Modeless.lsp" "Working with Modeless forms")
  ("Basic" "Palette" "AllControls.lsp" "Palette showing samples of all controls")
  ("Basic" "HTML" "HTML.lsp" "Web browser control")
  ("Basic" "Animation" "Animation.lsp" "Play an AVI file with the Animation control")
  ("Basic" "Hatch" "Hatches.lsp" "Display hatch patterns")
  ("Basic" "Tooltip" "ToolTip.lsp" "Working with control tool tips")
  ("Basic" "Drag & Drop" "DragNDrop.lsp" "Drag and drop from one control to another")
  ("Basic" "ListView" "ListView.lsp" "Using the ListView control")
  ("Basic" "Copy/Paste" "ListBoxCopyPaste.lsp" "Copy/move text between controls")
  ("Basic" "Splitter" "Splitter.lsp" "Working with Splitter controls")
  ("Basic" "DWG List" "DwgList.lsp" "Working with the Dwg List control")
  ("Basic" "List Box" "ListBox.lsp" "Working with a List Box & file selection")
  ("Advanced" "Tree" "Tree.lsp" "Working with the Tree control")
  ("Advanced" "Grid" "Grid.lsp" "Working with the Grid control")
  ("Advanced" "Selection" "Selections.lsp" "Hiding a Modal form to select points or entities")
  ("Advanced" "Slider" "Slider.lsp" "Custom slider control from picture box")
  ("Advanced" "Options Tab" "OptionsTab.lsp" "Adding a custom Options tab")
  ("Advanced" "Splash" "Splash.lsp" "Using OnTimer to display a splash screen")
  ("Advanced" "Mover" "FormMover.lsp" "Moving and resizing a form after it's been shown")
  ("Advanced" "Combo Box Lab" "ComboBoxLab.lsp" "Compare combo box styles and behavior")
  ("Advanced" "VLX 1" "DistSample1.vlx" "ODCL embedded as a resource")
  ("Advanced" "VLX 2" "DistSample2.vlx" "ODCL as an inline stream")
))

;; Main program
(DEFUN C:OPENDCLDEMO (/ *error*)
  (defun *error* (msg)
    (while (< 0 (getvar "cmdactive")) (command))
    ;; do error stuff
    (if (dcl-Form-IsActive AllSamples/Main)
      (dcl-Form-Close AllSamples/Main)
    )
    (princ
      (strcat "\nApplication Error: " (itoa (getvar "errno")) " :- " msg)
    )
    (princ)
  )
  ;;------------------------

  ;; Ensure OpenDCL Runtime is loaded (without echoing to command line)
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Load the project
  (dcl-Project-Load (*ODCL:Samples-FindFile "@AllSamples.odcl") NIL "AllSamples")

  ;; Show the main form
  (dcl-Form-Show AllSamples/Main)
  ;; The Event handlers manage the form here.
  (princ)
)

;; Load the specified sample
(DEFUN *ODCL:RunSample (filename)
  (setq *ODCL:AllSamples T) ; flag the sample to run on load
  (if (eq "Fail" (load (*ODCL:Samples-FindFile filename) "Fail"))
    ((alert (strcat "\"" filename "\" failed to load!")))
  )
  (setq *ODCL:AllSamples nil)
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
            (vlax-invoke-method http "SetRequestHeader" "User-Agent" "OpenDCL AllSamples")
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
          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
          "Language"
        )
      ) ;_ 64-bit location
      ( "ENU"
      )
    )
  )
  (defun append_status (status) ; append to dialog box status listbox
    (dcl-ListBox-AddString AllSamples/Update/lbxStatus status)
    (dcl-Control-Redraw AllSamples/Update)
  )

  (dcl-Form-Show AllSamples/Update)
  (princ)
)


;|*OpenDCL Event Handlers*|;

(DEFUN c:AllSamples/Update#OnInitialize (/ curlang myver myver_string curver curver_string)
  (dcl-ListBox-Clear AllSamples/Update/lbxStatus)
  (dcl-Control-SetVisible AllSamples/Update/btnUpdate nil)

  (append_status "Checking for an update. Please wait...")

  ;; Display installed OpenDCL version
  (setq curlang (get_curlang))
  (setq myver (parse_version (setq myver_string (dcl-GETVERSIONEX))))
  (dcl-Control-SetCaption AllSamples/Update/lblLanguage curlang)
  (dcl-Control-SetCaption AllSamples/Update/lblVersion myver_string)

  ;; Display latest available OpenDCL version
  (setq curver (parse_version (setq curver_string (get_curver nil))))
  (if (<version curver myver)
    (progn
      (setq curver_string (get_curver T)) ; now getting dev build version
      (setq curver (parse_version curver_string))
      (dcl-Control-SetVisible AllSamples/Update/lblDevBuild T)
      (dcl-Control-SetVisible AllSamples/Update/lblDevBuildAvail T)
    )
    (progn
      (dcl-Control-SetVisible AllSamples/Update/lblDevBuild nil)
      (dcl-Control-SetVisible AllSamples/Update/lblDevBuildAvail nil)
    )
  )
  (dcl-Control-SetCaption AllSamples/Update/lblLanguageAvail curlang)
  (dcl-Control-SetCaption AllSamples/Update/lblVersionAvail curver_string)

  ;; Decide whether installed version can be updated
  (dcl-Control-SetVisible AllSamples/Update/btnClose T)
  (if (<version myver curver)
    (progn
      (append_status "A newer version of OpenDCL Studio is available!")
      (dcl-Control-SetEnabled AllSamples/Update/btnUpdate T)
      (dcl-Control-SetVisible AllSamples/Update/btnUpdate T)
    )
    (append_status "OpenDCL Studio is up to date!")
  )
)

(DEFUN c:AllSamples/Update/btnClose#OnClicked (/)
  (dcl-Form-Close AllSamples/Update 2)
)

(DEFUN c:AllSamples/Update/btnUpdate#OnClicked (/ msipath curlang)
  (dcl-Control-SetEnabled AllSamples/Update/btnUpdate nil)
  (dcl-Control-SetVisible AllSamples/Update/btnClose nil)
  (setq msipath
    (write_msi
      (setq curlang (dcl-Control-GetCaption AllSamples/Update/lblLanguageAvail))
      (dcl-Control-GetVisible AllSamples/Update/lblDevBuildAvail)
      (strcat
        "OpenDCL.Studio."
        (strcase curlang)
        "."
        (dcl-Control-GetCaption AllSamples/Update/lblVersionAvail)
        ".msi"
      )
    )
  )
  (if msipath
    (progn
      (dcl-SendString "(*ODCL:Vanish)\n")
      (startapp (strcat "msiexec.exe /i " msipath))
      (dcl-Form-Close AllSamples/Update 1)
    )
    (dcl-Control-SetVisible AllSamples/Update/btnClose T)
  )
)
(defun c:AllSamples/Main#OnInitialize (/)
  (dcl-Grid-Clear AllSamples/Main/Basic)
  (foreach sample *OdclAllSamples
    (dcl-Grid-AddRow "AllSamples" "Main" (nth 0 sample) (nth 1 sample) (nth 2 sample) (nth 3 sample))
  )
  (c:AllSamples/Main#OnDocActivated)
  (princ)
)

(defun c:AllSamples/Main#OnSize (NewWidth NewHeight /)
  (dcl-Grid-SetColumnWidth AllSamples/Main/Basic 2 (- (dcl-Control-GetWidth AllSamples/Main/Basic) (dcl-Grid-GetColumnWidth AllSamples/Main/Basic 0) 21))
  (dcl-Grid-SetColumnWidth AllSamples/Main/Advanced 2 (- (dcl-Control-GetWidth AllSamples/Main/Advanced) (dcl-Grid-GetColumnWidth AllSamples/Main/Advanced 0) 21))
  (princ)
)

(defun c:AllSamples/Main#OnEnteringNoDocState (/)
  (dcl-Control-SetEnabled AllSamples/Main/Basic NIL)
  (dcl-Control-SetEnabled AllSamples/Main/Advanced NIL)
  (dcl-Control-SetEnabled AllSamples/Main/btnUpdateCheck NIL)
  (princ)
)

(defun c:AllSamples/Main#OnDocActivated (/)
  (dcl-Control-SetEnabled AllSamples/Main/Basic T)
  (dcl-Control-SetEnabled AllSamples/Main/Advanced T)
  (dcl-Control-SetEnabled AllSamples/Main/btnUpdateCheck T)
  (princ)
)

(defun c:AllSamples/Main/Basic#OnDblClicked (Row Column /)
  (*ODCL:RunSample (dcl-Grid-GetCellText AllSamples/Main/Basic Row 1))
  (princ)
)

(defun c:AllSamples/Main/Advanced#OnDblClicked (Row Column /)
  (*ODCL:RunSample (dcl-Grid-GetCellText AllSamples/Main/Advanced Row 1))
  (princ)
)

(defun c:AllSamples/Main/Basic#OnSelChanged (Row Column /)
  (dcl-Grid-SetCurCell AllSamples/Main/Basic Row -1)
  (princ)
)

(defun c:AllSamples/Main/Advanced#OnSelChanged (Row Column /)
  (dcl-Grid-SetCurCell AllSamples/Main/Advanced Row -1)
  (princ)
)

(defun c:AllSamples/Main/Basic#OnKeyDown (Character RepeatCount Flags / row)
  (if (and (or (= Character " ") (= Character "\r")) (>= (setq row (car (dcl-Grid-GetCurCell AllSamples/Main/Basic))) 0))
    (*ODCL:RunSample (dcl-Grid-GetCellText AllSamples/Main/Basic row 1))
  )
  (princ)
)

(defun c:AllSamples/Main/Advanced#OnKeyDown (Character RepeatCount Flags / row)
  (if (and (or (= Character " ") (= Character "\r")) (>= (setq row (car (dcl-Grid-GetCurCell AllSamples/Main/Advanced))) 0))
    (*ODCL:RunSample (dcl-Grid-GetCellText AllSamples/Main/Advanced row 1))
  )
  (princ)
)

(defun c:AllSamples/Main/btnReadme#OnClicked (/ readme)
  (setq readme (*ODCL:Samples-FindFile "@ReadME.txt"))
  (if readme
    (startapp "notepad" readme)
    (alert (strcat "Cant find \"@ReadME.txt\"!"))
  )
  (princ)
)

(defun c:AllSamples/Main/btnUpdateCheck#OnClicked ()
  (dcl-SendString "(*ODCL:UpdateCheck)\n")
  (princ)
)

(princ)

;;;######################################################################
;;;######################################################################
;;; The following section of code is designed to locate OpenDCL Studio
;;; sample files in the samples folder by prefixing the filename with
;;; the path prefix that was saved in the registry by the installer.
;;; The global *ODCL:Prefix and function *ODCL:Samples-FindFile
;;; are used throughout the samples.
;;;
(or *ODCL:Samples-FindFile
  (defun *ODCL:Samples-FindFile (file)
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
(C:OPENDCLDEMO)

;;;######################################################################
;;;######################################################################
