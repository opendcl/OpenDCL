;;This sample demonstrates how to move or resize a form after it's been shown.
;;It uses (dcl_DelayedInvoke) to wait a few milliseconds before moving the form to appear animated.

(IF (NOT *MasterDemo*)
    (princ "\nOpenDCL sample programs.\nEnter \"MOVER\" to run the sample.\n")
)

(defun c:FormMover_Form1_GraphicButton1_OnClicked ( /)
  (setq *FormPos* 0)
  (C:FormMover)
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
    ((= *FormPos* 0)
      (dcl_Form_SetPos FormMover_Form1 0 0);_TL
    )
    ((= *FormPos* 1)
      (dcl_Form_SetPos FormMover_Form1 0 (- ScreenY ht));_ BL
    )
    ((= *FormPos* 2)
      (dcl_Form_SetPos FormMover_Form1 (- ScreenX wd) (- ScreenY ht));_ BR
    )
    ((= *FormPos* 3)
      (dcl_Form_SetPos FormMover_Form1 (- ScreenX wd) 0);_ TR
    )
    ((= *FormPos* 4)
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


(defun C:Mover ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "FormMover.odcl")
  (dcl_Form_Show FormMover_Form1)
  (dcl_Form_Resize FormMover_Form1 300 200)
  (dcl_Form_Center FormMover_Form1)
  (c:FormMover_Form1_GraphicButton1_OnClicked)
  (princ)
)
(princ)