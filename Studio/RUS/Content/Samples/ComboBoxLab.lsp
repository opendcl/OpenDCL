;; COMBOBOXLAB
;; 2014-01-28
;; 
;; Provied an interface for testing and comparing combo box properties, methods,
;; and events.
;;
;; Author: Owen Wengerd (owenw@manusoft.com)
;;
;;
;; This file is placed in the public domain, and may be redistributed or used
;; for any purpose. Changes to this file should be clearly identified as such.
;; 
;;


;|«OpenDCL Global Constants»|;


;; Ordered list of combo box styles, used to map display name to style value, and vice versa
(setq *ComboBoxLab@ComboStyles '(
  "Standard combo"
  "Simple combo"
  "Dropdown combo"
  "Arrow styles"
  "Colors"
  "Lineweights"
  "Plot style names"
  "Plot style tables"
  "Font dropdown"
  "Font simple"
  "Plotters"
  "Paper sizes"
  "Folder selection"
  "Layers"
  "Linetypes"
))


;; Ordered list of information used to populate and operate each row of the property grid
;;   row format: (display-name cell-style (cell-droplist) getter-function setter-function)
(setq *ComboBoxLab@PropGridRows (list
  (list "Style" 18 *ComboBoxLab@ComboStyles
    (function (lambda (ctrl) (dcl-Control-GetComboBoxStyle ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetComboBoxStyle ctrl val) (*ComboBoxLab-Restart)))
  )
  (list "Dropdown Height" 8 NIL
    (function (lambda (ctrl) (dcl-Control-GetEventInvoke ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetEventInvoke ctrl val)))
  )
  (list "Enabled" 1 '("Disabled" "Enabled")
    (function (lambda (ctrl) (if (dcl-Control-GetEnabled ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetEnabled ctrl (not (zerop val)))))
  )
  (list "Event Invoke" 1 '("Synchronous" "Asynchronous")
    (function (lambda (ctrl) (dcl-Control-GetEventInvoke ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetEventInvoke ctrl val)))
  )
  (list "Font" 25 NIL
    (function (lambda (ctrl) (dcl-Control-GetFont ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetFont ctrl val)))
  )
  (list "Font Bold" 1 '("Normal" "Bold")
    (function (lambda (ctrl) (if (dcl-Control-GetFontBold ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetFontBold ctrl (not (zerop val)))))
  )
  (list "Font Italic" 1 '("Normal" "Italic")
    (function (lambda (ctrl) (if (dcl-Control-GetFontItalic ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetFontItalic ctrl (not (zerop val)))))
  )
  (list "Font Size" 38 '("-8" "-9" "-10" "-11" "-12" "-14" "-16" "-18" "-20" "-22" "-24" "-26" "-28" "-36" "-48" "48" "36" "28" "26" "24" "22" "20" "18" "16" "14" "12" "11" "10" "9" "8")
    (function (lambda (ctrl) (dcl-Control-GetFontSize ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetFontSize ctrl val)))
  )
  (list "Font Strikeout" 1 '("Normal" "Strikeout")
    (function (lambda (ctrl) (if (dcl-Control-GetFontStrikeout ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetFontStrikeout ctrl (not (zerop val)))))
  )
  (list "Font Underline" 1 '("Normal" "Underlined")
    (function (lambda (ctrl) (if (dcl-Control-GetFontUnderline ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetFontUnderline ctrl (not (zerop val)))))
  )
  (list "Height" 0 NIL
    (function (lambda (ctrl) (itoa (dcl-Control-GetHeight ctrl))))
    (function (lambda (ctrl val) (dcl-Control-SetHeight ctrl (atoi val))))
  )
  (list "Is Tab Stop" 1 '("No" "Yes")
    (function (lambda (ctrl) (if (dcl-Control-GetIsTabStop ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetIsTabStop ctrl (not (zerop val)))))
  )
  (list "Name" 0 NIL
    (function (lambda (ctrl) (dcl-Control-GetName ctrl)))
    (function (lambda (ctrl val) "# Changing the control name is not supported!"))
  )
  (list "Visible" 1 '("Invisible" "Visible")
    (function (lambda (ctrl) (if (dcl-Control-GetVisible ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetVisible ctrl (not (zerop val)))))
  )
  (list "Return As Tab" 1 '("No" "Yes")
    (function (lambda (ctrl) (if (dcl-Control-GetReturnAsTab ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetReturnAsTab ctrl (not (zerop val)))))
  )
  (list "Sorted" 1 '("Unsorted" "Sorted")
    (function (lambda (ctrl) (if (dcl-Control-GetSorted ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetSorted ctrl (not (zerop val)))))
  )
  (list "Text" 6 NIL
    (function (lambda (ctrl) (dcl-Control-GetText ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetText ctrl val)))
  )
  (list "Text Limit" 8 NIL
    (function (lambda (ctrl) (dcl-Control-GetTextLimit ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetTextLimit ctrl val)))
  )
  (list "ToolTip Balloon" 1 '("Rectangle" "Balloon")
    (function (lambda (ctrl) (if (dcl-Control-GetToolTipBalloon  ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetToolTipBalloon ctrl (not (zerop val)))))
  )
  (list "ToolTip Line" 1 '("No Line" "Line")
    (function (lambda (ctrl) (if (dcl-Control-GetToolTipLine  ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetToolTipLine ctrl (not (zerop val)))))
  )
  (list "ToolTip Main Text" 6 NIL
    (function (lambda (ctrl) (dcl-Control-GetToolTipMainText ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetToolTipMainText ctrl val)))
  )
  (list "ToolTip Picture" 8 NIL
    (function (lambda (ctrl) (dcl-Control-GetToolTipPicture ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetToolTipPicture ctrl val)))
  )
  (list "ToolTip Main Title" 6 NIL
    (function (lambda (ctrl) (dcl-Control-GetToolTipTitle ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetToolTipTitle ctrl val)))
  )
  (list "ToolTip Title Color" 8 NIL
    (function (lambda (ctrl) (dcl-Control-GetToolTipTitleColor ctrl)))
    (function (lambda (ctrl val) (dcl-Control-SetToolTipTitleColor ctrl val)))
  )
  (list "Use Visual Style" 1 '("No" "Yes")
    (function (lambda (ctrl) (if (dcl-Control-GetUseVisualStyle ctrl) 1 0)))
    (function (lambda (ctrl val) (dcl-Control-SetUseVisualStyle ctrl (not (zerop val))) (*ComboBoxLab-SaveSettings)))
  )
  (list "Width" 0 NIL
    (function (lambda (ctrl) (itoa (dcl-Control-GetWidth ctrl))))
    (function (lambda (ctrl val) (dcl-Control-SetWidth ctrl (atoi val))))
  )
))


;; This list maps all argument input controls that are available for use
;;   format: (style arg1-control arg2-control)
(setq *ComboBoxLab@ArgInputControls '(
  (-1 "TextBoxA1" "TextBoxB1")   ; textbox
  (0 "ComboBoxA1" "ComboBoxB1")  ; dropdown combo
  (2 "ComboBoxA2" "ComboBoxB2")  ; dropdown list
  (4 "ComboBoxA3" "ComboBoxB3")  ; acad colors
  (5 "ComboBoxA4" "ComboBoxB4")  ; lineweights
  (8 "ComboBoxA5" "ComboBoxB5")  ; fonts
  (12 "ComboBoxA6" "ComboBoxB6") ; directory picker
  (14 "ComboBoxA7" "ComboBoxB7") ; linetypes
))


;; Ordered list of information used to populate and operate the method call combo
;;   row format: (display-name (arg1-def) (arg2-def) exec-function)
;;   arg-def format: (style filter-or-droplist default-value)
;;   if style is negative, its absolute value is the combo box style and the second member is
;;   a list of string items for the combo list, otherwise style is the filter style of the
;;   textbox and the second member is the custom filter.
(setq *ComboBoxLab@Methods (list
  (list "AddColor" '(4 NIL 0) NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-AddColor ctrl (atoi (car arglist)))))
  )
  (list "AddList" '(0 NIL "ListItem1") '(0 NIL "ListItem2")
    (function (lambda (ctrl arglist) (dcl-ComboBox-AddList ctrl arglist)))
  )
  (list "AddPath" '(12 NIL NIL) NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-AddPath ctrl (car arglist))))
  )
  (list "AddString" '(0 NIL "NewItem") NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-AddString ctrl (car arglist))))
  )
  (list "Clear" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-Clear ctrl)))
  )
  (list "ClearEdit" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-ClearEdit ctrl)))
  )
  (list "DeleteItem" '(0 "0123456789" "0") NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-DeleteItem ctrl (atoi (car arglist)))))
  )
  (list "Dir" '(12 NIL NIL) '(0 NIL "Filter")
    (function (lambda (ctrl arglist) (dcl-ComboBox-Dir ctrl (car arglist) (if (eq (cadr arglist) "") NIL (cadr arglist)))))
  )
  (list "Dump" '(2 ("Shallow" "Deep") 0) NIL
    (function (lambda (ctrl arglist) (dcl-Control-Dump ctrl (= 1 (car arglist)))))
  )
  (list "FindColor" '(4 NIL 0) NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-FindColor ctrl (atoi (car arglist)))))
  )
  (list "FindLineWeight" '(5 NIL NIL) NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-FindLineWeight ctrl (atoi (car arglist)))))
  )
  (list "FindString" '(0 NIL "Search") '(0 "-0123456789" "-1")
    (function (lambda (ctrl arglist) (dcl-ComboBox-FindString ctrl (car arglist) (if (eq (cadr arglist) "") -1 (atoi (cadr arglist))))))
  )
  (list "FindStringExact" '(0 NIL "Search") '(0 "-0123456789" "-1")
    (function (lambda (ctrl arglist) (dcl-ComboBox-FindStringExact ctrl (car arglist) (if (eq (cadr arglist) "") -1 (atoi (cadr arglist))))))
  )
  (list "GetCount" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetCount ctrl)))
  )
  (list "GetCurSel" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetCurSel ctrl)))
  )
  (list "GetDir" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetDir ctrl)))
  )
  (list "GetDroppedWidth" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetDroppedWidth ctrl)))
  )
  (list "GetEBText" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetEBText ctrl)))
  )
  (list "GetEditSel" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetEditSel ctrl)))
  )
  (list "GetHWND" NIL NIL
    (function (lambda (ctrl arglist) (dcl-Control-GetHWND ctrl)))
  )
  (list "GetItemData" '(0 "0123456789" "0") NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetItemData ctrl (atoi (car arglist)))))
  )
  (list "GetLBText" '(0 "0123456789" "0") NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetLBText ctrl (atoi (car arglist)))))
  )
  (list "GetProperties" NIL NIL
    (function (lambda (ctrl arglist) (dcl-Control-GetProperties ctrl)))
  )
  (list "GetProperty" '(0 NIL "Name") NIL
    (function (lambda (ctrl arglist) (dcl-Control-GetProperty ctrl (car arglist))))
  )
  (list "GetTopIndex" NIL NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-GetTopIndex ctrl)))
  )
  (list "InsertString" '(0 "0123456789" "0") '(0 NIL "NewItem")
    (function (lambda (ctrl arglist) (dcl-ComboBox-InsertString ctrl (atoi (car arglist)) (cadr arglist))))
  )
  (list "Redraw" NIL NIL
    (function (lambda (ctrl arglist) (dcl-Control-Redraw ctrl)))
  )
  (list "SelectString" '(0 NIL "Search") '(0 "-0123456789" "-1")
    (function (lambda (ctrl arglist) (dcl-ComboBox-SelectString ctrl (car arglist) (if (eq (cadr arglist) "") -1 (atoi (cadr arglist))))))
  )
  (list "SetDroppedWidth" '(0 "0123456789" "0") NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-SetDroppedWidth ctrl (atoi (car arglist)))))
  )
  (list "SetCurSel" '(0 "-0123456789" "-1") NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-SetCurSel ctrl (atoi (car arglist)))))
  )
  (list "SetEditSel" '(0 "-0123456789" "-1") '(0 "-0123456789" "-1")
    (function (lambda (ctrl arglist) (dcl-ComboBox-SetEditSel ctrl (atoi (car arglist)) (atoi (cadr arglist)))))
  )
  (list "SetFocus" NIL NIL
    (function (lambda (ctrl arglist) (dcl-Control-SetFocus ctrl)))
  )
  (list "SetItemData" '(0 "0123456789" "0") '(0 "-0123456789" "0")
    (function (lambda (ctrl arglist) (dcl-ComboBox-SetItemData ctrl (atoi (car arglist)) (atoi (cadr arglist)))))
  )
  (list "SetTopIndex" '(0 "0123456789" "0") NIL
    (function (lambda (ctrl arglist) (dcl-ComboBox-SetTopIndex ctrl (atoi (car arglist)))))
  )
  (list "ShowToolTip" '(0 "-0123456789" "400") '(0 "-0123456789" "300")
    (function (lambda (ctrl arglist) (dcl-Control-ShowToolTip ctrl (atoi (car arglist)) (atoi (cadr arglist)))))
  )
  (list "ZOrder" '(0 ("Move to bottom" "Move to top") "0") NIL
    (function (lambda (ctrl arglist) (dcl-Control-ZOrder ctrl (atoi (car arglist)))))
  )
))


