(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL sample programs.\nEnter \"Methods\" to run the sample.\n")
)


;; this function loads the project & shows the form
(DEFUN c:Methods (/ )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "Methods.odcl")
  (dcl_FORM_SHOW Methods_DemoMethods)
  ;; The Event handlers manage the form here.
  (PRINC)
)



(defun c:Methods_DemoMethods_cmdClose_OnClicked ( /)
   (dcl_Form_Close Methods_DemoMethods)
)


(defun PopulateManipulationTabFields (/ curPos tabSize)
   (setq
      curPos           (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
      nLeft	       (car curPos)
      nTop	       (cadr curPos)
      nWidth	       (caddr curPos)
      nHeight	       (cadddr curPos)
      tabSize          (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
      BottomFromBottom (- (cadr tabSize) nTop nHeight)
      RightFromRight   (- (car tabSize) nLeft nWidth)
      TopFromBottom    (- (cadr tabSize) nTop)
      LeftFromRight    (- (car tabSize) nLeft)
   )
	
   (dcl_Control_SetText Methods_DemoMethods_BottomFromBottom (itoa BottomFromBottom))
   (dcl_Control_SetText Methods_DemoMethods_RightFromRight (itoa RightFromRight))
   (dcl_Control_SetText Methods_DemoMethods_TopFromBottom (itoa TopFromBottom))
   (dcl_Control_SetText Methods_DemoMethods_LeftFromRight (itoa LeftFromRight))

   (dcl_Control_SetText Methods_DemoMethods_LeftBox (itoa nLeft))
   (dcl_Control_SetText Methods_DemoMethods_TopBox (itoa nTop))
   (dcl_Control_SetText Methods_DemoMethods_WidthBox (itoa nWidth))
   (dcl_Control_SetText Methods_DemoMethods_HeightBox (itoa nHeight))
)


(defun c:DemoMethods_TabControl_Changed	(nSelIndex / BottomFromBottom RightFromRight TopFromBottom LeftFromRight nLeft nTop nWidth nHeight)
   
   (if (= nSelIndex 1);_ Sliders Tab
      (progn
	 (dcl_Control_SetValue Methods_DemoMethods_AngleSlider1 (atoi (dcl_Control_GetText Methods_DemoMethods_AngleBox)))
         (dcl_Control_SetValue Methods_DemoMethods_ScrollBar1 (atoi (dcl_Control_GetText Methods_DemoMethods_VScrollLabel)))
         (dcl_Control_SetValue Methods_DemoMethods_SliderBar1 (atoi (dcl_Control_GetText Methods_DemoMethods_VSliderLabel)))
         (dcl_Control_SetValue Methods_DemoMethods_ScrollBar2 (atoi (dcl_Control_GetText Methods_DemoMethods_HScrollLabel)))
         (dcl_Control_SetValue Methods_DemoMethods_SliderBar2 (atoi (dcl_Control_GetText Methods_DemoMethods_HSliderLabel)))
         (dcl_Control_SetText Methods_DemoMethods_SpinnedBox (itoa (dcl_Control_GetValue Methods_DemoMethods_SpinButton1)))
      )
   );_ if
  
   (if (= nSelIndex 2);_ Manipulation Tab
      (PopulateManipulationTabFields)
   ); _if
)


;; ------------------- PictureBox Tab -------------------------------

(defun c:DemoMethods_DrawText_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (dcl_PictureBox_DrawText Methods_DemoMethods_PictureBox
      (List (List 10 10 -19 20 "Line 1" "TL")
	(List 10 25 -19 30 "Line 2" "TL")
	(List 10 40 -19 40 "Line 3" "TL")
	(List 10 55 -19 50 "Line 4" "TL" T)
	(List 10 70 -19 56 "Line 5" "TL" T)
      )
   )
)

(defun c:DemoMethods_DrawWrappedText_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (dcl_PictureBox_DrawWrappedText Methods_DemoMethods_PictureBox
      '((10 10 220 -19 "This sample text is supposed to wrap around if the width of the text is going to exceed the allowed width given." "L")
	(10 70 220 -19 "This sample text is also supposed to wrap around if the width of the text is going to exceed the allowed width given." "C"	)
	(10 130 220 -19 "This sample text is also supposed to wrap around if the width of the text is going to exceed the allowed width given." "R"	)
       )
   )  
)

(defun c:DemoMethods_DrawLine_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (dcl_PictureBox_DrawLine Methods_DemoMethods_PictureBox
      '(
	(10 10 230 10 248)
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
(defun c:DemoMethods_DrawArc_Clicked (/ nBoundingLeft nBoundingTop)
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (setq nBoundingLeft 50)
   (setq nBoundingTop 50)
   ; OpenDCLARX is requires larger lists to be created in a  certain format, ensure you create list in the manor shown below
   ; we have experienced errors when specifying a list as follows
   ; '((nBoundingLeft nBoundingTop ...) (nBoundingLeft nBoundingTop ...)),
   ; For some unknown reason OpenDCLARX cannot handle list created in this manor.
   (setq ArgumentList
	   (list
	      (list
		 nBoundingLeft
		 nBoundingTop
		 25
		 (+ nBoundingLeft 25)
		 nBoundingTop
		 (+ nBoundingLeft 25)
		 (+ nBoundingTop 50)
		 248
	      )
	      (list
		 nBoundingLeft
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
   (dcl_PictureBox_DrawArc Methods_DemoMethods_PictureBox ArgumentList)
)

(defun c:DemoMethods_DrawCircle_Clicked	()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (dcl_PictureBox_DrawCircle Methods_DemoMethods_PictureBox
      '((10 10 140 140 1)
	(10 160 140 40 2)
       )
   )
)

(defun c:DemoMethods_DrawHatchRect_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (dcl_PictureBox_DrawHatchRect Methods_DemoMethods_PictureBox
      '((10 10 40 40 1 1)
	(10 60 40 40 2 2)
	(60 10 40 40 3 3)
	(60 60 40 40 4 4)
	(10 110 40 40 5 5)
       )
   )
)

(defun c:DemoMethods_DrawFillRect_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (dcl_PictureBox_DrawFillRect Methods_DemoMethods_PictureBox
      '((10 10 40 40 1)
	(10 60 40 40 2)
	(60 10 40 40 3)
	(60 60 40 40 4)
	(10 110 40 40 5)
       )
   )
)

(defun c:DemoMethods_PaintPicture_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (setq PictureList
	   (list
	      (list 10 10 102 T T)
	      (list 10 50 105 T T)
	      (list 10 90 106 T T)
	      (list 10 130 107 T T)
	      (list 10 170 108 T T)
	   )
   )
   (dcl_PictureBox_PaintPicture Methods_DemoMethods_PictureBox PictureList)
)

(defun c:DemoMethods_DrawPoints_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (dcl_PictureBox_DrawPoint Methods_DemoMethods_PictureBox 14 10 1)
   (dcl_PictureBox_DrawPoint Methods_DemoMethods_PictureBox 18 10 2)
   (dcl_PictureBox_DrawPoint Methods_DemoMethods_PictureBox 22 10 3)
   (dcl_PictureBox_DrawPoint Methods_DemoMethods_PictureBox 26 10 4)
)

(defun c:DemoMethods_DrawEdge_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
   (dcl_PictureBox_DrawEdge Methods_DemoMethods_PictureBox
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

(defun c:DemoMethods_LoadPictureFile_Clicked (/ sFileName)
   (setq sFileName (GetFiled "Select a picture file" "" "bmp;ico;jpg;gif;wmf" 8))
   (if (/= sFileName nil)
      (dcl_PictureBox_LoadPictureFile Methods_DemoMethods_PictureBox sFileName)
      (dcl_PictureBox_StoreImage Methods_DemoMethods_PictureBox)
   )
)

(defun c:DemoMethods_Clear_Clicked ()
   (dcl_PictureBox_Clear Methods_DemoMethods_PictureBox)
)


;; ------------------- Sliders Tab -------------------------------

(defun c:DemoMethods_ScrollBar1_Scroll (nValue /)
  (dcl_Control_SetText Methods_DemoMethods_VScrollLabel (itoa nValue))
)

(defun c:DemoMethods_ScrollBar2_Scroll (nValue /)
  (dcl_Control_SetText Methods_DemoMethods_HScrollLabel (itoa nValue))
)

(defun c:DemoMethods_AngleSlider1_Scroll (nValue /)
  (dcl_Control_SetText Methods_DemoMethods_AngleBox (itoa nValue))
)

(defun c:DemoMethods_SliderBar2_ReleasedCapture	(nValue /)
  (dcl_Control_SetText Methods_DemoMethods_HSliderLabel (itoa nValue))
)

(defun c:DemoMethods_SliderBar1_ReleasedCapture	(nValue /)
  (dcl_Control_SetText Methods_DemoMethods_VSliderLabel (itoa nValue))
)

(defun c:DemoMethods_SpinButton1_Changed (nValue nDelta/)
  (dcl_Control_SetText Methods_DemoMethods_SpinnedBox (itoa nValue))
)

(defun c:DemoMethods_AngleBox_EditChanged (sText /)
  (dcl_Control_SetValue Methods_DemoMethods_AngleSlider1 (atoi sText))
)

(defun c:DemoMethods_VScrollLabel_EditChanged (sText /)
  (dcl_Control_SetValue Methods_DemoMethods_ScrollBar1 (atoi sText))
)

(defun c:DemoMethods_VSliderLabel_EditChanged (sText /)
  (dcl_Control_SetValue Methods_DemoMethods_SliderBar1 (atoi sText))
)

(defun c:DemoMethods_HScrollLabel_EditChanged (sText /)
  (dcl_Control_SetValue Methods_DemoMethods_ScrollBar2 (atoi sText))
)

(defun c:DemoMethods_HSliderLabel_EditChanged (sText /)
  (dcl_Control_SetValue Methods_DemoMethods_SliderBar2 (atoi sText))
)


(defun c:DemoMethods_SpinnedBox_EditChanged (sText /)
  (dcl_Control_SetValue Methods_DemoMethods_SpinButton1 (atoi sText))
)


;; ------------------- Manipulation Tab -------------------------------

(defun c:DemoMethods_LeftBox_EditChanged (sText / curPos tabSize nVal)
  (setq curPos (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
        tabSize (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
        nVal (atoi sText)
  )
  (dcl_Control_SetLeft Methods_DemoMethods_SampleControl nVal)
  (dcl_Control_SetLeftFromRight Methods_DemoMethods_SampleControl (setq nVal (- (car tabSize) nVal)))
  (dcl_Control_SetText Methods_DemoMethods_LeftFromRight nVal)
)

(defun c:DemoMethods_TopBox_EditChanged	(sText / curPos tabSize nVal)
  (setq curPos (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
        tabSize (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
        nVal (atoi sText)
  )
  (dcl_Control_SetTop Methods_DemoMethods_SampleControl nVal)
  (dcl_Control_SetTopFromBottom Methods_DemoMethods_SampleControl (setq nVal (- (cadr tabSize) nVal)))
  (dcl_Control_SetText Methods_DemoMethods_TopFromBottom nVal)
)

(defun c:DemoMethods_WidthBox_EditChanged (sText / curPos tabSize nVal)
  (setq curPos (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
        tabSize (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
        nVal (atoi sText)
  )
  (dcl_Control_SetWidth Methods_DemoMethods_SampleControl nVal)
  (dcl_Control_SetRightFromRight Methods_DemoMethods_SampleControl (setq nVal (- (car tabSize) nVal (car curPos))))
  (dcl_Control_SetText Methods_DemoMethods_RightFromRight nVal)
)

(defun c:DemoMethods_HeightBox_EditChanged (sText / curPos tabSize nVal)
  (setq curPos (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
        tabSize (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
        nVal (atoi sText)
  )
  (dcl_Control_SetHeight Methods_DemoMethods_SampleControl nVal)
  (dcl_Control_SetBottomFromBottom Methods_DemoMethods_SampleControl (setq nVal (- (cadr tabSize) nVal (cadr curPos))))
  (dcl_Control_SetText Methods_DemoMethods_BottomFromBottom nVal)
)

(defun c:DemoMethods_VisibleCheck_Clicked (nValue /)
  (dcl_Control_SetVisible Methods_DemoMethods_SampleControl (not (zerop nValue)))
)

(defun c:DemoMethods_EnableOption_Clicked (nValue /)
  (dcl_Control_SetEnabled Methods_DemoMethods_SampleControl T)
)

(defun c:DemoMethods_DisableOption_Clicked (nValue /)
  (dcl_Control_SetEnabled Methods_DemoMethods_SampleControl nil)
)

(defun c:DemoMethods_LeftCheck_Clicked (nValue /)
  (dcl_Control_SetUseLeftFromRight Methods_DemoMethods_SampleControl nValue)
)

(defun c:DemoMethods_TopCheck_Clicked (nValue /)
  (dcl_Control_SetUseTopFromBottom Methods_DemoMethods_SampleControl nValue)
)

(defun c:DemoMethods_RightCheck_Clicked	(nValue /)
  (dcl_Control_SetUseRightFromRight Methods_DemoMethods_SampleControl nValue)
)

(defun c:DemoMethods_BottomCheck_Clicked (nValue /)
  (dcl_Control_SetUseBottomFromBottom Methods_DemoMethods_SampleControl nValue)
)

(defun c:DemoMethods_LeftFromRight_EditChanged (sText / curPos tabSize nVal)
  (setq curPos (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
        tabSize (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
        nVal (atoi sText)
  )
  (dcl_Control_SetLeftFromRight Methods_DemoMethods_SampleControl nVal)
  (dcl_Control_SetLeft Methods_DemoMethods_SampleControl (setq nVal (- (car tabSize) nVal)))
  (dcl_Control_SetText Methods_DemoMethods_LeftBox nVal)
)

(defun c:DemoMethods_TopFromBottom_EditChanged (sText / curPos tabSize nVal)
  (setq curPos (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
        tabSize (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
        nVal (atoi sText)
  )
  (dcl_Control_SetTopFromBottom Methods_DemoMethods_SampleControl nVal)
  (dcl_Control_SetTop Methods_DemoMethods_SampleControl (setq nVal (- (cadr tabSize) nVal)))
  (dcl_Control_SetText Methods_DemoMethods_TopBox nVal)
)

(defun c:DemoMethods_RightFromRight_EditChanged (sText / curPos tabSize nVal)
  (setq curPos (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
        tabSize (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
        nVal (atoi sText)
  )
  (dcl_Control_SetRightFromRight Methods_DemoMethods_SampleControl nVal)
  (dcl_Control_SetWidth Methods_DemoMethods_SampleControl (setq nVal (- (car tabSize) nVal (car curPos))))
  (dcl_Control_SetText Methods_DemoMethods_WidthBox nVal)
)

(defun c:DemoMethods_BottomFromBottom_EditChanged (sText / curPos tabSize nVal)
  (setq curPos (dcl_Control_GetCurPos Methods_DemoMethods_SampleControl)
        tabSize (dcl_Tab_GetControlArea Methods_DemoMethods_TabControl)
        nVal (atoi sText)
  )
  (dcl_Control_SetBottomFromBottom Methods_DemoMethods_SampleControl nVal)
  (dcl_Control_SetHeight Methods_DemoMethods_SampleControl (setq nVal (- (cadr tabSize) nVal (cadr curPos))))
  (dcl_Control_SetText Methods_DemoMethods_HeightBox nVal)
)

(defun c:Methods_DemoMethods_OnSize (nWidth nHeight /)
  (PopulateManipulationTabFields)
)

(princ)
