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
	(dcl-Project-Load (*ODCL:Samples:FindFile "HTML.odcl"))

	;; Show the main form
	(dcl-Form-Show HTML/Dcl1)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:HTML/Dcl1#OnInitialize (/)               ;navigate to new url on HTML control
	(dcl-Html-Navigate HTML/Dcl1/Html "http://www.opendcl.com")
)

;;Navigate to entred URL
(defun c:HTML/Dcl1/TB_Go#OnClicked (/ url)
	;;get url from textBox
	(Setq url (dcl-Control-GetText HTML/Dcl1/TB_URL))
	;;navigate to new url on HTML control
	(dcl-Html-Navigate HTML/Dcl1/Html url)
)

(defun c:HTML/Dcl1/TB_GetDocument#OnClicked (/ htmlCode)
	;;Get HTML code of document
	(Setq htmlCode (dcl-Html-GetHtmlDocument HTML/Dcl1/Html))
	(princ htmlCode)
	(alert "HTML code has been printed in command bar")
)

(defun c:HTML/Dcl1/TB-GetOffline#OnClicked (/ status info)
	;;Check browser status
	(Setq status (dcl-Html-GetOffline HTML/Dcl1/Html)
		  info   (if status
					 "Browser OFFLINE"
					 "Browser ONLINE"
				 )                                ; T - offline, NIL - online
	)
	(dcl-MessageBox info "Browser status" 0 2)
)

(defun c:HTML/Dcl1/TB-SetEnables#OnClicked ()
	;;Set html control enabled
	(dcl-Control-SetEnabled HTML/Dcl1/Html T)
)

(defun c:HTML/Dcl1/TB-SetDisabled#OnClicked ()
	;;Set html control disabled
	(dcl-Control-SetEnabled HTML/Dcl1/Html nil)
)

(defun c:HTML/Dcl1/TB-GetFullName#OnClicked ()    ;Get Full Name
	(dcl-MessageBox (dcl-Html-GetFullName HTML/Dcl1/Html) "Full Name" 0 2)
)

(defun c:HTML/Dcl1/TB-LoadingStatus#OnClicked (/ status info)
	;;get loading status
	(Setq status (dcl-Html-GetBusy HTML/Dcl1/Html)
		  info   (if status
					 "Page is loading"
					 "-----------"
				 )
	)
	(dcl-MessageBox info "Info" 0 2)
)

(defun c:HTML/Dcl1/TB-PageTitle#OnClicked ()
	;; get title of website
	(dcl-MessageBox (dcl-Html-GetLocationName HTML/Dcl1/Html) "Page title" 0 2)
)

(defun c:HTML/Dcl1/RB-GoBack#OnClicked ()
	;; go back to previous website
	(dcl-Html-GoBack HTML/Dcl1/Html)
)

(defun c:HTML/Dcl1/TB-Home#OnClicked ()
	;; navigate to home website
	(dcl-Html-GoHome HTML/Dcl1/Html)
)

(defun c:HTML/Dcl1/TB-GetUrl#OnClicked ()
	;; get url of current loaded website
	(dcl-MessageBox (dcl-Html-GetLocationURL HTML/Dcl1/Html) "URL" 0 2)
)

(defun c:HTML/Dcl1/TB-UpdateHtml#OnClicked ()
	;; update html code
	(dcl-Html-UpdateHtmlCode HTML/Dcl1/Html "<b>New code....</b>")
)

;; triger when website is loaded
(defun c:HTML/Dcl1/Html#OnNavigationComplete (sUrl)
	(princ (strcat "\nNavigation completed, URL: " sUrl))
)

(defun c:HTML/Dcl1/TB_Close#OnClicked ()
	(dcl-Form-Close HTML/Dcl1)                    ; close the form now
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

;; If master demo is active, run the main function immediately; otherwise
;; display a banner. The extra gymnastics allow the sample name to be
;; specified in only one place, thus making it easier to reuse this code.
(	(lambda (demoname)
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
