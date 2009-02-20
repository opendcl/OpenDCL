(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL sample programs.\nEnter \"SPLITTER\" to run the sample.\n ")
)


(defun c:splitter (/ set_style set_thickness c:splitter_splitter_pb_1_OnClicked
		     c:splitter_splitter_pb_2_OnClicked c:splitter_splitter_pb_3_OnClicked
		     c:splitter_splitter_pb_4_OnClicked c:splitter_splitter_pb_5_OnClicked
		     c:splitter_splitter_pb_6_OnClicked c:splitter_splitter_pb_8_OnClicked
		     c:splitter_splitter_pb_10_OnClicked c:splitter_splitter_pb_12_OnClicked
		     c:splitter_splitter_pb_raised_OnClicked c:splitter_splitter_pb_doubled_OnClicked
		     c:splitter_splitter_pb_etched_OnClicked c:splitter_splitter_pb_flat_OnClicked
		     c:splitter_splitter_pb_sunken_OnClicked c:splitter_splitter_pb_bump_OnClicked)
  
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit)) 
  (LoadRunTime)
  (LoadODCLProj "splitter.odcl")

  (defun set_style (n)
    (dcl_Control_SetEdgeStyle splitter_splitter_spl_hor n)
    (dcl_Control_SetEdgeStyle splitter_splitter_spl_ver n)
  ); set_style

  (defun set_thickness (n)
    (dcl_Control_SetHeight splitter_splitter_spl_hor n)
    (dcl_Control_SetWidth splitter_splitter_spl_ver n)
  ); set_thickness

  (defun c:splitter_splitter_pb_1_OnClicked ( /) (set_thickness 1))
  (defun c:splitter_splitter_pb_2_OnClicked ( /) (set_thickness 2))
  (defun c:splitter_splitter_pb_3_OnClicked ( /) (set_thickness 3))
  (defun c:splitter_splitter_pb_4_OnClicked ( /) (set_thickness 4))
  (defun c:splitter_splitter_pb_5_OnClicked ( /) (set_thickness 5))
  (defun c:splitter_splitter_pb_6_OnClicked ( /) (set_thickness 6))
  (defun c:splitter_splitter_pb_8_OnClicked ( /) (set_thickness 8))
  (defun c:splitter_splitter_pb_10_OnClicked ( /) (set_thickness 10))
  (defun c:splitter_splitter_pb_12_OnClicked ( /) (set_thickness 12))

  (defun c:splitter_splitter_pb_raised_OnClicked ( /) (set_style 0))
  (defun c:splitter_splitter_pb_doubled_OnClicked ( /) (set_style 1))
  (defun c:splitter_splitter_pb_etched_OnClicked ( /) (set_style 2))
  (defun c:splitter_splitter_pb_flat_OnClicked ( /) (set_style 3))
  (defun c:splitter_splitter_pb_sunken_OnClicked ( /) (set_style 4))
  (defun c:splitter_splitter_pb_bump_OnClicked ( /) (set_style 5))

  (dcl_form_show splitter_splitter)

  (princ)
); splitter
(princ)