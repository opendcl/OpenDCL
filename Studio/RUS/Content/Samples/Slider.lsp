;;;
;;; Custom Slider Control Sample
;;;
;;; Author: Fred Tomke
;;; Date: 2009-10-27
;;;
;;; Demonstrates how to create and manage a custom slider control based
;;; on a generic OpenDCL picture box control.
;;; 
;;; Modified by: Owen Wengerd [2009-10-30]
;;; I rewrote large portions of Fred's fine work to eliminate all globals
;;; and reformatted to more closely resemble the other OpenDCL samples


;; Main program
(defun c:Slider (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl-Project-Load (*ODCL:Samples-FindFile "Slider.odcl"))

	;; Show the main form
	(dcl-Form-Show Slider/Form1)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)


;; --------------------------------------------------------------------------------------
;; Description: converts the slider button y-coordinate into percentage
;; Arguments: intTop = new y coordinate of slider button in picture box client coordinates
;; Returns: calculated percentage as integer

(defun slider_get_percent (intPos / intBorder intButtonHeight intHeight)
	(setq intBorder (slider_get_border_size))
	(setq intButtonHeight (slider_get_button_height))
	(setq intHeight (- (dcl-Control-GetHeight Slider/Form1/pic_slider) intBorder intButtonHeight 1))
	(cond
		((<= intHeight 0) 0)
		((>= intPos intHeight) 0)
		((<= intPos 0) 100)
		((fix (+ 0.5 (* 100.0 (/ (float (- intHeight intPos)) (float intHeight))))))
	); cond
); slider_get_percent


;; --------------------------------------------------------------------------------------
;; Description: converts the percentage value into y-coordinate for the slider button
;; Arguments: intPercent = percentage value as integer
;; Returns: new y-coordinate in picture box client coordinates as integer

(defun slider_get_pos (intPercent / intBorder intButtonHeight intHeight)
	(setq intBorder (slider_get_border_size))
	(setq intButtonHeight (slider_get_button_height))
	(setq intHeight (- (dcl-Control-GetHeight Slider/Form1/pic_slider) intBorder intButtonHeight 1))
	(cond
		((<= intHeight 0) 0)
		((<= intPercent 0) intHeight)
		((>= intPercent 100) 0)
		((- intHeight (fix (+ 0.5 (* (/ intPercent 100.0) (float intHeight))))))
	); cond
); slider_get_pos


;; --------------------------------------------------------------------------------------
;; Description: updates the slider position while dragging
;; Arguments: intPercent = new percentage value as integer
;; Returns: <unused>

(defun slider_change_percent (intPercent)
	(cond
		((< intPercent 0) (setq intPercent 0))
		((> intPercent 100) (setq intPercent 100))
	); cond
	(if (/= intPercent(atoi (dcl-Control-GetText Slider/Form1/edt_percent)))
		(progn
			(dcl-Control-SetText Slider/Form1/edt_percent (itoa intPercent))
			(dcl-Control-Redraw Slider/Form1/pic_slider)
		); progn
	); if
); slider_change_percent


;; --------------------------------------------------------------------------------------
;; Description: initiates dragging by enabling the OnMouseMove event
;; Arguments: none
;; Returns: <unused>

(defun slider_start_dragging ()
	(dcl-Control-SetProperty Slider/Form1/pic_slider "MouseMove" "c:Slider_Form1_pic_slider_OnMouseMove")
); slider_start_dragging


;; --------------------------------------------------------------------------------------
;; Description: stops dragging by disabling the OnMouseMove event
;; Arguments: none
;; Returns: <unused>

(defun slider_stop_dragging ()
	(dcl-Control-SetProperty Slider/Form1/pic_slider "MouseMove" "")
); slider_stop_dragging


;; --------------------------------------------------------------------------------------
;; Description: Validates and returns the large increment value
;; Arguments: none
;; Returns: large increment as integer

(defun slider_get_large_increment (/ intVal)
	(setq intVal (atoi (dcl-ComboBox-GetEBText Slider/Form1/cbb_large)))
	(cond
		((<= intVal 5) 5)
		((>= intVal 50) 50)
		(intVal)
	); cond
); slider_get_large_increment


;; --------------------------------------------------------------------------------------
;; Description: Validates and returns the small increment value
;; Arguments: none
;; Returns: small increment as integer

(defun slider_get_small_increment (/ intVal)
	(setq intVal (atoi (dcl-ComboBox-GetEBText Slider/Form1/cbb_small)))
	(cond
		((<= intVal 1) 1)
		((>= intVal 20) 20)
		(intVal)
	); cond
); slider_get_small_increment