;|«OpenDCL Global Utility Functions»|;


;; Save a user setting in the registry
(defun *ComboBoxLab-SetRegVal (key name value / subkey)
  (vl-registry-write (strcat "HKEY_CURRENT_USER\\Software\\OpenDCL\\Samples\\ComboBoxLab" (if (and key (/= "" key)) (strcat "\\" key) "")) name value)
)


;; Get a user setting from the registry
(defun *ComboBoxLab-GetRegVal (key name)
  (vl-registry-read (strcat "HKEY_CURRENT_USER\\Software\\OpenDCL\\Samples\\ComboBoxLab" (if (and key (/= "" key)) (strcat "\\" key) "")) name)
)


;; Get the control handle from the form name and control name
(defun *ComboBoxLab-Ctrl ($form $ctrl)
  (eval (read (strcat "ComboBoxLab/" $form (if $ctrl (strcat "/" $ctrl) ""))))
)

;; Log to the left log list (if logging is enabled)
(defun *ComboBoxLab-Log ($form target msg / prefix targetlistview count logmsg targetlog targetqueue)
  (if (not *ComboBoxLab@LogNestLevel) (setq *ComboBoxLab@LogNestLevel 0))
  (setq prefix (substr msg 1 1))
  (cond
    ( (= ">" prefix)
      (setq msg (strcat (substr "     " 1 *ComboBoxLab@LogNestLevel) msg))
      (setq *ComboBoxLab@LogNestLevel (1+ *ComboBoxLab@LogNestLevel))
    )
    ( (= "<" prefix)
      (setq *ComboBoxLab@LogNestLevel (1- *ComboBoxLab@LogNestLevel))
      (setq msg (strcat (substr "     " 1 *ComboBoxLab@LogNestLevel) msg))
    )
    ( (setq msg (strcat (substr "     " 1 *ComboBoxLab@LogNestLevel) msg))
    )
  )
  (setq targetlog (assoc target *ComboBoxLab@LastLog))
  (setq count
    (if (eq msg (caddr targetlog))
      (1+ (cadr targetlog))
      1
    )
  )
  (setq logmsg (if (> count 1) (strcat msg " (x" (itoa count) ")") msg))
  (setq targetlistview (*ComboBoxLab-Ctrl $form (strcat "ListView" (itoa target))))
  (setq targetqueue (assoc target *ComboBoxLab-LogQueue))
  (cond
    ( (not targetlistview)
      (setq *ComboBoxLab-LogQueue
        (if targetqueue
          (subst
            (cons target (cons logmsg (if (> count 1) (cddr targetqueue) (cdr targetqueue))))
            targetqueue
            *ComboBoxLab-LogQueue
          )
          (append *ComboBoxLab-LogQueue (list (list target logmsg)))
        )
      )
    )
    ( (= 1 (dcl-Control-GetValue (*ComboBoxLab-Ctrl $form (strcat "CheckBox" (itoa target)))))
      (if targetqueue
        (progn
          (foreach item
            (reverse (if (> count 1) (cddr targetqueue) (cdr targetqueue)))
            (dcl-ListView-AddString targetlistview item)
          )
          (setq *ComboBoxLab-LogQueue (subst (list target) targetqueue *ComboBoxLab-LogQueue))
        )
      )
      (dcl-ListView-SetCurSel targetlistview
        (dcl-ListView-AddString targetlistview logmsg)
      )
      (if (> count 1)
        (dcl-ListView-DeleteItem targetlistview
          (- (dcl-ListView-GetCount targetlistview) 2)
        )
      )
      (if (> (dcl-ListView-GetCount targetlistview) 100)
        (dcl-ListView-DeleteItem targetlistview 0)
      )
    )
  )
  (setq *ComboBoxLab@LastLog
    (if targetlog
      (subst (list target count msg) targetlog *ComboBoxLab@LastLog)
      (append *ComboBoxLab@LastLog (list (list target count msg)))
    )
  )
)


