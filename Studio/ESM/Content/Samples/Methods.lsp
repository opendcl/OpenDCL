;;;
;;; Methods Sample
;;;
;;; This sample demonstrates various control methods
;;;

;; Main program
(defun c:Methods (/ cmdecho)

    ;; Ensure OpenDCL Runtime is (quietly) loaded
    (setq cmdecho (getvar "CMDECHO"))
    (setvar "CMDECHO" 0)
    (command "_OPENDCL")
    (setvar "CMDECHO" cmdecho)

    ;; Load the project
    (dcl_Project_Load (*ODCL:Samples:FindFile "Methods.odcl"))

    ;; Show the main form
    (dcl_Form_Show Methods_Main)

    ;; This is a modal form, so (dcl_Form_Show) does not return until
    ;; the modal form is closed. In the meantime, the event handlers
    ;; manage the form.

    (princ)
)

(defun PopulateManipulationTabFields (/ curPos tabSize)
    (setq curPos           (dcl_Control_GetCurPos Methods_Main_SampleControl)
          nLeft            (car curPos)
          nTop             (cadr curPos)
          nWidth           (caddr curPos)
          nHeight          (cadddr curPos)
          tabSize          (dcl_Tab_GetControlArea Methods_Main_TabControl)
          BottomFromBottom (- (cadr tabSize) nTop nHeight)
          RightFromRight   (- (car tabSize) nLeft nWidth)
          TopFromBottom    (- (cadr tabSize) nTop)
          LeftFromRight    (- (car tabSize) nLeft)
    )
    (dcl_Control_SetText Methods_Main_BottomFromBottom
                         (itoa BottomFromBottom)
    )
    (dcl_Control_SetText Methods_Main_RightFromRight
                         (itoa RightFromRight)
    )
    (dcl_Control_SetText Methods_Main_TopFromBottom (itoa TopFromBottom))
    (dcl_Control_SetText Methods_Main_LeftFromRight (itoa LeftFromRight))
    (dcl_Control_SetText Methods_Main_LeftBox (itoa nLeft))
    (dcl_Control_SetText Methods_Main_TopBox (itoa nTop))
    (dcl_Control_SetText Methods_Main_WidthBox (itoa nWidth))
    (dcl_Control_SetText Methods_Main_HeightBox (itoa nHeight))
)

;|<<OpenDCL Event Handlers>>|;

(defun c:Methods_Main_cmdClose_OnClicked (/)
    (dcl_Form_Close Methods_Main)
)

(defun c:Methods_Main_TabControl_OnChanged (nSelIndex /
                                            BottomFromBottom
                                            RightFromRight  TopFromBottom
                                            LeftFromRight   nLeft
                                            nTop            nWidth
                                            nHeight
                                           )
    (if (= nSelIndex 1) ;_ Sliders Tab
        (progn (dcl_Control_SetValue
                   Methods_Main_AngleSlider1
                   (atoi (dcl_Control_GetText Methods_Main_AngleBox))
               )
               (dcl_Control_SetValue
                   Methods_Main_ScrollBar1
                   (atoi (dcl_Control_GetText Methods_Main_VScrollLabel))
               )
               (dcl_Control_SetValue
                   Methods_Main_SliderBar1
                   (atoi (dcl_Control_GetText Methods_Main_VSliderLabel))
               )
               (dcl_Control_SetValue
                   Methods_Main_ScrollBar2
                   (atoi (dcl_Control_GetText Methods_Main_HScrollLabel))
               )
               (dcl_Control_SetValue
                   Methods_Main_SliderBar2
                   (atoi (dcl_Control_GetText Methods_Main_HSliderLabel))
               )
               (dcl_Control_SetText
                   Methods_Main_SpinnedBox
                   (itoa (dcl_Control_GetValue Methods_Main_SpinButton1))
               )
        )
    ) ;_ if
    (if (= nSelIndex 2) ;_ Manipulation Tab
        (PopulateManipulationTabFields)
    )                                             ; _if
)

;; ------------------- PictureBox Tab -------------------------------

(defun c:Methods_Main_DrawText_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (dcl_PictureBox_DrawText Methods_Main_PictureBox
                             (List (List 10 10 -19 20 "Line 1" "TL")
                                   (List 10 25 -19 30 "Line 2" "TL")
                                   (List 10 40 -19 40 "Line 3" "TL")
                                   (List 10 55 -19 50 "Line 4" "TL" T)
                                   (List 10 70 -19 56 "Line 5" "TL" T)
                             )
    )
)

