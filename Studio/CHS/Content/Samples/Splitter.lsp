;;;
;;; Splitter Sample
;;;
;;; This sample demonstrates the splitter control.
;;;

;; Main program
(defun c:splitter (/ lstDistance lstStyle lstThickness lstLastContent strStyle strThick strInner strOuter
                     set_style set_thickness check_distance set_inside set_outside
                     c:splitter_splitter_OnInitialize
                     c:splitter_splitter_OnCancelClose
                     c:splitter_splitter_spl_ver_OnSplitterMoved
                     c:Splitter_splitter_grd_OnBeginLabelEdit
                     c:Splitter_splitter_grd_OnEndLabelEdit
         )

  ;; Setting variables
  
  (setq lstDistance (list "10" "30" "50" "70"))
  (setq lstThickness (list "0" "1" "2" "3" "4" "5" "6" "7" "8"))
  (setq lstStyle (list "Raised" "Double raised" "Etched" "Flat" "Sunken" "Bump"))
  (setq strStyle "Style")
  (setq strThick "Thickness (in px)")
  (setq strInner "Closest to left/top")
  (setq strOuter "Closest to right/bottom")

  ;; Helper functions

  (defun set_style (n)
    (dcl-Control-SetEdgeStyle splitter_splitter_spl_hor n)
    (dcl-Control-SetEdgeStyle splitter_splitter_spl_ver n)
  ); set_style

  (defun set_thickness (n)
    (dcl-Control-SetHeight splitter_splitter_spl_hor n)
    (dcl-Control-SetWidth splitter_splitter_spl_ver n)
  ); set_thickness

  (defun set_outside (n)
    (dcl-Control-SetClosestOutside splitter_splitter_spl_hor n)
    (dcl-Control-SetClosestOutside splitter_splitter_spl_ver n)
  ); set_outside

  (defun set_inside (n)
    (dcl-Control-SetClosestInside splitter_splitter_spl_hor n)
    (dcl-Control-SetClosestInside splitter_splitter_spl_ver n)
  ); set_inside

  (defun check_distance (strValue)
    (if (not (member strValue lstDistance))
      (setq lstDistance (vl-sort (cons strValue lstDistance)
         '(lambda (a b) (< (atoi a) (atoi b)))))
    ); if
  ); check_distance

  (defun check_thickness (strValue)
    (if (not (member strValue lstThickness))
      (setq lstThickness (vl-sort (cons strValue lstThickness)
         '(lambda (a b) (< (atoi a) (atoi b)))))
    ); if
  ); check_thickness

  ;|«OpenDCL Event Handlers»|;

  ;; This event is called before the form is shown.
  ;; It sets default values for the dialog and its controls.
  (defun c:splitter_splitter_OnInitialize (/ intRow lstRow intInside intOutside intThickness)
    (setq intInside (dcl-Control-GetClosestInside splitter_splitter_spl_ver))
    (check_distance (itoa intInside))

    (setq intOutside (dcl-Control-GetClosestOutside splitter_splitter_spl_ver))
    (check_distance (itoa intOutside))

    (setq intThickness (dcl-Control-GetWidth splitter_splitter_spl_ver))
    (check_thickness (itoa intThickness))

    (setq intStyle (dcl-Control-GetEdgeStyle splitter_splitter_spl_ver))
    
    (foreach lstRow (list (list strInner 38 lstDistance (itoa intInside))
        (list strOuter 38 lstDistance (itoa intOutside))
        (list strThick 38 lstThickness (itoa intThickness))
        (list strStyle 18 lstStyle (nth intStyle lstStyle))
        )
      (if (setq intRow (dcl-Grid-AddString Splitter_splitter_grd (car lstRow)))
  (progn
    (dcl-Grid-SetCellStyle Splitter_splitter_grd intRow 1 (cadr lstRow))
    (dcl-Grid-SetCellDropList Splitter_splitter_grd intRow 1 (caddr lstRow))
    (dcl-Grid-SetCellText Splitter_splitter_grd intRow 1 (last lstRow))
  ); progn
      ); if
    ); foreach

    (dcl-Control-SetText Splitter_splitter_edt_info "")
  ); c:splitter_splitter_OnInitialize

  ;; This event is called when Enter is pressed in the grid
  ;; To prevent the form from closing this event function must return T
  ;; But there must be a way that this event function will return NIL
  ;; to close the form
  (defun c:splitter_splitter_OnCancelClose (intIsESC /)
    (dcl-Grid-CancelCellEdit Splitter_splitter_grd)
    (/= intIsESC 1)
  ); c:splitter_splitter_OnCancelClose

  ;; This event is called when the vertical splitter is moved
  ;; to recalculate the grid's column width
  (defun c:splitter_splitter_spl_ver_OnSplitterMoved (intUpperLeftX intUpperLeftY intWidth intHeight /)
    (setq intWidth (fix (* 0.5 (dcl-Control-GetWidth Splitter_splitter_grd))))
    (dcl-Grid-SetColumnWidth Splitter_splitter_grd 0 intWidth)
    (dcl-Grid-SetColumnWidth Splitter_splitter_grd 1 intWidth)
  ); c:splitter_splitter_spl_ver_OnSplitterMoved

  ;; This event is called when a cell is going to be edited.
  ;; Just save the latest values for the possible case to restore
  (defun c:Splitter_splitter_grd_OnBeginLabelEdit (intRow intCol /)
    (setq lstLastContent (list intRow intCol (dcl-Grid-GetCellText Splitter_splitter_grd intRow intCol)))
  ); c:Splitter_splitter_grd_OnBeginLabelEdit

  ;; This event is called when cell editing is completed.
  (defun c:Splitter_splitter_grd_OnEndLabelEdit (intRow intCol / strValue strProp intPos)
    (cond
      ((not lstLastContent) nil)
      ((/= intRow (car lstLastContent)) nil)
      ((or (not (setq strValue (dcl-Grid-GetCellText Splitter_splitter_grd intRow 1)))
     (= (vl-string-trim " " strValue) ""))
       (dcl-Grid-SetCellText Splitter_splitter_grd intRow 1 (last lstLastContent)))
      ((or (not (setq strProp (dcl-Grid-GetCellText Splitter_splitter_grd intRow 0)))
     (= strProp ""))
       (dcl-Grid-SetCellText Splitter_splitter_grd intRow 1 (last lstLastContent)))
      ((= strProp strStyle)
       (if (setq intPos (vl-position strValue lstStyle))
   (set_style intPos)
   (dcl-Grid-SetCellText Splitter_splitter_grd intRow 1 (last lstLastContent))
       ))
      ((= strProp strThick)
       (check_thickness strValue)
       (dcl-Grid-SetCellDropList Splitter_splitter_grd intRow 1 lstThickness)
       (set_thickness (atoi strValue)))
      ((= strProp strInner)
       (check_distance strValue)
       (dcl-Grid-SetCellDropList Splitter_splitter_grd intRow 1 lstDistance)
       (set_inside (atoi strValue)))
      ((= strProp strOuter)
       (check_distance strValue)
       (dcl-Grid-SetCellDropList Splitter_splitter_grd intRow 1 lstDistance)
       (set_outside (atoi strValue)))
    ); cond
    (setq lstLastContent nil)
  ); c:Splitter_splitter_grd_OnEndLabelEdit

  ;; This event is called when the selection has changed in the grid
  (defun c:Splitter_splitter_grd_OnSelChanged (intRow intCol / strProp)
    (setq strProp (dcl-Grid-GetCellText Splitter_splitter_grd intRow 0))
    (cond
      ((= strProp strStyle) (dcl-Control-SetText Splitter_splitter_edt_info "The edge style controls how the splitter is painted."))
      ((= strProp strThick) (dcl-Control-SetText Splitter_splitter_edt_info "This property determines the width of the vertical splitter and the height of the horizontal splitter."))
      ((= strProp strInner) (dcl-Control-SetText Splitter_splitter_edt_info "This property determines how close to the top or left edge the splitter is allowed to travel."))
      ((= strProp strOuter) (dcl-Control-SetText Splitter_splitter_edt_info "This property determines how close to the bottom or right edge the splitter is allowed to travel."))
      (T (dcl-Control-SetText Splitter_splitter_edt_info ""))
    ); cond
    nil
  ); c:Splitter_splitter_grd_OnSelChanged

  ;|<<Show the form>>|;

  ;; Ensure OpenDCL Runtime is (quietly) loaded
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Load the project
  (dcl-Project-Load (*ODCL:Samples-FindFile "Splitter.odcl"))

  ;; Show the main form
  (dcl-Form-Show splitter_splitter)

  ;; This is a modal form, so (dcl-Form-Show) does not return until
  ;; the modal form is closed. In the meantime, the event handlers
  ;; manage the form.

  (princ)
); splitter

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
  "Splitter"
)
(princ)

;;;######################################################################
;;;######################################################################
