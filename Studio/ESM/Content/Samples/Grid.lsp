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
	(dcl-Project-Load (*ODCL:Samples:FindFile "Grid.odcl"))

	;; Show the main form
	(dcl-Form-Show Grid/Dcl-1)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|<<OpenDCL Event Handlers>>|;

;;;******************************************************************
(defun c:Project1_Dcl-1_TB-Close_OnClicked () (dcl-Form-Close Grid/Dcl-1))


;;;******************************************************************
(defun c:Project1_Dcl-1_OnInitialize
	   (/ GridCellTypeList ImageComboBoxList x nRow)
	(defun AddListImageComboBox ($ItemList $CTRL)
		(foreach x $ItemList
			(dcl-ImageComboBox-AddString $CTRL (nth 0 x) (nth 1 x))
		)
	)
	(defun FillColCountList (/ cnt ColCnt)
										;Add total number of colums to ComboBox
		(setq ColCnt (dcl-Grid-GetColumnCount Grid/Dcl-1/grid1))
		(setq cnt 1)
		(dcl-ComboBox-Clear Grid/Dcl-1/CBx-GridColCount)
		(while (< cnt ColCnt)
			(dcl-ComboBox-AddString Grid/Dcl-1/CBx-GridColCount (itoa cnt))
			(setq cnt (1+ cnt))
		) ;_ while
		(dcl-ComboBox-SetCurSel Grid/Dcl-1/CBx-GridColCount (- ColCnt 2))
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

	(dcl-ComboBox-Clear Grid/Dcl-1/CBx-GridCellType)

        (foreach lstItem GridCellTypeList
            (setq nRow (dcl-ComboBox-AddString Grid/Dcl-1/CBx-GridCellType (strcat (itoa (car lstItem)) " - " (cdr lstItem))))
            (dcl-ComboBox-setitemdata Grid/Dcl-1/CBx-GridCellType nRow (car lstItem))
        ); foreach

	(dcl-ComboBox-SetCurSel Grid/Dcl-1/CBx-GridCellType 0)			; Show first element from the list

	(AddListImageComboBox ImageComboBoxList Grid/Dcl-1/IC-Images)		; Add list elements to ImageComboBox

	(dcl-ImageComboBox-SetCurSel Grid/Dcl-1/IC-Images 0)			; Show first element from the list

	(AddListImageComboBox ImageComboBoxList Grid/Dcl-1/IC-ColImages)	; Add list elements to ImageComboBox

	(dcl-ImageComboBox-SetCurSel Grid/Dcl-1/IC-ColImages 0)			; Show first element from the list

	(dcl-ComboBox-SetCurSel Grid/Dcl-1/CBx-GridColJust 0)			; Show first element from the list

	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1
								   (strcat "0\t \tCheck Boxes")
			   )
	)
	(dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 1 0)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 1)

	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1
								   (strcat "1\t \tOption Buttons")
			   )
	)
	(dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 1 1)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 2)

	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1
								   (strcat "2\t \tEllipses Buttons")
			   )
	)
	(dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 1 2)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 4)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)

	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 
								   (strcat "3\t \tPick Buttons")
			   )
	)
	(dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 1 3)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 5)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)
									
	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "4\t \tItem1"))
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 18)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)		
	(dcl-Grid-SetItemDropList Grid/Dcl-1/grid1 nRow 2 '("Item1" "Item2" "Item3" "Item4"))
	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "5\t \tStar"))
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 29)
	(dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 2 0)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)
	(dcl-Grid-SetItemDropList Grid/Dcl-1/grid1 nRow 2
							  '("Star" "Feet" "Hand" "Moon")
							  '(0 1 2 3))
							 
          
	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "6\t \tStringCombo"))
							
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 36)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)
	(dcl-Grid-SetItemDropList Grid/Dcl-1/grid1 nRow 2
                               '("Item1" "Item2" "Item3" "Item4"))


	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "7\t \t0"))
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 37)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)

	(dcl-Grid-SetItemDropList Grid/Dcl-1/grid1 nRow 2 '("0" "90" "180" "270"))
										
	(Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "8\t \tSwitchable Images"))
										
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 3 1 2)
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)
	(FillColCountList)
	(dcl-ListBox-Clear Grid/Dcl-1/LB-Status)
	(princ)
)