;; Convert a lisp value into a loggable string
(defun *ComboBoxLab-LispValueToString (val)
  (cond
    ( (equal 'T val)
      "T"
    )
    ( (equal NIL val)
      "NIL"
    )
    ( (= 'STR (type val))
      (strcat "\"" (vl-princ-to-string val) "\"")
    )
    ( (numberp val)
      (vl-princ-to-string val)
    )
    ( (listp val)
      (strcat
        "LIST["
        (if (> 2 (length val))
          (itoa (length val))
          (vl-princ-to-string val)
        )
        "]"
      )
    )
    ( (strcat (vl-princ-to-string (type val)) "[" (vl-princ-to-string val) "]")
    )
  )
)


;; Log an event
(defun *ComboBoxLab-LogEvent ($form target event)
  (*ComboBoxLab-Log $form target (strcat "! On" event))
)


;; Log a method call
(defun *ComboBoxLab-LogMethod ($form target method arglist / argstr)
  (foreach arg arglist (setq argstr (strcat (if argstr argstr "") " " (vl-princ-to-string arg))))
  (*ComboBoxLab-Log $form target (strcat "> " method (if argstr argstr "")))
)


;; Log a result
(defun *ComboBoxLab-LogResult ($form target result)
  (*ComboBoxLab-Log $form target (strcat "< " (*ComboBoxLab-LispValueToString result)))
)


;; Get the current cell value and return it in the form expected by the property setter function in *ComboBoxLab@PropGridRows
(defun *ComboBoxLab-PropValueFromCell ($form Row Column / cellstyle)
  (setq cellstyle (dcl-Grid-GetCellStyle (*ComboBoxLab-Ctrl $form "Grid1") Row Column))
  (cond
    ( (= 1 cellstyle) ;checkbox
      (dcl-Grid-GetCellCheckState (*ComboBoxLab-Ctrl $form "Grid1") Row Column)
    )
    ( (= 8 cellstyle) ;integer
      (atoi (dcl-Grid-GetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column))
    )
    ( (= 18 cellstyle) ;droplist
      (vl-position (dcl-Grid-GetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column) (car (dcl-Grid-GetCellDropList (*ComboBoxLab-Ctrl $form "Grid1") Row Column)))
    )
    ( (= 38 cellstyle) ;integer combo
      (atoi (dcl-Grid-GetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column))
    )
    ( T ;plain text
      (dcl-Grid-GetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column)
    )
  )
)


;; Set the current cell value using whatever is returned by the property getter function in *ComboBoxLab@PropGridRows
(defun *ComboBoxLab-PropValueToCell ($form Row Column Value / cellstyle)
  (setq cellstyle (dcl-Grid-GetCellStyle (*ComboBoxLab-Ctrl $form "Grid1") Row Column))
  (cond
    ( (= 1 cellstyle) ;checkbox
      (dcl-Grid-SetCellCheckState (*ComboBoxLab-Ctrl $form "Grid1") Row Column Value)
      (dcl-Grid-SetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column (nth Value (car (dcl-Grid-GetCellDropList (*ComboBoxLab-Ctrl $form "Grid1") Row Column))))
    )
    ( (= 8 cellstyle) ;integer
      (dcl-Grid-SetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column (itoa Value))
    )
    ( (= 18 cellstyle) ;droplist
      (dcl-Grid-SetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column (nth Value (car (dcl-Grid-GetCellDropList (*ComboBoxLab-Ctrl $form "Grid1") Row Column))))
    )
    ( (= 38 cellstyle) ;integer combo
      (dcl-Grid-SetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column (itoa Value))
    )
    ( T ;plain text
      (dcl-Grid-SetCellText (*ComboBoxLab-Ctrl $form "Grid1") Row Column Value)
    )
  )
  Value ;return the value
)


;; Update the displayed combo properties
(defun *ComboBoxLab-AllPropValuesToCell ($form target / row)
  (if (*ComboBoxLab-Ctrl $form "Grid1")
    (progn
      (setq row -1)
      (foreach prop *ComboBoxLab@PropGridRows
        (*ComboBoxLab-PropValueToCell $form (setq row (1+ row)) target
          ((eval (nth 3 prop)) (*ComboBoxLab-Ctrl $form (cond ((= 2 target) "ComboBoxR")("ComboBoxL"))))
        )
      )
    )
  )
)


;; Close the current form and redisplay (i.e. because dropdown style changed)
(defun *ComboBoxLab-Restart (/ form-type)
  (setq form-type (*ComboBoxLab-GetRegVal NIL "FormType"))
  (*ComboBoxLab-SaveSettings)
  (dcl-Form-Close (*ComboBoxLab-Ctrl form-type NIL))
  (dcl-DelayedInvoke 10 "C:COMBOBOXLAB")
)


;; Save persisted user settings
(defun *ComboBoxLab-SaveSettings (/ form-type)
  (setq form-type (*ComboBoxLab-GetRegVal NIL "FormType"))
  (*ComboBoxLab-SetRegVal "ComboBoxL" "Style" (itoa (dcl-Control-GetProperty "ComboBoxLab" form-type "ComboBoxL" "ComboBoxStyle")))
  (*ComboBoxLab-SetRegVal "ComboBoxL" "UseVisualStyle" (if (dcl-Control-GetProperty "ComboBoxLab" form-type "ComboBoxL" "UseVisualStyle") "1" "0"))
  (*ComboBoxLab-SetRegVal "ComboBoxR" "Style" (itoa (dcl-Control-GetProperty "ComboBoxLab" form-type "ComboBoxR" "ComboBoxStyle")))
  (*ComboBoxLab-SetRegVal "ComboBoxR" "UseVisualStyle" (if (dcl-Control-GetProperty "ComboBoxLab" form-type "ComboBoxR" "UseVisualStyle") "1" "0"))
)


;; Transfer all properties from one form to a different one (i.e. after switching form type)
(defun *ComboBoxLab-XferProps ($form $source-form / control-name source-control source-props prop-val)
  (if (and $source-form (/= $form $source-form))
    (foreach $control (dcl-Form-GetControls "ComboBoxLab" $form)
      (setq control-name (dcl-Control-GetName $control))
      (foreach $prop (dcl-Control-GetProperties $control)
        (setq source-control (if (= control-name form-name) (list "ComboBoxLab" $source-form) (list "ComboBoxLab" $source-form control-name)))
        (setq source-props (apply 'dcl-Control-GetProperties source-control))
        (if (member $prop source-props)
          (cond
            ( (= "(" (substr $prop 1 1))
            )
            ( (and (= 'STR (type (setq prop-val (apply 'dcl-Control-GetProperty (append source-control (list $prop)))))) (= "c:" (substr prop-val 1 2)))
              (if (= control-name $form)
                (dcl-Control-SetProperty $control $prop (strcat "c:ComboBoxLab/" $form "#On" $prop))
                (dcl-Control-SetProperty $control $prop (strcat "c:ComboBoxLab/" $form "/" control-name "#On" $prop))
              )
            )
            ( (dcl-Control-SetProperty $control $prop prop-val)
            )
          )
        )
      )
    )
  )
)


;|«Form-Agnostic Event Handler Implementation»|;


(defun *ComboBoxLab/$#OnInitialize ($form / grid row init-method)
  (if (and *ComboBoxLab@InitFormWidth *ComboBoxLab@InitFormHeight)
    (dcl-Form-Resize (*ComboBoxLab-Ctrl $form NIL) *ComboBoxLab@InitFormWidth *ComboBoxLab@InitFormHeight)
  )
  (dcl-ListView-AddColumns (*ComboBoxLab-Ctrl $form "ListView1") (list (list "" 0 (- (dcl-Control-GetWidth (*ComboBoxLab-Ctrl $form "ListView1")) 4))))
  (dcl-ListView-AddColumns (*ComboBoxLab-Ctrl $form "ListView2") (list (list "" 0 (- (dcl-Control-GetWidth (*ComboBoxLab-Ctrl $form "ListView2")) 4))))
  (dcl-Grid-Clear (setq grid (*ComboBoxLab-Ctrl $form "Grid1")))
  (foreach prop *ComboBoxLab@PropGridRows
    (setq row (dcl-Grid-AddRow grid (nth 0 prop) "" "" ""))
    (dcl-Grid-SetCellStyle grid row 1 (nth 1 prop))
    (dcl-Grid-SetCellDropList grid row 1 (nth 2 prop))
    (dcl-Grid-SetCellStyle grid row 2 (nth 1 prop))
    (dcl-Grid-SetCellDropList grid row 2 (nth 2 prop))
    (dcl-Grid-SetCellCheckState grid row 3 0)
    (if (zerop (nth 1 prop)) (dcl-Grid-SetCellStyle grid row 3 0))
    (*ComboBoxLab-PropValueToCell $form row 1 ((eval (nth 3 prop)) (*ComboBoxLab-Ctrl $form "ComboBoxL")))
    (*ComboBoxLab-PropValueToCell $form row 2 ((eval (nth 3 prop)) (*ComboBoxLab-Ctrl $form "ComboBoxR")))
  )
  (dcl-Grid-SetCellStyle grid 0 3 0)
  (dcl-Control-SetList (*ComboBoxLab-Ctrl $form "ComboBoxMethod") (mapcar 'car *ComboBoxLab@Methods))
  (cond
    ( (and
        *ComboBoxLab@InitMethod
        (>= (setq init-method (dcl-ComboBox-SelectString (*ComboBoxLab-Ctrl $form "ComboBoxMethod") *ComboBoxLab@InitMethod)) 0)
      )
    )
    ( (< (dcl-ComboBox-GetCurSel (*ComboBoxLab-Ctrl $form "ComboBoxMethod")) 0)
      (dcl-ComboBox-SetCurSel (*ComboBoxLab-Ctrl $form "ComboBoxMethod") 4) ; 'Clear'
    )
  )
  (*ComboBoxLab/$/ComboBoxMethod#OnSelChanged $form
    (dcl-ComboBox-GetCurSel (*ComboBoxLab-Ctrl $form "ComboBoxMethod"))
    (dcl-Control-GetText (*ComboBoxLab-Ctrl $form "ComboBoxMethod"))
  )
)


(defun *ComboBoxLab/$#OnSize ($form NewWidth NewHeight / colwidth)
  (setq colwidth (/ (- NewWidth 225) 2))
  (dcl-Grid-SetColumnWidth (*ComboBoxLab-Ctrl $form "Grid1") 1 colwidth)
  (dcl-Grid-SetColumnWidth (*ComboBoxLab-Ctrl $form "Grid1") 2 colwidth)
)


(defun *ComboBoxLab/$#OnCancelClose ($form Reason / row col)
  (cond
    ( (zerop Reason)
      (dcl-Grid-CancelCellEdit (*ComboBoxLab-Ctrl $form "Grid1"))
      T
    )
  )
)


(defun *ComboBoxLab/$/Grid1#OnSelChanged ($form Row Column /)
  (if (/= -1 Column) (dcl-Grid-SetCurCell (*ComboBoxLab-Ctrl $form "Grid1") Row))
)


(defun *ComboBoxLab/$/Grid1#OnEndLabelEdit ($form Row Column / newval)
  (cond
    ( (= Column 1)
      (setq newval (*ComboBoxLab-PropValueToCell $form Row 1 (*ComboBoxLab-PropValueFromCell $form Row 1)))
      (if (/= newval ((eval (nth 3 (nth Row *ComboBoxLab@PropGridRows))) (*ComboBoxLab-Ctrl $form "ComboBoxL")))
        ((eval (nth 4 (nth Row *ComboBoxLab@PropGridRows))) (*ComboBoxLab-Ctrl $form "ComboBoxL") newval)
      )
      (if (and (*ComboBoxLab-Ctrl $form "Grid1") (= 1 (dcl-Grid-GetCellCheckState (*ComboBoxLab-Ctrl $form "Grid1") Row 3)))
        (if (/= newval ((eval (nth 3 (nth Row *ComboBoxLab@PropGridRows))) (*ComboBoxLab-Ctrl $form "ComboBoxR")))
          ((eval (nth 4 (nth Row *ComboBoxLab@PropGridRows))) (*ComboBoxLab-Ctrl $form "ComboBoxR")
            (*ComboBoxLab-PropValueToCell $form Row 2 newval)
          )
        )
      )
    )
    ( (= Column 2)
      (setq newval (*ComboBoxLab-PropValueToCell $form Row 2 (*ComboBoxLab-PropValueFromCell $form Row 2)))
      (if (/= newval ((eval (nth 3 (nth Row *ComboBoxLab@PropGridRows))) (*ComboBoxLab-Ctrl $form "ComboBoxR")))
        ((eval (nth 4 (nth Row *ComboBoxLab@PropGridRows))) (*ComboBoxLab-Ctrl $form "ComboBoxR") newval)
      )
      (if (and (*ComboBoxLab-Ctrl $form "Grid1") (/= newval (*ComboBoxLab-PropValueFromCell $form Row 1)))
        (dcl-Grid-SetCellCheckState (*ComboBoxLab-Ctrl $form "Grid1") Row 3 0)
      )
    )
    ( (and (= Column 3) (= 1 (dcl-Grid-GetCellCheckState (*ComboBoxLab-Ctrl $form "Grid1") Row 3)))
      (setq newval (*ComboBoxLab-PropValueToCell $form Row 2 (*ComboBoxLab-PropValueFromCell $form Row 1)))
      (if (/= newval ((eval (nth 3 (nth Row *ComboBoxLab@PropGridRows))) (*ComboBoxLab-Ctrl $form "ComboBoxR")))
        ((eval (nth 4 (nth Row *ComboBoxLab@PropGridRows))) (*ComboBoxLab-Ctrl $form "ComboBoxR") newval)
      )
    )
  )
)


(defun *ComboBoxLab/$/ComboBoxMethod#OnSelChanged ($form ItemIndexOrCount Value
                       / method arg1 arg2 arg1style arg2style arg1filterstyle arg2filterstyle)
  (setq *ComboBoxLab@InitMethod (if (and Value (/= "" Value)) Value NIL))
  (cond
    ( (< ItemIndexOrCount 0)
      (foreach style *ComboBoxLab@ArgInputControls
        (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (cadr style)) NIL)
        (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (caddr style)) NIL)
        (dcl-Control-SetText (*ComboBoxLab-Ctrl $form (cadr style)) "")
        (dcl-Control-SetText (*ComboBoxLab-Ctrl $form (caddr style)) "")
        (dcl-Control-SetList (*ComboBoxLab-Ctrl $form (cadr style)) NIL)
        (dcl-Control-SetList (*ComboBoxLab-Ctrl $form (caddr style)) NIL)
      )
    )
    ( (setq method (nth ItemIndexOrCount *ComboBoxLab@Methods))
      (setq arg1 (nth 1 method) arg2 (nth 2 method))

      (setq arg1style (car arg1))
      (cond
        ( (null arg1style)
        )
        ( (< arg1style 0)
          (setq arg1style (- arg1style))
        )
        ( (not (and (cadr arg1) (listp (cadr arg1))))
          (setq arg1filterstyle arg1style arg1style -1)
        )
      )

      (setq arg2style (car arg2))
      (cond
        ( (null arg2style)
        )
        ( (< arg2style 0)
          (setq arg2style (- arg2style))
        )
        ( (not (and (cadr arg2) (listp (cadr arg2))))
          (setq arg2filterstyle arg2style arg2style -1)
        )
      )

      (foreach style *ComboBoxLab@ArgInputControls
        (cond
          ( (< (car style) 0)
            (if (= (car style) arg1style)
              (progn
                (dcl-TextBox-SetFilter (*ComboBoxLab-Ctrl $form (cadr style)) (cond ((cadr arg1)) ("")))
                (dcl-Control-SetText (*ComboBoxLab-Ctrl $form (cadr style)) (cond ((caddr arg1)) ("")))
                (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (cadr style)) T)
              )
              (progn
                (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (cadr style)) NIL)
                (dcl-TextBox-SetFilter (*ComboBoxLab-Ctrl $form (cadr style)) "")
                (dcl-Control-SetText (*ComboBoxLab-Ctrl $form (cadr style)) "")
              )
            )
            (if (= (car style) arg2style)
              (progn
                (dcl-TextBox-SetFilter (*ComboBoxLab-Ctrl $form (caddr style)) (cond ((cadr arg2)) ("")))
                (dcl-Control-SetText (*ComboBoxLab-Ctrl $form (caddr style)) (cond ((caddr arg2)) ("")))
                (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (caddr style)) T)
              )
              (progn
                (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (caddr style)) NIL)
                (dcl-TextBox-SetFilter (*ComboBoxLab-Ctrl $form (caddr style)) "")
                (dcl-Control-SetText (*ComboBoxLab-Ctrl $form (caddr style)) "")
              )
            )
          )
          ( T
            (if (= (car style) arg1style)
              (progn
                (dcl-Control-SetList (*ComboBoxLab-Ctrl $form (cadr style)) (cadr arg1))
                (dcl-ComboBox-SetCurSel (*ComboBoxLab-Ctrl $form (cadr style)) (cond ((caddr arg1)) (-1)))
                (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (cadr style)) T)
              )
              (progn
                (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (cadr style)) NIL)
                (dcl-Control-SetText (*ComboBoxLab-Ctrl $form (cadr style)) "")
                (dcl-Control-SetList (*ComboBoxLab-Ctrl $form (cadr style)) NIL)
              )
            )
            (if (= (car style) arg2style)
              (progn
                (dcl-Control-SetList (*ComboBoxLab-Ctrl $form (caddr style)) (cadr arg2))
                (dcl-ComboBox-SetCurSel (*ComboBoxLab-Ctrl $form (caddr style)) (cond ((caddr arg2)) (-1)))
                (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (caddr style)) T)
              )
              (progn
                (dcl-Control-SetVisible (*ComboBoxLab-Ctrl $form (caddr style)) NIL)
                (dcl-Control-SetText (*ComboBoxLab-Ctrl $form (caddr style)) "")
                (dcl-Control-SetList (*ComboBoxLab-Ctrl $form (caddr style)) NIL)
              )
            )
          )
        )
      )
    )
  )
)


