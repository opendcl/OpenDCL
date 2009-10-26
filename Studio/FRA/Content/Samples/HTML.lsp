;;;
;;; HTML Sample
;;;
;;; This sample demonstrates the HTML control and all its events.
;;;

;; Main program
(defun c:HTML (/ cmdecho)

    ;; Ensure OpenDCL Runtime is (quietly) loaded
    (setq cmdecho (getvar "CMDECHO"))
    (setvar "CMDECHO" 0)
    (command "_OPENDCL")
    (setvar "CMDECHO" cmdecho)

    ;; Load the project
    (dcl_Project_Load (*ODCL:Samples:FindFile "HTML.odcl"))

    ;; Show the main form
    (dcl_Form_Show HTML_Dcl1)

    ;; This is a modal form, so (dcl_Form_Show) does not return until
    ;; the modal form is closed. In the meantime, the event handlers
    ;; manage the form.

    (princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:HTML_Dcl1_OnInitialize (/)               ;navigate to new url on HTML control
    (dcl_Html_Navigate HTML_Dcl1_Html "http://www.opendcl.com")
)

;;Navigate to entred URL
(defun c:HTML_Dcl1_TB_Go_OnClicked (/ url)
    ;;get url from textBox
    (Setq url (dcl_Control_GetText HTML_Dcl1_TB_URL))
    ;;navigate to new url on HTML control
    (dcl_Html_Navigate HTML_Dcl1_Html url)
)

(defun c:HTML_Dcl1_TB_GetDocument_OnClicked (/ htmlCode)
    ;;Get HTML code of document
    (Setq htmlCode (dcl_Html_GetHtmlDocument HTML_Dcl1_Html))
    (princ htmlCode)
    (alert "HTML code has been printed in command bar")
)

(defun c:HTML_Dcl1_TB-GetOffline_OnClicked (/ status info)
    ;;Check browser status
    (Setq status (dcl_Html_GetOffline HTML_Dcl1_Html)
          info   (if status
                     "Browser OFFLINE"
                     "Browser ONLINE"
                 )                                ; T - offline, NIL - online
    )
    (dcl_MessageBox info "Browser status" 0 2)
)

(defun c:HTML_Dcl1_TB-SetEnables_OnClicked ()
    ;;Set html control enabled
    (dcl_Control_SetEnabled HTML_Dcl1_Html T)
)

(defun c:HTML_Dcl1_TB-SetDisabled_OnClicked ()
    ;;Set html control disabled
    (dcl_Control_SetEnabled HTML_Dcl1_Html nil)
)

(defun c:HTML_Dcl1_TB-GetFullName_OnClicked ()    ;Get Full Name
    (dcl_MessageBox (dcl_Html_GetFullName HTML_Dcl1_Html) "Full Name" 0 2)
)

(defun c:HTML_Dcl1_TB-LoadingStatus_OnClicked (/ status info)
    ;;get loading status
    (Setq status (dcl_Html_GetBusy HTML_Dcl1_Html)
          info   (if status
                     "Page is loading"
                     "-----------"
                 )
    )
    (dcl_MessageBox info "Info" 0 2)
)

(defun c:HTML_Dcl1_TB-PageTitle_OnClicked ()
    ;; get title of website
    (dcl_MessageBox (dcl_Html_GetLocationName HTML_Dcl1_Html) "Page title" 0 2)
)

(defun c:HTML_Dcl1_RB-GoBack_OnClicked ()
    ;; go back to previous website
    (dcl_Html_GoBack HTML_Dcl1_Html)
)

(defun c:HTML_Dcl1_TB-Home_OnClicked ()
    ;; navigate to home website
    (dcl_Html_GoHome HTML_Dcl1_Html)
)

(defun c:HTML_Dcl1_TB-GetUrl_OnClicked ()
    ;; get url of current loaded website
    (dcl_MessageBox (dcl_Html_GetLocationURL HTML_Dcl1_Html) "URL" 0 2)
)

(defun c:HTML_Dcl1_TB-UpdateHtml_OnClicked ()
    ;; update html code
    (dcl_Html_UpdateHtmlCode HTML_Dcl1_Html "<b>New code....</b>")
)

;; triger when website is loaded
(defun c:HTML_Dcl1_Html_OnNavigationComplete (sUrl)
    (princ (strcat "\nNavigation completed, URL: " sUrl))
)

(defun c:HTML_Dcl1_TB_Close_OnClicked ()
    (dcl_Form_Close HTML_Dcl1)                    ; close the form now
)

(princ)

;|<<OpenDCL Samples Epilog>>|;

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
                 (   *ODCL:Prefix
                 ) ;_ already defined
                 (   (vl-registry-read
                         "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 32-bit location
                 (   (vl-registry-read
                         "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 32-bit location
                 (   (vl-registry-read
                         "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
                         "SamplesFolder"
                     )
                 ) ;_ 64-bit location
                 (   (vl-registry-read
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

;; If master demo is active, run the main function immediately; otherwise
;; display a banner. The extra gymnastics allow the sample name to be
;; specified in only one place, thus making it easier to reuse this code.
(   (lambda (demoname)
        (if *ODCL:MasterDemo
            (progn
                (princ (strcat "'" demoname "\n"))
                (apply (read (strcat "C:" demoname)) nil)
            )
            (progn
                (princ (strcat "\n" demoname " OpenDCL sample loaded"))
                (princ (strcat " (Enter " (strcase demoname) " command to run)\n"))
            )
        )
    )
    "HTML"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