;; --------------------------------------------------------------------------------------
;; Description: Calculates the button height based on the selected style
;; Arguments: none
;; Returns: button height as integer

(defun slider_get_button_height (/ intButtonStyle)
	(setq intButtonStyle (dcl-ComboBox-GetCurSel Slider/Form1/cbb_style))
	(cond
		((member intButtonStyle '(-1 0 1)) 12)
		((= intButtonStyle 3) 3)
		((= intButtonStyle 4) 16)
		(T 0)
	); cond
); slider_get_button_height


;; --------------------------------------------------------------------------------------
;; Description: Calculates the border size based on the selected border style
;; Arguments: none
;; Returns: border size as integer

(defun slider_get_border_size (/ intBorderStyle)
	(setq intBorderStyle (dcl-Control-GetBorderStyle Slider/Form1/pic_slider))
	(cond
		((= intBorderStyle 0) 0)
		((= intBorderStyle 1) 4)
		((= intBorderStyle 2) 2)
		(T 0)
	); cond
); slider_get_border_size


;; --------------------------------------------------------------------------------------
;; Description: Appends a new line to the end of the event log and scrolls it into view
;; Arguments: strLine = new event log text as string
;; Returns: <unused>

(defun slider_log_event (strLine)
	(if (= 1 (dcl-Control-GetValue Slider/Form1/chb_event))
		(dcl-ListBox-SetTopIndex Slider/Form1/lbx_eventlog (1- (dcl-ListBox-AddString Slider/Form1/lbx_eventlog strLine)))
	); if
); slider_log_event


;|«OpenDCL Event Handlers»|;

;; --------------------------------------------------------------------------------------
;; Event desc.: Called when dcl-Form-show is executed. Sets initial values for the form and its controls
;; Arguments: none
;; Returns: <unused>

(defun c:Slider/Form1#OnInitialize (/)
;|
	;; Uncomment this code to reset the increment combos every time the form is shown
	(mapcar 'dcl-ComboBox-Clear (list Slider/Form1/cbb_small Slider/Form1/cbb_large))
	(dcl-ComboBox-AddList Slider/Form1/cbb_small (list "1" "2" "5"))
	(dcl-ComboBox-AddList Slider/Form1/cbb_large (list "5" "10" "20"))
	(dcl-ComboBox-SetCurSel Slider/Form1/cbb_small 0)
	(dcl-ComboBox-SetCurSel Slider/Form1/cbb_large 0)
|;
	(dcl-ListBox-Clear Slider/Form1/lbx_eventlog)
	(dcl-ComboBox-SetCurSel Slider/Form1/cbb_border (dcl-Control-GetBorderStyle Slider/Form1/pic_slider))
); c:Slider/Form1#OnInitialize


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when Return was pressed in the textbox or the user has pressed ESC or he is closing the form.
;; Arguments: intIsESC = reason of closing as integer; 0, when Return was pressed in the textbox
;; Returns: boole, T to prevent the form from closing

(defun c:Slider/Form1#OnCancelClose (intIsESC /)
	(slider_log_event (strcat "OnCancelClose / " (vl-prin1-to-string intIsESC)))
	(/= intIsESC 1)
); c:Slider/Form1#OnCancelClose


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when the textbox content was changed
;; Arguments: strNewValue = new textbox content as string
;; Returns: <unused>

(defun c:Slider/Form1/edt_percent#OnEditChanged (strNewValue / intPercent)
	(slider_log_event (strcat "OnEditChanged / " strNewValue))
	(setq intPercent (atoi strNewValue))
	(cond
		((< intPercent 0) (dcl-Control-SetText Slider/Form1/edt_percent "0"))
		((> intPercent 100) (dcl-Control-SetText Slider/Form1/edt_percent "100"))
	); cond
	(dcl-PictureBox-Refresh Slider/Form1/pic_slider)
); c:Slider/Form1/edt_percent#OnEditChanged


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when the user changes the scrollbar style
;; Arguments: intStyle = index of selected item
;;  strValue = text value of selected item
;; Returns: <unused>

(defun c:Slider/Form1/cbb_style#OnSelChanged (intStyle strValue /)
	(dcl-PictureBox-Refresh Slider/Form1/pic_slider)
); c:Slider/Form1/cbb_style#OnSelChanged


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when the user changes the borderstyle style
;; Arguments: intBorder = index of selected borderstyle
;;  strValue = text value of selected item
;; Returns: <unused>

(defun c:Slider/Form1/cbb_border#OnSelChanged (intBorder strValue /)
	(dcl-Control-SetBorderStyle Slider/Form1/pic_slider intBorder)
); c:Slider/Form1/cbb_border#OnSelChanged


;; --------------------------------------------------------------------------------------
;; Event desc.: Called whenever the picturebox has to be repainted
;; Arguments: isHasFocus = boole, T id picturebox has focus
;; Returns: <unused>

(defun c:Slider/Form1/pic_slider#OnPaint (isHasFocus / intPos intStyle lstPos lstLines intLeft)
	(slider_log_event (strcat "OnPaint / " (vl-prin1-to-string isHasFocus)))
	(setq intPos (slider_get_pos (atoi (dcl-Control-GetText Slider/Form1/edt_percent))))
	(setq lstPos (dcl-Control-GetPos Slider/Form1/pic_slider))
	(setq intStyle (dcl-ComboBox-GetCurSel Slider/Form1/cbb_style))
	(cond
		(	(or (not intStyle) (<= intStyle 0)) ; scroll bar
			(dcl-PictureBox-PaintPicture Slider/Form1/pic_slider (list (list 0 intPos 100 T nil))))

	(	(= intStyle 1) ; solid rectangle
		(dcl-PictureBox-DrawSolidRect Slider/Form1/pic_slider
			(list
				(list 0 intPos (caddr lstPos) 2 0)
				(list 0 (+ intPos 2) (caddr lstPos) 8 171)
				(list 0 (+ intPos 10) (caddr lstPos) 2 0))))

	(	(= intStyle 2) ; solid fill
		(dcl-PictureBox-DrawSolidRect Slider/Form1/pic_slider
			(list (list 0 intPos (caddr lstPos) (- (cadddr lstPos) intPos) 171))))

	(	(= intStyle 3) ; solid triangle
		(repeat (1+ (setq intLen (1- (caddr lstPos))))
			(setq lstLines (cons (list 0 (last lstPos) (setq intLen (1- intLen)) 0 163) lstLines))
		); repeat
		(dcl-PictureBox-DrawLine Slider/Form1/pic_slider lstLines)
		(dcl-PictureBox-DrawSolidRect Slider/Form1/pic_slider
			(list (list 0 intPos (caddr lstPos) 4 0))))

	(	(= intStyle 4) ; bullet
		(setq intLeft (/ (caddr lstPos) 2))
		(dcl-PictureBox-DrawSolidRect Slider/Form1/pic_slider
			(list (list intLeft 0 1 (last lstPos) 8)))
		(dcl-PictureBox-PaintPicture Slider/Form1/pic_slider
			(list (list (- intLeft 8) intPos 101 T nil))))
	); cond
); c:Slider/Form1/pic_slider#OnPaint


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when the mouse moves over the picturebox.
;; Arguments: none
;; Returns: <unused>
;; Notes: To make sure that picturebox has focus for OnKeyPressed event and mousewheel

(defun c:Slider/Form1/pic_slider#OnMouseEntered (/)
	(dcl-Control-SetFocus Slider/Form1/pic_slider)
); c:Slider/Form1/pic_slider#OnMouseEntered


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when the user presses a mouse button on the control
;; Arguments: intButton = button as integer
;;  intFlags = sum of bitflags as integer
;;  intX = client x coordinate of mouseposition as integer
;;  intY = client y coordinate of mouseposition as integer
;; Returns: <unused>
;; Notes: The MouseDblClick event handler has been set to the same function name as
;;  the MouseDown event in the .odcl, which results both single click and
;;  double click events callign this event handler
;;  Left mousebutton over the scrollbar button = large step up
;;  Left mousebutton below the scrollbar button = large step down
;;  Left mousebutton on the scrollbar button = move directly to destination
;;  Right mousebutton anywhere = move directly to destination

(defun c:Slider/Form1/pic_slider#OnMouseDown (intButton intFlags intX intY / intPercent intTop intButtonHeight)
	(slider_log_event (strcat "OnMouseDown / " (vl-prin1-to-string (list intButton intFlags intX intY))))
	(setq intPercent (atoi (dcl-Control-GetText Slider/Form1/edt_percent)))
	(setq intTop (slider_get_pos intPercent))
	(setq intButtonHeight (slider_get_button_height))
	(cond
		(	(/= intButton 1)
			(slider_change_percent (slider_get_percent (- intPos (/ intButtonHeight 2)))))
		(	(< intY intTop)
			(slider_change_percent (+ intPercent (slider_get_large_increment))))
		(	(< intY (+ intTop intButtonHeight))
			(slider_start_dragging))
		(T
			(slider_change_percent (- intPercent (slider_get_large_increment))))
	); cond
); c:Slider/Form1/pic_slider#OnMouseDown


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when the user releases a mouse button on the control
;; Arguments: intButton = button as integer
;;  intFlags = sum of bitflags as integer
;;  intX = client x coordinate of mouseposition as integer
;;  intY = client y coordinate of mouseposition as integer
;; Returns: <unused>

(defun c:Slider/Form1/pic_slider#OnMouseUp (intButton intFlags intX intY / intPercent intTop intButtonHeight)
	(slider_log_event (strcat "OnMouseUp / " (vl-prin1-to-string (list intButton intFlags intX intY))))
	(slider_stop_dragging)
); c:Slider/Form1/pic_slider#OnMouseUp


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when the user moves the mouse over the control.
;; Arguments: intFlags = sum of bitflags as integer
;;  intX = client x coordinate of mouseposition as integer
;;  intY = client y coordinate of mouseposition as integer
;; Returns: <unused>
;; Notes: Triggers the moving of the slider button while the mouse button is pressed.

(defun c:Slider_Form1_pic_slider_OnMouseMove (intFlags intX intY /)
	;; logging mouse move events quickly overflows the list box
	;(slider_log_event (strcat "OnMouseMove / " (vl-prin1-to-string (list intFlags intX intY))))
	(if (= (logand intFlags 1) 1)
		(slider_change_percent (slider_get_percent (- intY (/ (slider_get_button_height) 2))))
	); if
); c:Slider_Form1_pic_slider_OnMouseMove


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when the user has scrolled the mousewheel
;; Arguments: intFlags = sum of bitflags as integer
;;  intZDelta = number of lines which are scrolled, negative, when down
;;  intX = client x coordinate of mouseposition as integer
;;  intY = client y coordinate of mouseposition as integer
;; Returns: <unused>
;; Notes: Scrolling down = small step down
;;  Scrolling up = small step up
;;  Pressing Shift-Key while scrolling uses large step instead of small steps

(defun c:Slider/Form1/pic_slider#OnMouseWheel (intFlags intZDelta intX intY / intPercent intInc)
	(slider_log_event (strcat "OnMouseWheel / " (vl-prin1-to-string (list intFlags intZDelta intX intY))))
	(setq intInc (if (zerop (logand intFlags 4)) (slider_get_small_increment) (slider_get_large_increment)))
	(setq intPercent (atoi (dcl-Control-GetText Slider/Form1/edt_percent)))
	(slider_change_percent (+ intPercent (fix (* intZDelta intInc 0.01))))
); c:Slider/Form1/pic_slider#OnMouseWheel


;; --------------------------------------------------------------------------------------
;; Event desc.: Called when a key was pressed while picturebox has focus
;; Arguments: strCharacter = character as string
;;  intRepeatCount = number of repeating as integer
;;  intFlags = sum of bitflags as integer
;; Returns: <unused>
;; Notes: PgUp = large step up
;;  PgDown = large step down
;;  End = bottom
;;  Home = top
;;  Left or Up = small step up
;;  Right or Down = small step down

(defun c:Slider/Form1/pic_slider#OnKeyDown (strCharacter intRepeatCount intFlags / intChar intSmall intLarge)
	(setq intChar (ascii strCharacter))
	(slider_log_event (strcat "OnKeyDown / " strCharacter "{" (itoa intChar) "} " (itoa intFlags)))
	(setq intPercent (atoi (dcl-Control-GetText Slider/Form1/edt_percent)))
	(if (= (logand intFlags 256) 256)
		(cond
			((= intChar 33) (slider_log_event " \=> Page Up") (slider_change_percent (+ intPercent (slider_get_large_increment))))
			((= intChar 34) (slider_log_event " \=> Page Down") (slider_change_percent (- intPercent (slider_get_large_increment))))
			((= intChar 35) (slider_log_event " \=> End") (slider_change_percent 100))
			((= intChar 36) (slider_log_event " \=> Home") (slider_change_percent 0))
			((= intChar 37) (slider_log_event " \=> Left") (slider_change_percent (+ intPercent (slider_get_small_increment))))
			((= intChar 38) (slider_log_event " \=> Up") (slider_change_percent (+ intPercent (slider_get_small_increment))))
			((= intChar 39) (slider_log_event " \=> Right") (slider_change_percent (- intPercent (slider_get_small_increment))))
			((= intChar 40) (slider_log_event " \=> Down") (slider_change_percent (- intPercent (slider_get_small_increment))))
		); cond
	); if
); c:Slider/Form1/pic_slider#OnKeyDown


;; --------------------------------------------------------------------------------------
(princ)

;|«OpenDCL Samples Epilog»|;

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

;; If AllSamples is active, run the main function immediately; otherwise
;; display a banner. The extra gymnastics allow the sample name to be
;; specified in only one place, thus making it easier to reuse this code.
(	(lambda (demoname)
		(if *ODCL:AllSamples
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
	"Slider"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