(defun *ComboBoxLab/$/TextButton1#OnClicked ($form / idx method arg1 arg1raw arg2 arg2raw arglist arglistraw)
  (cond
    ( (>= (setq idx (dcl-ComboBox-GetCurSel (*ComboBoxLab-Ctrl $form "ComboBoxMethod"))) 0)
      (setq method (nth idx *ComboBoxLab@Methods))
      (foreach style *ComboBoxLab@ArgInputControls
        (if (dcl-Control-GetVisible (*ComboBoxLab-Ctrl $form (caddr style)))
          (setq
            arg2
            (if (< (car style) 0)
              (setq arg2raw (dcl-Control-GetText (*ComboBoxLab-Ctrl $form (caddr style))))
              (itoa (setq arg2raw (dcl-ComboBox-GetCurSel (*ComboBoxLab-Ctrl $form (caddr style)))))
            )
            arglist
            (cons arg2 arglist)
            arglistraw
            (cons (if (/= "" arg2) arg2raw NIL) arglistraw)
          )
        )
        (if (dcl-Control-GetVisible (*ComboBoxLab-Ctrl $form (cadr style)))
          (setq
            arg1
            (if (< (car style) 0)
              (setq arg1raw (dcl-Control-GetText (*ComboBoxLab-Ctrl $form (cadr style))))
              (itoa (setq arg1raw (dcl-ComboBox-GetCurSel (*ComboBoxLab-Ctrl $form (cadr style)))))
            )
            arglist
            (cons arg1 arglist)
            arglistraw
            (cons (if (/= "" arg1) arg1raw NIL) arglistraw)
          )
        )
      )
      (*ComboBoxLab-LogMethod $form 1 (nth 0 method) arglistraw)
      (*ComboBoxLab-LogResult $form 1 ((eval (nth 3 method)) (*ComboBoxLab-Ctrl $form "ComboBoxL") arglist))
      (*ComboBoxLab-AllPropValuesToCell $form 1)
    )
  )
)