(defun c:Methods_Main_DrawWrappedText_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (dcl_PictureBox_DrawWrappedText
        Methods_Main_PictureBox
        '((10
           10
           220
           -19
           "This sample text is supposed to wrap around if the width of the text is going to exceed the allowed width given."
           "L"
          )
          (10
           70
           220
           -19
           "This sample text is also supposed to wrap around if the width of the text is going to exceed the allowed width given."
           "C"
          )
          (10
           130
           220
           -19
           "This sample text is also supposed to wrap around if the width of the text is going to exceed the allowed width given."
           "R"
          )
         )
    )
)

(defun c:Methods_Main_DrawLine_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (dcl_PictureBox_DrawLine Methods_Main_PictureBox
                             '((10 10 230 10 248)
                               (10 20 230 20 249)
                               (10 30 230 30 250)
                               (10 40 230 40 251)
                               (10 50 230 50 252)
                               (10 60 230 60 253)
                               (10 70 230 70 254)
                               (10 80 230 80 255)
                              )
    )
)
(defun c:Methods_Main_DrawArc_OnClicked (/ nBoundingLeft nBoundingTop)
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (setq nBoundingLeft 50)
    (setq nBoundingTop 50)
    ;; OpenDCLARX is requires larger lists to be created in a  certain format, ensure you create list in the manor shown below
                                                  ; we have experienced errors when specifying a list as follows
    ;; '((nBoundingLeft nBoundingTop ...) (nBoundingLeft nBoundingTop ...)),
    ;; For some unknown reason OpenDCLARX cannot handle list created in this manor.
    (setq ArgumentList (list (list nBoundingLeft
                                   nBoundingTop
                                   25
                                   (+ nBoundingLeft 25)
                                   nBoundingTop
                                   (+ nBoundingLeft 25)
                                   (+ nBoundingTop 50)
                                   248
                             )
                             (list nBoundingLeft
                                   (+ nBoundingTop 50)
                                   25
                                   (+ nBoundingLeft 25)
                                   (+ nBoundingTop 100)
                                   (+ nBoundingLeft 25)
                                   (+ nBoundingTop 50)
                                   248
                             )
                       )
    )
    (dcl_PictureBox_DrawArc Methods_Main_PictureBox ArgumentList)
)

(defun c:Methods_Main_DrawCircle_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (dcl_PictureBox_DrawCircle Methods_Main_PictureBox
                               '((10 10 140 140 1) (10 160 140 40 2))
    )
)

(defun c:Methods_Main_DrawHatchRect_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (dcl_PictureBox_DrawHatchRect Methods_Main_PictureBox
                                  '((10 10 40 40 1 1)
                                    (10 60 40 40 2 2)
                                    (60 10 40 40 3 3)
                                    (60 60 40 40 4 4)
                                    (10 110 40 40 5 5)
                                   )
    )
)

(defun c:Methods_Main_DrawFillRect_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (dcl_PictureBox_DrawFillRect Methods_Main_PictureBox
                                 '((10 10 40 40 1)
                                   (10 60 40 40 2)
                                   (60 10 40 40 3)
                                   (60 60 40 40 4)
                                   (10 110 40 40 5)
                                  )
    )
)

(defun c:Methods_Main_PaintPicture_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (setq PictureList (list (list 10 10 102 T T)
                            (list 10 50 105 T T)
                            (list 10 90 106 T T)
                            (list 10 130 107 T T)
                            (list 10 170 108 T T)
                      )
    )
    (dcl_PictureBox_PaintPicture Methods_Main_PictureBox PictureList)
)

(defun c:Methods_Main_DrawPoints_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (dcl_PictureBox_DrawPoint Methods_Main_PictureBox 14 10 1)
    (dcl_PictureBox_DrawPoint Methods_Main_PictureBox 18 10 2)
    (dcl_PictureBox_DrawPoint Methods_Main_PictureBox 22 10 3)
    (dcl_PictureBox_DrawPoint Methods_Main_PictureBox 26 10 4)
)

(defun c:Methods_Main_DrawEdge_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
    (dcl_PictureBox_DrawEdge Methods_Main_PictureBox
                             '((10 10 40 40 1)
                               (60 10 40 40 2)
                               (10 60 40 40 3)
                               (60 60 40 40 4)
                               (10 110 40 40 5)
                               (60 110 40 40 6)
                               (10 160 40 40 7)
                               (60 160 40 40 8)
                              )
    )
)

(defun c:Methods_Main_LoadPictureFile_OnClicked (/ sFileName)
    (setq sFileName
             (GetFiled "Select a picture file" "" "bmp;ico;jpg;gif;wmf" 8)
    )
    (if (/= sFileName nil)
        (dcl_PictureBox_LoadPictureFile Methods_Main_PictureBox
                                        sFileName
        )
        (dcl_PictureBox_StoreImage Methods_Main_PictureBox)
    )
)

