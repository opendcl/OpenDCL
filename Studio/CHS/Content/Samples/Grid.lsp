;;;
;;; GRID Sample
;;;
;;; This sample demonstrates the Grid control and all its events.
;;;

;; Main program
(defun c:Grd (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl_Project_Load (*ODCL:Samples:FindFile "Grid.odcl"))

	;; Show the main form
	(dcl_Form_Show GRID_Dcl-1)

	;; This is a modal form, so (dcl_Form_Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

;;;******************************************************************
(defun c:Project1_Dcl-1_TB-Close_OnClicked () (dcl_Form_Close GRID_Dcl-1))


;;;******************************************************************
(defun c:Project1_Dcl-1_OnInitialize
	   (/ GridCellTypeList ImageComboBoxList x nRow)
	(defun AddListImageComboBox ($ItemList $CTRL)
		(foreach x $ItemList
			(dcl_ImageComboBox_AddString $CTRL (nth 0 x) (nth 1 x))
		)
	)
	(defun FillColCountList (/ cnt ColCnt)
										;Add total number of colums to ComboBox
		(setq ColCnt (dcl_Grid_GetColumnCount GRID_Dcl-1_grid1))
		(setq cnt 1)
		(dcl_ComboBox_Clear GRID_Dcl-1_CBx-GridColCount)
		(while (< cnt ColCnt)
			(dcl_ComboBox_AddString GRID_Dcl-1_CBx-GridColCount (itoa cnt))
			(setq cnt (1+ cnt))
		) ;_ while
		(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridColCount (- ColCnt 2))
	)
	(setq GridCellTypeList '(
								(-1 . "Undefined")
								(0 . "Decide at run time")
								(1 . "Check Boxes")
								(2 . "Option Buttons")
								(3 . "Switchable Icons")
								(4 . "Ellipses Buttons")
								(5 . "Pick Buttons")
								(6 . "Strings")
								(7 . "AngleUnits")
								(8 . "Integers")
								(9 . "Units")
								(10 . "UpperCase")
								(11 . "LowerCase")
								(12 . "Password")
								(13 . "MultiLine")
								(14 . "Currency")
								(15 . "Date")
								(16 . "Time")
								(17 . "Percentage")
								(18 . "DropDown")
								(19 . "ArrowHeads")
								(20 . "Acad Colors")
								(21 . "TextStyle List")
								(22 . "PlotStyle Names")
								(23 . "PlotStyle Tables")
								(24 . "Plotter List")
								(25 . "Fonts")
								(26 . "Drive List")
								(27 . "Layer List")
								(28 . "DimStyle List")
								(29 . "ImageDrop List")
								(30 . "AcadColor Cell")
								(31 . "TrueColor Cell")
								(32 . "LineWeight Cell")
								(33 . "Linetype Cell")
								(34 . "Directories")
								(35 . "Files")
								(36 . "Strings Combo")
								(37 . "AngleUnits Combo")
								(38 . "Integers Combo")
								(39 . "Units Combo")
								(40 . "UpperCase Combo")
								(41 . "LowerCase Combo")
								(42 . "Symbol Name")
								(43 . "Symbol Name Combo")
							)
	      ImageComboBoxList (list '("---" 100) '("Star" 0) '("Feet" 1) '("Hand" 2) '("Moon" 3))) ; Add list to the ComboBox

	(dcl_ComboBox_Clear GRID_Dcl-1_CBx-GridCellType)

        (foreach lstItem GridCellTypeList
            (setq nRow (dcl_ComboBox_AddString GRID_Dcl-1_CBx-GridCellType (strcat (itoa (car lstItem)) " - " (cdr lstItem))))
            (dcl_combobox_setitemdata GRID_Dcl-1_CBx-GridCellType nRow (car lstItem))
        ); foreach

	(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridCellType 0)			; Show first element from the list

	(AddListImageComboBox ImageComboBoxList GRID_Dcl-1_IC-Images)		; Add list elements to ImageComboBox

	(dcl_ImageComboBox_SetCurSel GRID_Dcl-1_IC-Images 0)			; Show first element from the list

	(AddListImageComboBox ImageComboBoxList GRID_Dcl-1_IC-ColImages)	; Add list elements to ImageComboBox

	(dcl_ImageComboBox_SetCurSel GRID_Dcl-1_IC-ColImages 0)			; Show first element from the list

	(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridColJust 0)			; Show first element from the list

	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1
								   (strcat "0\t \tCheck Boxes")
			   )
	)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 0)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 1)

	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1
								   (strcat "1\t \tOption Buttons")
			   )
	)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 1)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 2)

	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1
								   (strcat "2\t \tEllipses Buttons")
			   )
	)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 2)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 4)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)

	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 
								   (strcat "3\t \tPick Buttons")
			   )
	)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 3)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 5)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
									
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "4\t \tItem1"))
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 18)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)		
	(dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("Item1" "Item2" "Item3" "Item4"))
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "5\t \tStar"))
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 29)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 2 0)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
	(dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2
							  '("Star" "Feet" "Hand" "Moon")
							  '(0 1 2 3))
							 
          
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "6\t \tStringCombo"))
							
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 36)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
	(dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2
                               '("Item1" "Item2" "Item3" "Item4"))


	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "7\t \t0"))
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 37)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)

	(dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("0" "90" "180" "270"))
										
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "8\t \tSwitchable Images"))
										
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 3 1 2)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
	(FillColCountList)
	(dcl_ListBox_Clear Grid_Dcl-1_LB-Status)
	(princ)
)