(defun *ComboBoxLab/$/TextButton2#OnClicked ($form / idx method arg1 arg1raw arg2 arg2raw arglist arglistraw)
  (cond
    ( (>= (setq idx (dcl-ComboBox-GetCurSel (*ComboBoxLab-Ctrl $form "ComboBoxMethod"))) 0)
      (setq method (nth idx *ComboBoxLab@Methods))
      (foreach style *ComboBoxLab@ArgInputControls
        (if (dcl-Control-GetVisible (*ComboBoxLab-Ctrl $form (caddr style)))
          (setq
            arg2
            (if (< (car style) 0)
              (setq arg2raw (dcl-Control-GetText (*ComboBoxLab-Ctrl $form (caddr style))))
              (itoa (setq arg2raw (dcl-ComboBox-GetCurSel (*ComboBoxLab-Ctrl $form (caddr style)))))
            )
            arglist
            (cons arg2 arglist)
            arglistraw
            (cons (if (/= "" arg2) arg2raw NIL) arglistraw)
          )
        )
        (if (dcl-Control-GetVisible (*ComboBoxLab-Ctrl $form (cadr style)))
          (setq
            arg1
            (if (< (car style) 0)
              (setq arg1raw (dcl-Control-GetText (*ComboBoxLab-Ctrl $form (cadr style))))
              (itoa (setq arg1raw (dcl-ComboBox-GetCurSel (*ComboBoxLab-Ctrl $form (cadr style)))))
            )
            arglist
            (cons arg1 arglist)
            arglistraw
            (cons (if (/= "" arg1) arg1raw NIL) arglistraw)
          )
        )
      )
      (*ComboBoxLab-LogMethod $form 2 (nth 0 method) arglistraw)
      (*ComboBoxLab-LogResult $form 2 ((eval (nth 3 method)) (*ComboBoxLab-Ctrl $form "ComboBoxR") arglist))
      (*ComboBoxLab-AllPropValuesToCell $form 2)
    )
  )
)


