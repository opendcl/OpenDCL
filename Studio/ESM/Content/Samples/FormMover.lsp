;;;
;;; Form Move Sample
;;;
;;; This sample demonstrates how to move or resize a form after it's been shown.
;;; It uses (dcl_DelayedInvoke) to wait a few milliseconds before moving the
;;; form in order give the movement an animated appearance.

;; Main program
(defun c:Mover (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl_Project_Load (*ODCL:Samples:FindFile "FormMover.odcl"))

	;; Show the main form
	(dcl_Form_Show FormMover_Form1)
	(dcl_Form_Resize FormMover_Form1 300 200)
	(dcl_Form_Center FormMover_Form1)
	(c:FormMover_Form1_GraphicButton1_OnClicked)

	;; This is a modeless form, so (dcl_Form_Show) returns immediately,
	;; leaving the event handlers to manage the form.

	(princ)
)

;;This function moves the form to the 4 corners of the screen, then to the center.
(defun C:FormMover ( / ce FormSize delay wd ht ScreenSize ScreenX ScreenY)
	(Setq FormSize (dcl_Form_GetRectangle FormMover_Form1))
	(setq wd (caddr FormSize))
	(setq ht (cadddr FormSize))
	(setq ScreenSize (dcl_GetScreenSize))
	(setq ScreenX (car ScreenSize))
	(setq ScreenY (cadr ScreenSize))

	(cond
		(	(= *FormPos* 0)
			(dcl_Form_SetPos FormMover_Form1 0 0);_TL
		)
		(	(= *FormPos* 1)
			(dcl_Form_SetPos FormMover_Form1 0 (- ScreenY ht));_ BL
		)
		(	(= *FormPos* 2)
			(dcl_Form_SetPos FormMover_Form1 (- ScreenX wd) (- ScreenY ht));_ BR
		)
		(	(= *FormPos* 3)
			(dcl_Form_SetPos FormMover_Form1 (- ScreenX wd) 0);_ TR
		)
		(	(= *FormPos* 4)
			(dcl_Form_Center FormMover_Form1)
		)
	);_ cond

	(setq *FormPos* (1+ *FormPos*));_ bump the form position count
	(if (<= *FormPos* 4)
		(dcl_DelayedInvoke 800 "C:FormMover");_ repeat
	)
	(princ)
)

;;This function moves & resizes the form to the bottom left corner of the screen or back to the center.
(defun C:FormMinMax ( / FormSize lf tp wd ht ScreenSize ScreenX ScreenY)
	(Setq FormSize (dcl_Form_GetRectangle FormMover_Form1))
	(setq lf (car FormSize))
	(setq tp (cadr FormSize))
	(setq wd (caddr FormSize))
	(setq ht (cadddr FormSize))
	(setq ScreenSize (dcl_GetScreenSize))
	(setq ScreenX (car ScreenSize))
	(setq ScreenY (cadr ScreenSize))

	(if (/= (dcl_Control_GetCaption FormMover_Form1_cmdMin) "_")
		(progn
			(dcl_Control_SetVisible FormMover_Form1_Label1 nil)
			(if (< lf (- *CtrLf* *offsetX*))
				(dcl_Form_SetPos FormMover_Form1 (+ lf *offsetX*) (- tp *offsetY*) (+ wd *WdOffset*) (+ ht *HtOffset*));_ MAX
				(progn
					(setq *DONE* T)
					(dcl_Control_SetCaption FormMover_Form1_cmdMin "_")
					(dcl_Form_Resize FormMover_Form1 *MaxWd* *MaxHt*)
					(dcl_Form_Center FormMover_Form1)
				);_ progn
			);_ if
		);_ progn
		(progn
			(if (>= lf *offsetX*)
				(dcl_Form_SetPos FormMover_Form1 (- lf *offsetX*) (+ tp *offsetY*) (- wd *WdOffset*) (- ht *HtOffset*));_ MIN
				(progn
					(setq *DONE* T)
					(dcl_Control_SetVisible FormMover_Form1_Label1 T)
					(dcl_Control_SetCaption FormMover_Form1_cmdMin "[  ]")
					(dcl_Form_SetPos FormMover_Form1 0 (- ScreenY *MinHt*) *MinWd* *MinHt*)
				);_ progn
			);_ if
		);_ progn
	);_ if

	(if (null *DONE*)
		(dcl_DelayedInvoke 50 "C:FormMinMax");_ repeat
	)
	(princ)
)

;|<<OpenDCL Event Handlers>>|;

(defun c:FormMover_Form1_GraphicButton1_OnClicked ( /)
	(setq *FormPos* 0)
	(C:FormMover)
)

(defun c:FormMover_Form1_cmdMin_OnClicked ( /  FormSize lf tp wd ht ScreenSize ScreenX ScreenY);_ MIN/MAX
	(Setq FormSize (dcl_Form_GetRectangle FormMover_Form1))
	(setq lf (car FormSize))
	(setq tp (cadr FormSize))
	(setq wd (caddr FormSize))
	(setq ht (cadddr FormSize))
	(setq ScreenSize (dcl_GetScreenSize))
	(setq ScreenX (car ScreenSize))
	(setq ScreenY (cadr ScreenSize))

	(setq *MinHt* 32)
	(setq *MinWd* 150)
	(setq *MaxHt* 200)
	(setq *MaxWd* 300)
	(setq *STEPS* 10)

	(setq *CtrLf* (fix (- (* ScreenX 0.5) (* *MaxWd* 0.5))))
	(setq *Ctrtp* (fix (- (* ScreenY 0.5) (* *MaxHt* 0.5))))

	(setq *offsetX* (/ *CtrLf* *STEPS*))
	(setq *offsetY* (/ (- (- ScreenY *Ctrtp*) *MinHt*) *STEPS*))

	(setq *WdOffset* (/ (- *MaxWd* *MinWd*) *STEPS*))
	(setq *HtOffset* (/ (- *MaxHt* *MinHt*) *STEPS*))

	(setq *DONE* nil)
	(C:FormMinMax)
)

(defun c:FormMover_Form1_cmdClose_OnClicked ( /);_ CLOSE
	(dcl_Form_Close FormMover_Form1)
)

;;Expand or Collapse the form
(defun c:FormMover_Form1_cmdExpand_OnClicked ( / );_ expand
	(if (= (Car (dcl_Form_GetControlArea FormMover_Form1)) 300)
		(progn
			(dcl_Form_Resize FormMover_Form1 500 200)
			(dcl_Control_SetPicture FormMover_Form1_cmdExpand 101)
		);_ progn
		(progn
			(dcl_Form_Resize FormMover_Form1 300 200)
			(dcl_Control_SetPicture FormMover_Form1_cmdExpand 100)
		);_ progn
	);_ if
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
	"Mover"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