;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-AddRow_OnClicked
	   (/ GRIDCELLIMAGEINT GRIDCELLTYPEINT GRIDCOLUMNVALUE GRIDROWCOUNT NROW)
	(setq GridCellTypeInt  (dcl_ComboBox_GetCurSel GRID_Dcl-1_CBx-GridCellType)
              GridCellTypeVal  (dcl_combobox_getitemdata GRID_Dcl-1_CBx-GridCellType GridCellTypeInt)
              GridCellImageInt (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images)
              GridColumnValue  (dcl_Control_GetText GRID_Dcl-1_TB-Value)
              GridRowCount     (dcl_Grid_GetRowCount GRID_Dcl-1_grid1)
	)

	(Setq nRow (dcl_Grid_AddString
				   GRID_Dcl-1_grid1
				   (strcat (itoa GridRowCount) "\t \t" GridColumnValue)
				) ; \t - separates columns"
	)

	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)			; Set the new grid cell style for column 1
	(dcl_Grid_SetItemCheck GRID_Dcl-1_grid1 nRow 1 0)			; Check off the new grid cell
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 GridCellTypeVal)		; Set the new grid cell style for column 2
	(if (> (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images) 0)		; Set the new image to cell for column 2
		(dcl_Grid_SetItemImage
			GRID_Dcl-1_grid1
			nRow
			2
			(1- (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images))
		)
	)
)


(defun c:grid_Dcl-1_TB-AddCol_OnClicked (/ txt cWidth cImg)
	(setq txt (dcl_Control_GetText GRID_Dcl-1_TB-ColTitle))
	(setq cImg (1- (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-ColImages)))
	(Setq cWidth (+ (dcl_Grid_CalcColWidth GRID_Dcl-1_grid1 txt) (if (>= cImg 0) 48 12)))
	(dcl_Grid_InsertColumn
		GRID_Dcl-1_grid1
		(dcl_Grid_GetColumnCount GRID_Dcl-1_grid1)
		(list txt
			  (dcl_ComboBox_GetCurSel GRID_Dcl-1_CBx-GridColJust)
			  cWidth
			  (1- (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-ColImages))
		)
	)
	(FillColCountList)
	(princ)
)


(defun c:grid_Dcl-1_TB-DeleteColumn_OnClicked (/ DelCol)
	(Setq DelCol (1+ (dcl_ComboBox_GetCurSel GRID_Dcl-1_CBx-GridColCount)))
	(dcl_Grid_DeleteColumn GRID_Dcl-1_grid1 DelCol)
	(FillColCountList)
	(princ)
)



;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-SelectAll_OnClicked (/ GridRowCount)
	(setq GridRowCount (dcl_Grid_GetRowCount GRID_Dcl-1_grid1))
	(while (>= GridRowCount 0)
		(dcl_Grid_SetItemCheck GRID_Dcl-1_grid1 GridRowCount 1 1)
		(setq GridRowCount (1- GridRowCount))
	)
)

;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-ClearAll_OnClicked ()
	(setq GridRowCount (dcl_Grid_GetRowCount GRID_Dcl-1_grid1))
	(while (>= GridRowCount 0)
		(dcl_Grid_SetItemCheck GRID_Dcl-1_grid1 GridRowCount 1 0)
		(setq GridRowCount (1- GridRowCount))
	)
)

;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-DeleteRows_OnClicked ()
	(setq GridRowCount (dcl_Grid_GetRowCount GRID_Dcl-1_grid1))
	(while (>= GridRowCount 0)
		(if (= (dcl_Grid_GetCellCheckState GRID_Dcl-1_grid1 GridRowCount 1) 1)
			(dcl_Grid_DeleteRow GRID_Dcl-1_grid1 GridRowCount)
		)
		(setq GridRowCount (1- GridRowCount))
	)
)



(defun c:Grid_Dcl-1_grid1_OnBeginLabelEdit (nRow nCol / newValue)
	(setq newValue (strcat "Event: OnBeginLabelEdit
Arguments: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl_ListBox_SetTopIndex Grid_Dcl-1_LB-Status (dcl_ListBox_AddString Grid_Dcl-1_LB-Status newValue))
)




(defun c:Grid_Dcl-1_grid1_OnButtonClicked (nRow nCol / newValue)
	(setq newValue (strcat "Event: OnButtonClicked
Arguments: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl_ListBox_SetTopIndex Grid_Dcl-1_LB-Status (dcl_ListBox_AddString Grid_Dcl-1_LB-Status newValue))
)

(defun c:Grid_Dcl-1_grid1_OnSelChanged (nRow nCol / newValue)
	(setq newValue (strcat "Event: OnSelChanged
Arguments: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl_ListBox_SetTopIndex Grid_Dcl-1_LB-Status (dcl_ListBox_AddString Grid_Dcl-1_LB-Status newValue))
)


(defun c:Grid_Dcl-1_grid1_OnEndLabelEdit (nRow nCol / newValue)
	(setq newValue (strcat "Event: OnEndLabelEdit
Arguments: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(if Grid_Dcl-1_LB-Status (dcl_ListBox_SetTopIndex Grid_Dcl-1_LB-Status (dcl_ListBox_AddString Grid_Dcl-1_LB-Status newValue)))
)


(defun c:Grid_Dcl-1_grid1_OnColumnClick (nColumn / newValue)
	(setq newValue (strcat "Event: OnColumnClick
Arguments: nColumn: "      (itoa nColumn)
				   )
	)
	(dcl_ListBox_SetTopIndex Grid_Dcl-1_LB-Status (dcl_ListBox_AddString Grid_Dcl-1_LB-Status newValue))
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
	"Grd"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