(defun *ComboBoxLab/$/TextButton3#OnClicked ($form /)
  (*ComboBoxLab/$/TextButton1#OnClicked $form)
  (*ComboBoxLab/$/TextButton2#OnClicked $form)
)


(defun *ComboBoxLab/$/ComboBoxForm#OnSelChanged ($form ItemIndexOrCount Value / new-form)
  (cond
    ( (= Value "Modeless")
       (setq new-form "Modeless")
   )
    ( (= Value "Modal")
      (setq new-form "Modal")
    )
    ( (= Value "Control Bar")
      (setq new-form "ControlBar")
    )
    ( (= Value "Palette")
      (setq new-form "Palette")
    )
  )
  (if new-form
    (progn
      (dcl-Form-Close (*ComboBoxLab-Ctrl $form NIL))
      (*ComboBoxLab-SetRegVal NIL "FormType" new-form)
      (setq *ComboBoxLab@XferFrom $form)
      (dcl-DelayedInvoke 10 "C:COMBOBOXLAB")
    )
  )
)


(defun *ComboBoxLab/$/ComboBoxL#OnDropDown ($form /)
  (*ComboBoxLab-LogEvent $form 1 "DropDown")
)


(defun *ComboBoxLab/$/ComboBoxL#OnEditChanged ($form NewValue /)
  (*ComboBoxLab-LogEvent $form 1 (strcat "EditChanged " NewValue))
)


(defun *ComboBoxLab/$/ComboBoxL#OnKillFocus ($form /)
  (*ComboBoxLab-LogEvent $form 1 "KillFocus")
)


(defun *ComboBoxLab/$/ComboBoxL#OnMouseMove ($form Flags X Y /)
  (if (= 1 (dcl-Control-GetValue (*ComboBoxLab-Ctrl $form "CheckBox3")))
    (*ComboBoxLab-LogEvent $form 1 (strcat "MouseMove " (itoa Flags) " X Y"))
  )
)


(defun *ComboBoxLab/$/ComboBoxL#OnSelChanged ($form ItemIndexOrCount Value /)
  (*ComboBoxLab-LogEvent $form 1 (strcat "SelChanged "  (itoa ItemIndexOrCount) " " (if Value Value "NIL")))
  (*ComboBoxLab-AllPropValuesToCell $form 1)
)


(defun *ComboBoxLab/$/ComboBoxL#OnSetFocus ($form /)
  (*ComboBoxLab-LogEvent $form 1 "SetFocus")
)


(defun *ComboBoxLab/$/ComboBoxR#OnDropDown ($form /)
  (*ComboBoxLab-LogEvent $form 2 "DropDown")
)


(defun *ComboBoxLab/$/ComboBoxR#OnEditChanged ($form NewValue /)
  (*ComboBoxLab-LogEvent $form 2 (strcat "EditChanged " NewValue))
)


(defun *ComboBoxLab/$/ComboBoxR#OnKillFocus ($form /)
  (*ComboBoxLab-LogEvent $form 2 "KillFocus")
)


(defun *ComboBoxLab/$/ComboBoxR#OnMouseMove ($form Flags X Y /)
  (if (= 1 (dcl-Control-GetValue (*ComboBoxLab-Ctrl $form "CheckBox4")))
    (*ComboBoxLab-LogEvent $form 2 (strcat "MouseMove " (itoa Flags) " X Y"))
  )
)


(defun *ComboBoxLab/$/ComboBoxR#OnSelChanged ($form ItemIndexOrCount Value /)
  (*ComboBoxLab-LogEvent $form 2 (strcat "SelChanged "  (itoa ItemIndexOrCount) " " (if Value Value "NIL")))
  (*ComboBoxLab-AllPropValuesToCell $form 2)
)


(defun *ComboBoxLab/$/ComboBoxR#OnSetFocus ($form /)
  (*ComboBoxLab-LogEvent $form 2 "SetFocus")
)


;|«OpenDCL Event Handlers»|;


;; Modeless form
(defun c:ComboBoxLab/Modeless#OnInitialize (/) (*ComboBoxLab/$#OnInitialize "Modeless") (princ))
(defun c:ComboBoxLab/Modeless#OnSize (NewWidth NewHeight /) (*ComboBoxLab/$#OnSize "Modeless" NewWidth NewHeight) (princ))
(defun c:ComboBoxLab/Modeless#OnCancelClose (Reason /) (*ComboBoxLab/$#OnCancelClose "Modeless" Reason))
(defun c:ComboBoxLab/Modeless/Grid1#OnSelChanged (Row Column /) (*ComboBoxLab/$/Grid1#OnSelChanged "Modeless" Row Column) (princ))
(defun c:ComboBoxLab/Modeless/Grid1#OnEndLabelEdit (Row Column /) (*ComboBoxLab/$/Grid1#OnEndLabelEdit "Modeless" Row Column) (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxMethod#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxMethod#OnSelChanged "Modeless" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Modeless/TextButton1#OnClicked (/) (*ComboBoxLab/$/TextButton1#OnClicked "Modeless") (princ))
(defun c:ComboBoxLab/Modeless/TextButton2#OnClicked (/) (*ComboBoxLab/$/TextButton2#OnClicked "Modeless") (princ))
(defun c:ComboBoxLab/Modeless/TextButton3#OnClicked (/) (*ComboBoxLab/$/TextButton3#OnClicked "Modeless") (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxForm#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxForm#OnSelChanged "Modeless" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxL#OnDropDown (/) (*ComboBoxLab/$/ComboBoxL#OnDropDown "Modeless") (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxL#OnEditChanged (NewValue /) (*ComboBoxLab/$/ComboBoxL#OnEditChanged "Modeless" NewValue) (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxL#OnKillFocus (/) (*ComboBoxLab/$/ComboBoxL#OnKillFocus "Modeless") (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxL#OnMouseMove (Flags X Y /) (*ComboBoxLab/$/ComboBoxL#OnMouseMove "Modeless" Flags X Y) (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxL#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxL#OnSelChanged "Modeless" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxL#OnSetFocus (/) (*ComboBoxLab/$/ComboBoxL#OnSetFocus "Modeless") (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxR#OnDropDown (/) (*ComboBoxLab/$/ComboBoxR#OnDropDown "Modeless") (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxR#OnEditChanged (NewValue /) (*ComboBoxLab/$/ComboBoxR#OnEditChanged "Modeless" NewValue) (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxR#OnKillFocus (/) (*ComboBoxLab/$/ComboBoxR#OnKillFocus "Modeless") (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxR#OnMouseMove (Flags X Y /) (*ComboBoxLab/$/ComboBoxR#OnMouseMove "Modeless" Flags X Y) (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxR#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxR#OnSelChanged "Modeless" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Modeless/ComboBoxR#OnSetFocus (/) (*ComboBoxLab/$/ComboBoxR#OnSetFocus "Modeless") (princ))


;; Modal form
(defun c:ComboBoxLab/Modal#OnInitialize (/) (*ComboBoxLab/$#OnInitialize "Modal") (princ))
(defun c:ComboBoxLab/Modal#OnSize (NewWidth NewHeight /) (*ComboBoxLab/$#OnSize "Modal" NewWidth NewHeight) (princ))
(defun c:ComboBoxLab/Modal#OnCancelClose (Reason /) (*ComboBoxLab/$#OnCancelClose "Modal" Reason))
(defun c:ComboBoxLab/Modal/Grid1#OnSelChanged (Row Column /) (*ComboBoxLab/$/Grid1#OnSelChanged "Modal" Row Column) (princ))
(defun c:ComboBoxLab/Modal/Grid1#OnEndLabelEdit (Row Column /) (*ComboBoxLab/$/Grid1#OnEndLabelEdit "Modal" Row Column) (princ))
(defun c:ComboBoxLab/Modal/ComboBoxMethod#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxMethod#OnSelChanged "Modal" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Modal/TextButton1#OnClicked (/) (*ComboBoxLab/$/TextButton1#OnClicked "Modal") (princ))
(defun c:ComboBoxLab/Modal/TextButton2#OnClicked (/) (*ComboBoxLab/$/TextButton2#OnClicked "Modal") (princ))
(defun c:ComboBoxLab/Modal/TextButton3#OnClicked (/) (*ComboBoxLab/$/TextButton3#OnClicked "Modal") (princ))
(defun c:ComboBoxLab/Modal/ComboBoxForm#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxForm#OnSelChanged "Modal" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Modal/ComboBoxL#OnDropDown (/) (*ComboBoxLab/$/ComboBoxL#OnDropDown "Modal") (princ))
(defun c:ComboBoxLab/Modal/ComboBoxL#OnEditChanged (NewValue /) (*ComboBoxLab/$/ComboBoxL#OnEditChanged "Modal" NewValue) (princ))
(defun c:ComboBoxLab/Modal/ComboBoxL#OnKillFocus (/) (*ComboBoxLab/$/ComboBoxL#OnKillFocus "Modal") (princ))
(defun c:ComboBoxLab/Modal/ComboBoxL#OnMouseMove (Flags X Y /) (*ComboBoxLab/$/ComboBoxL#OnMouseMove "Modal" Flags X Y) (princ))
(defun c:ComboBoxLab/Modal/ComboBoxL#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxL#OnSelChanged "Modal" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Modal/ComboBoxL#OnSetFocus (/) (*ComboBoxLab/$/ComboBoxL#OnSetFocus "Modal") (princ))
(defun c:ComboBoxLab/Modal/ComboBoxR#OnDropDown (/) (*ComboBoxLab/$/ComboBoxR#OnDropDown "Modal") (princ))
(defun c:ComboBoxLab/Modal/ComboBoxR#OnEditChanged (NewValue /) (*ComboBoxLab/$/ComboBoxR#OnEditChanged "Modal" NewValue) (princ))
(defun c:ComboBoxLab/Modal/ComboBoxR#OnKillFocus (/) (*ComboBoxLab/$/ComboBoxR#OnKillFocus "Modal") (princ))
(defun c:ComboBoxLab/Modal/ComboBoxR#OnMouseMove (Flags X Y /) (*ComboBoxLab/$/ComboBoxR#OnMouseMove "Modal" Flags X Y) (princ))
(defun c:ComboBoxLab/Modal/ComboBoxR#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxR#OnSelChanged "Modal" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Modal/ComboBoxR#OnSetFocus (/) (*ComboBoxLab/$/ComboBoxR#OnSetFocus "Modal") (princ))


;; ControlBar form
(defun c:ComboBoxLab/ControlBar#OnInitialize (/) (*ComboBoxLab/$#OnInitialize "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar#OnSize (NewWidth NewHeight /) (*ComboBoxLab/$#OnSize "ControlBar" NewWidth NewHeight) (princ))
(defun c:ComboBoxLab/ControlBar#OnCancelClose (Reason /) (*ComboBoxLab/$#OnCancelClose "ControlBar" Reason))
(defun c:ComboBoxLab/ControlBar/Grid1#OnSelChanged (Row Column /) (*ComboBoxLab/$/Grid1#OnSelChanged "ControlBar" Row Column) (princ))
(defun c:ComboBoxLab/ControlBar/Grid1#OnEndLabelEdit (Row Column /) (*ComboBoxLab/$/Grid1#OnEndLabelEdit "ControlBar" Row Column) (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxMethod#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxMethod#OnSelChanged "ControlBar" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/ControlBar/TextButton1#OnClicked (/) (*ComboBoxLab/$/TextButton1#OnClicked "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar/TextButton2#OnClicked (/) (*ComboBoxLab/$/TextButton2#OnClicked "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar/TextButton3#OnClicked (/) (*ComboBoxLab/$/TextButton3#OnClicked "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxForm#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxForm#OnSelChanged "ControlBar" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxL#OnDropDown (/) (*ComboBoxLab/$/ComboBoxL#OnDropDown "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxL#OnEditChanged (NewValue /) (*ComboBoxLab/$/ComboBoxL#OnEditChanged "ControlBar" NewValue) (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxL#OnKillFocus (/) (*ComboBoxLab/$/ComboBoxL#OnKillFocus "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxL#OnMouseMove (Flags X Y /) (*ComboBoxLab/$/ComboBoxL#OnMouseMove "ControlBar" Flags X Y) (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxL#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxL#OnSelChanged "ControlBar" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxL#OnSetFocus (/) (*ComboBoxLab/$/ComboBoxL#OnSetFocus "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxR#OnDropDown (/) (*ComboBoxLab/$/ComboBoxR#OnDropDown "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxR#OnEditChanged (NewValue /) (*ComboBoxLab/$/ComboBoxR#OnEditChanged "ControlBar" NewValue) (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxR#OnKillFocus (/) (*ComboBoxLab/$/ComboBoxR#OnKillFocus "ControlBar") (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxR#OnMouseMove (Flags X Y /) (*ComboBoxLab/$/ComboBoxR#OnMouseMove "ControlBar" Flags X Y) (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxR#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxR#OnSelChanged "ControlBar" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/ControlBar/ComboBoxR#OnSetFocus (/) (*ComboBoxLab/$/ComboBoxR#OnSetFocus "ControlBar") (princ))


;; Palette form
(defun c:ComboBoxLab/Palette#OnInitialize (/) (*ComboBoxLab/$#OnInitialize "Palette") (princ))
(defun c:ComboBoxLab/Palette#OnSize (NewWidth NewHeight /) (*ComboBoxLab/$#OnSize "Palette" NewWidth NewHeight) (princ))
(defun c:ComboBoxLab/Palette#OnCancelClose (Reason /) (*ComboBoxLab/$#OnCancelClose "Palette" Reason))
(defun c:ComboBoxLab/Palette/Grid1#OnSelChanged (Row Column /) (*ComboBoxLab/$/Grid1#OnSelChanged "Palette" Row Column) (princ))
(defun c:ComboBoxLab/Palette/Grid1#OnEndLabelEdit (Row Column /) (*ComboBoxLab/$/Grid1#OnEndLabelEdit "Palette" Row Column) (princ))
(defun c:ComboBoxLab/Palette/ComboBoxMethod#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxMethod#OnSelChanged "Palette" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Palette/TextButton1#OnClicked (/) (*ComboBoxLab/$/TextButton1#OnClicked "Palette") (princ))
(defun c:ComboBoxLab/Palette/TextButton2#OnClicked (/) (*ComboBoxLab/$/TextButton2#OnClicked "Palette") (princ))
(defun c:ComboBoxLab/Palette/TextButton3#OnClicked (/) (*ComboBoxLab/$/TextButton3#OnClicked "Palette") (princ))
(defun c:ComboBoxLab/Palette/ComboBoxForm#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxForm#OnSelChanged "Palette" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Palette/ComboBoxL#OnDropDown (/) (*ComboBoxLab/$/ComboBoxL#OnDropDown "Palette") (princ))
(defun c:ComboBoxLab/Palette/ComboBoxL#OnEditChanged (NewValue /) (*ComboBoxLab/$/ComboBoxL#OnEditChanged "Palette" NewValue) (princ))
(defun c:ComboBoxLab/Palette/ComboBoxL#OnKillFocus (/) (*ComboBoxLab/$/ComboBoxL#OnKillFocus "Palette") (princ))
(defun c:ComboBoxLab/Palette/ComboBoxL#OnMouseMove (Flags X Y /) (*ComboBoxLab/$/ComboBoxL#OnMouseMove "Palette" Flags X Y) (princ))
(defun c:ComboBoxLab/Palette/ComboBoxL#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxL#OnSelChanged "Palette" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Palette/ComboBoxL#OnSetFocus (/) (*ComboBoxLab/$/ComboBoxL#OnSetFocus "Palette") (princ))
(defun c:ComboBoxLab/Palette/ComboBoxR#OnDropDown (/) (*ComboBoxLab/$/ComboBoxR#OnDropDown "Palette") (princ))
(defun c:ComboBoxLab/Palette/ComboBoxR#OnEditChanged (NewValue /) (*ComboBoxLab/$/ComboBoxR#OnEditChanged "Palette" NewValue) (princ))
(defun c:ComboBoxLab/Palette/ComboBoxR#OnKillFocus (/) (*ComboBoxLab/$/ComboBoxR#OnKillFocus "Palette") (princ))
(defun c:ComboBoxLab/Palette/ComboBoxR#OnMouseMove (Flags X Y /) (*ComboBoxLab/$/ComboBoxR#OnMouseMove "Palette" Flags X Y) (princ))
(defun c:ComboBoxLab/Palette/ComboBoxR#OnSelChanged (ItemIndexOrCount Value /) (*ComboBoxLab/$/ComboBoxR#OnSelChanged "Palette" ItemIndexOrCount Value) (princ))
(defun c:ComboBoxLab/Palette/ComboBoxR#OnSetFocus (/) (*ComboBoxLab/$/ComboBoxR#OnSetFocus "Palette") (princ))


;|«OpenDCL Main Function»|;


(defun C:COMBOBOXLAB (/ cmdecho form-type first-run value)

  ;; Ensure OpenDCL Runtime is (quietly) loaded
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Load the project
  (setq first-run (null ComboBoxLab/Modeless))
  (dcl-Project-Load (*ODCL:Samples-FindFile "ComboBoxLab.odcl"))

  ;; Clear globals
  (setq *ComboBoxLab-LogQueue NIL)
  (setq *ComboBoxLab@LastLog NIL)

  ;; Determine which form type to show, and xfer properties if needed
  (setq form-type (cond ((*ComboBoxLab-GetRegVal NIL "FormType")) ("Modeless")))
  (*ComboBoxLab-XferProps form-type *ComboBoxLab@XferFrom)
  (if (and *ComboBoxLab@XferFrom (/= form-type *ComboBoxLab@XferFrom))
    (setq
      *ComboBoxLab@InitFormWidth (dcl-Control-GetProperty "ComboBoxLab" *ComboBoxLab@XferFrom "Width")
      *ComboBoxLab@InitFormHeight (dcl-Control-GetProperty "ComboBoxLab" *ComboBoxLab@XferFrom "Height")
      *ComboBoxLab@InitMethod (dcl-Control-GetProperty "ComboBoxLab" *ComboBoxLab@XferFrom "ComboBoxMethod" "Text")
    )
  )

  (setq *ComboBoxLab@XferFrom NIL)
  (*ComboBoxLab-SetRegVal NIL "FormType" form-type) ; set as default for the next run

  ;; If combo styles were previously saved, set them from the saved values
  (if first-run
    (progn
      (if (setq value (*ComboBoxLab-GetRegVal "ComboBoxL" "Style"))
        (dcl-Control-SetProperty "ComboBoxLab" form-type "ComboBoxL" "ComboBoxStyle" (atoi value))
      )
      (if (setq value (*ComboBoxLab-GetRegVal "ComboBoxL" "UseVisualStyle"))
        (dcl-Control-SetProperty "ComboBoxLab" form-type "ComboBoxL" "UseVisualStyle" (= "1" value))
      )
      (if (setq value (*ComboBoxLab-GetRegVal "ComboBoxR" "Style"))
        (dcl-Control-SetProperty "ComboBoxLab" form-type "ComboBoxR" "ComboBoxStyle" (atoi value))
      )
      (if (setq value (*ComboBoxLab-GetRegVal "ComboBoxR" "UseVisualStyle"))
        (dcl-Control-SetProperty "ComboBoxLab" form-type "ComboBoxR" "UseVisualStyle" (= "1" value))
      )
    )
  )

  ;; Show the main form
  (dcl-Form-Show (*ComboBoxLab-Ctrl form-type NIL))

  (princ)
)


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
  "ComboBoxLab"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