(defun c:Methods_Main_StoreImage_OnClicked ()
    (dcl_PictureBox_StoreImage Methods_Main_PictureBox)
)

(defun c:Methods_Main_Clear_OnClicked ()
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
)

(defun c:Methods_Main_PictureBox_OnMouseDblClick (Button Flags X Y /)
    (dcl_PictureBox_Clear Methods_Main_PictureBox)
)

;; ------------------- Sliders Tab -------------------------------

(defun c:Methods_Main_ScrollBar1_OnScroll (nValue /)
    (dcl_Control_SetText Methods_Main_VScrollLabel (itoa nValue))
)

(defun c:Methods_Main_ScrollBar2_OnScroll (nValue /)
    (dcl_Control_SetText Methods_Main_HScrollLabel (itoa nValue))
)

(defun c:Methods_Main_AngleSlider1_OnScroll (nValue /)
    (dcl_Control_SetText Methods_Main_AngleBox (itoa nValue))
)

(defun c:Methods_Main_SliderBar2_OnReleasedCapture (nValue /)
    (dcl_Control_SetText Methods_Main_HSliderLabel (itoa nValue))
)

(defun c:Methods_Main_SliderBar1_OnReleasedCapture (nValue /)
    (dcl_Control_SetText Methods_Main_VSliderLabel (itoa nValue))
)

(defun c:Methods_Main_SpinButton1_OnChanged (nValue nDelta/)
    (dcl_Control_SetText Methods_Main_SpinnedBox (itoa nValue))
)

(defun c:Methods_Main_AngleBox_OnEditChanged (sText /)
    (dcl_Control_SetValue Methods_Main_AngleSlider1 (atoi sText))
)

(defun c:Methods_Main_VScrollLabel_OnEditChanged (sText /)
    (dcl_Control_SetValue Methods_Main_ScrollBar1 (atoi sText))
)

(defun c:Methods_Main_VSliderLabel_OnEditChanged (sText /)
    (dcl_Control_SetValue Methods_Main_SliderBar1 (atoi sText))
)

(defun c:Methods_Main_HScrollLabel_OnEditChanged (sText /)
    (dcl_Control_SetValue Methods_Main_ScrollBar2 (atoi sText))
)

(defun c:Methods_Main_HSliderLabel_OnEditChanged (sText /)
    (dcl_Control_SetValue Methods_Main_SliderBar2 (atoi sText))
)


(defun c:Methods_Main_SpinnedBox_OnEditChanged (sText /)
    (dcl_Control_SetValue Methods_Main_SpinButton1 (atoi sText))
)


;; ------------------- Manipulation Tab -------------------------------

(defun c:Methods_Main_LeftBox_OnEditChanged (sText / curPos tabSize nVal)
    (setq curPos  (dcl_Control_GetCurPos Methods_Main_SampleControl)
          tabSize (dcl_Tab_GetControlArea Methods_Main_TabControl)
          nVal    (atoi sText)
    )
    (dcl_Control_SetLeft Methods_Main_SampleControl nVal)
    (dcl_Control_SetLeftFromRight Methods_Main_SampleControl
                                  (setq nVal (- (car tabSize) nVal))
    )
    (dcl_Control_SetText Methods_Main_LeftFromRight nVal)
)

(defun c:Methods_Main_TopBox_OnEditChanged (sText / curPos tabSize nVal)
    (setq curPos  (dcl_Control_GetCurPos Methods_Main_SampleControl)
          tabSize (dcl_Tab_GetControlArea Methods_Main_TabControl)
          nVal    (atoi sText)
    )
    (dcl_Control_SetTop Methods_Main_SampleControl nVal)
    (dcl_Control_SetTopFromBottom Methods_Main_SampleControl
                                  (setq nVal (- (cadr tabSize) nVal))
    )
    (dcl_Control_SetText Methods_Main_TopFromBottom nVal)
)

(defun c:Methods_Main_WidthBox_OnEditChanged (sText / curPos tabSize nVal)
    (setq curPos  (dcl_Control_GetCurPos Methods_Main_SampleControl)
          tabSize (dcl_Tab_GetControlArea Methods_Main_TabControl)
          nVal    (atoi sText)
    )
    (dcl_Control_SetWidth Methods_Main_SampleControl nVal)
    (dcl_Control_SetRightFromRight
        Methods_Main_SampleControl
        (setq nVal (- (car tabSize) nVal (car curPos)))
    )
    (dcl_Control_SetText Methods_Main_RightFromRight nVal)
)