;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-AddRow_OnClicked
	   (/ GRIDCELLIMAGEINT GRIDCELLTYPEINT GRIDCOLUMNVALUE GRIDROWCOUNT NROW)
	(setq GridCellTypeInt  (dcl-ComboBox-GetCurSel Grid/Dcl-1/CBx-GridCellType)
              GridCellTypeVal  (dcl-ComboBox-getitemdata Grid/Dcl-1/CBx-GridCellType GridCellTypeInt)
              GridCellImageInt (dcl-ImageComboBox-GetCurSel Grid/Dcl-1/IC-Images)
              GridColumnValue  (dcl-Control-GetText Grid/Dcl-1/TB-Value)
              GridRowCount     (dcl-Grid-GetRowCount Grid/Dcl-1/grid1)
	)

	(Setq nRow (dcl-Grid-AddString
				   Grid/Dcl-1/grid1
				   (strcat (itoa GridRowCount) "\t \t" GridColumnValue)
				) ; \t - separates columns"
	)

	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)			; Set the new grid cell style for column 1
	(dcl-Grid-SetItemCheck Grid/Dcl-1/grid1 nRow 1 0)			; Check off the new grid cell
	(dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 GridCellTypeVal)		; Set the new grid cell style for column 2
	(if (> (dcl-ImageComboBox-GetCurSel Grid/Dcl-1/IC-Images) 0)		; Set the new image to cell for column 2
		(dcl-Grid-SetItemImage
			Grid/Dcl-1/grid1
			nRow
			2
			(1- (dcl-ImageComboBox-GetCurSel Grid/Dcl-1/IC-Images))
		)
	)
)


(defun c:Grid/Dcl-1/TB-AddCol#OnClicked (/ txt cWidth cImg)
	(setq txt (dcl-Control-GetText Grid/Dcl-1/TB-ColTitle))
	(setq cImg (1- (dcl-ImageComboBox-GetCurSel Grid/Dcl-1/IC-ColImages)))
	(Setq cWidth (+ (dcl-Grid-CalcColWidth Grid/Dcl-1/grid1 txt) (if (>= cImg 0) 48 12)))
	(dcl-Grid-InsertColumn
		Grid/Dcl-1/grid1
		(dcl-Grid-GetColumnCount Grid/Dcl-1/grid1)
		(list txt
			  (dcl-ComboBox-GetCurSel Grid/Dcl-1/CBx-GridColJust)
			  cWidth
			  (1- (dcl-ImageComboBox-GetCurSel Grid/Dcl-1/IC-ColImages))
		)
	)
	(FillColCountList)
	(princ)
)


(defun c:Grid/Dcl-1/TB-DeleteColumn#OnClicked (/ DelCol)
	(Setq DelCol (1+ (dcl-ComboBox-GetCurSel Grid/Dcl-1/CBx-GridColCount)))
	(dcl-Grid-DeleteColumn Grid/Dcl-1/grid1 DelCol)
	(FillColCountList)
	(princ)
)



;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-SelectAll_OnClicked (/ GridRowCount)
	(setq GridRowCount (dcl-Grid-GetRowCount Grid/Dcl-1/grid1))
	(while (>= GridRowCount 0)
		(dcl-Grid-SetItemCheck Grid/Dcl-1/grid1 GridRowCount 1 1)
		(setq GridRowCount (1- GridRowCount))
	)
)

;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-ClearAll_OnClicked ()
	(setq GridRowCount (dcl-Grid-GetRowCount Grid/Dcl-1/grid1))
	(while (>= GridRowCount 0)
		(dcl-Grid-SetItemCheck Grid/Dcl-1/grid1 GridRowCount 1 0)
		(setq GridRowCount (1- GridRowCount))
	)
)

;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-DeleteRows_OnClicked ()
	(setq GridRowCount (dcl-Grid-GetRowCount Grid/Dcl-1/grid1))
	(while (>= GridRowCount 0)
		(if (= (dcl-Grid-GetCellCheckState Grid/Dcl-1/grid1 GridRowCount 1) 1)
			(dcl-Grid-DeleteRow Grid/Dcl-1/grid1 GridRowCount)
		)
		(setq GridRowCount (1- GridRowCount))
	)
)



(defun c:Grid/Dcl-1/grid1#OnBeginLabelEdit (nRow nCol / newValue)
	(setq newValue (strcat "Event: OnBeginLabelEdit
Arguments: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue))
)




(defun c:Grid/Dcl-1/grid1#OnButtonClicked (nRow nCol / newValue)
	(setq newValue (strcat "Event: OnButtonClicked
Arguments: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue))
)

(defun c:Grid/Dcl-1/grid1#OnSelChanged (nRow nCol / newValue)
	(setq newValue (strcat "Event: OnSelChanged
Arguments: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue))
)


(defun c:Grid/Dcl-1/grid1#OnEndLabelEdit (nRow nCol / newValue)
	(setq newValue (strcat "Event: OnEndLabelEdit
Arguments: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(if Grid/Dcl-1/LB-Status (dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue)))
)


(defun c:Grid/Dcl-1/grid1#OnColumnClick (nColumn / newValue)
	(setq newValue (strcat "Event: OnColumnClick
Arguments: nColumn: "      (itoa nColumn)
				   )
	)
	(dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue))
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