(defun c:Methods_Main_HeightBox_OnEditChanged (sText / curPos tabSize nVal)
    (setq curPos  (dcl_Control_GetCurPos Methods_Main_SampleControl)
          tabSize (dcl_Tab_GetControlArea Methods_Main_TabControl)
          nVal    (atoi sText)
    )
    (dcl_Control_SetHeight Methods_Main_SampleControl nVal)
    (dcl_Control_SetBottomFromBottom
        Methods_Main_SampleControl
        (setq nVal (- (cadr tabSize) nVal (cadr curPos)))
    )
    (dcl_Control_SetText Methods_Main_BottomFromBottom nVal)
)

(defun c:Methods_Main_VisibleCheck_OnClicked (nValue /)
    (dcl_Control_SetVisible Methods_Main_SampleControl
                            (not (zerop nValue))
    )
)

(defun c:Methods_Main_EnableOption_OnClicked (nValue /)
    (dcl_Control_SetEnabled Methods_Main_SampleControl T)
)

(defun c:Methods_Main_DisableOption_OnClicked (nValue /)
    (dcl_Control_SetEnabled Methods_Main_SampleControl nil)
)

(defun c:Methods_Main_LeftCheck_OnClicked (nValue /)
    (dcl_Control_SetUseLeftFromRight Methods_Main_SampleControl nValue)
)

(defun c:Methods_Main_TopCheck_OnClicked (nValue /)
    (dcl_Control_SetUseTopFromBottom Methods_Main_SampleControl nValue)
)

(defun c:Methods_Main_RightCheck_OnClicked (nValue /)
    (dcl_Control_SetUseRightFromRight Methods_Main_SampleControl nValue)
)

(defun c:Methods_Main_BottomCheck_OnClicked (nValue /)
    (dcl_Control_SetUseBottomFromBottom Methods_Main_SampleControl
                                        nValue
    )
)

(defun c:Methods_Main_LeftFromRight_OnEditChanged (sText / curPos tabSize nVal)
    (setq curPos  (dcl_Control_GetCurPos Methods_Main_SampleControl)
          tabSize (dcl_Tab_GetControlArea Methods_Main_TabControl)
          nVal    (atoi sText)
    )
    (dcl_Control_SetLeftFromRight Methods_Main_SampleControl nVal)
    (dcl_Control_SetLeft Methods_Main_SampleControl
                         (setq nVal (- (car tabSize) nVal))
    )
    (dcl_Control_SetText Methods_Main_LeftBox nVal)
)

(defun c:Methods_Main_TopFromBottom_OnEditChanged (sText / curPos tabSize nVal)
    (setq curPos  (dcl_Control_GetCurPos Methods_Main_SampleControl)
          tabSize (dcl_Tab_GetControlArea Methods_Main_TabControl)
          nVal    (atoi sText)
    )
    (dcl_Control_SetTopFromBottom Methods_Main_SampleControl nVal)
    (dcl_Control_SetTop Methods_Main_SampleControl
                        (setq nVal (- (cadr tabSize) nVal))
    )
    (dcl_Control_SetText Methods_Main_TopBox nVal)
)

(defun c:Methods_Main_RightFromRight_OnEditChanged (sText / curPos tabSize nVal)
    (setq curPos  (dcl_Control_GetCurPos Methods_Main_SampleControl)
          tabSize (dcl_Tab_GetControlArea Methods_Main_TabControl)
          nVal    (atoi sText)
    )
    (dcl_Control_SetRightFromRight Methods_Main_SampleControl nVal)
    (dcl_Control_SetWidth Methods_Main_SampleControl
                          (setq nVal (- (car tabSize) nVal (car curPos)))
    )
    (dcl_Control_SetText Methods_Main_WidthBox nVal)
)

(defun c:Methods_Main_BottomFromBottom_OnEditChanged (sText / curPos tabSize nVal)
    (setq curPos  (dcl_Control_GetCurPos Methods_Main_SampleControl)
          tabSize (dcl_Tab_GetControlArea Methods_Main_TabControl)
          nVal    (atoi sText)
    )
    (dcl_Control_SetBottomFromBottom Methods_Main_SampleControl nVal)
    (dcl_Control_SetHeight Methods_Main_SampleControl
                           (setq nVal (- (cadr tabSize) nVal (cadr curPos)))
    )
    (dcl_Control_SetText Methods_Main_HeightBox nVal)
)

(defun c:Methods_Main_OnSize (nWidth nHeight /)
    (PopulateManipulationTabFields)
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
    "Methods"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
