(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL sample programs.\nEnter \"GRD\" to run the sample.\n ")
)


;******************************************************************
(defun c:grd ( / )
  (or LoadRunTime (load "_OpenDclUtils.lsp") (exit))
  (LoadRunTime)
  (LoadODCLProj "GRID.odcl")
  (dcl_FORM_SHOW GRID_Dcl-1)
  ;; The Event handlers manage the form here.
  (princ)
)


;******************************************************************
(defun c:Project1_Dcl-1_TB-Close_OnClicked ()
(dcl_Form_Close GRID_Dcl-1)  
)


;******************************************************************
(defun c:Project1_Dcl-1_OnInitialize ( / GridCellTypeList ImageComboBoxList x nRow)

(defun AddListImageComboBox ($ItemList $CTRL)
  (foreach x $ItemList
    (dcl_ImageComboBox_AddString $CTRL (nth 0 x) (nth 1 x))
  )
)


(defun FillColCountList ( / cnt ColCnt)
;Add totle number of colums to ComboBox
(setq ColCnt (dcl_Grid_GetColumnCount GRID_Dcl-1_grid1))
(setq cnt 1)
(dcl_ComboBox_Clear GRID_Dcl-1_CBx-GridColCount)
(while (< cnt ColCnt)
   (dcl_ComboBox_AddString GRID_Dcl-1_CBx-GridColCount (itoa cnt))
    (setq cnt (1+ cnt))
);_ while
(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridColCount (- ColCnt 2))

)
  
(setq GridCellTypeList (list
	 "0 - Decide at run time"
	 "1 - Check Boxes"
	 "2 - Option Buttons"
	 "3 - Switchable Icons"
	 "4 - Ellipses Buttons"
	 "5 - Pick Buttons"	
	 "6 - Strings"	
	 "7 - AngleUnits"	
	 "8 - Integers"	
	 "9 - Units"		
	"10 - UpperCase"	
	"11 - LowerCase"	
	"12 - Password"	
	"13 - MultiLine"	
	"14 - Currency"	
	"15 - Date"		
	"16 - Time"		
	"17 - Percentage"	
	"18 - DropDown"
	"19 - ArrowHeads"
	"20 - Acad Colors"
	"21 - TextStyle List"
	"22 - PlotStyle Names"
	"23 - PlotStyle Tables"
	"24 - Plotter List"	
	"25 - Fonts"		
	"26 - Drive List"
	"27 - Layer List"	
	"28 - DimStyle List"
	"29 - ImageDrop List"
	"30 - AcadColor Cell"
	"31 - TrueColor Cell"
	"32 - LineWeight Cell"
	"33 - Linetype Cell"	
	"34 - Directories"
	"35 - Files"
	"36 - Strings Combo"
	"37 - AngleUnits Combo"
	"38 - Integers Combo"
	"39 - Units Combo"	
	"40 - UpperCase Combo"
	"41 - LowerCase Combo"

			 )

ImageComboBoxList (list
		    '("---" 100)
		    '("star" 0)
		    '("feet" 1)
		    '("hand" 2)
		    '("moon" 3)
		    )
      
      )


;add list to the ComboBox  
(dcl_ComboBox_AddList GRID_Dcl-1_CBx-GridCellType GridCellTypeList )
;Shows first element from the list
(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridCellType 0)
;add list elements to ImageComboBox
(AddListImageComboBox ImageComboBoxList GRID_Dcl-1_IC-Images)
;Show first element from the list  
(dcl_ImageComboBox_SetCurSel GRID_Dcl-1_IC-Images 0)

;add list elements to ImageComboBox
(AddListImageComboBox ImageComboBoxList GRID_Dcl-1_IC-ColImages)
;Show first element from the list  
(dcl_ImageComboBox_SetCurSel GRID_Dcl-1_IC-ColImages 0)
(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridColJust 0)
   

(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "0\t \tCheck Boxes")))
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 0)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 1)  
  
(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "1\t \tOption Buttons")))
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 1)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 2)
;(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)    
  
(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "2\t \tEllipses Buttons")))
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 2)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 4)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
  
(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "3\t \tPick Buttons")))
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 3)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 5)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)



  ; add an item as Drop Down List
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "4\t \tItem1"))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 18)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
  ; set the drop list
  (dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("Item1" "Item2" "Item3" "Item4"))  


   ; add an item as Image Drop Down List
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "5\t \tstar"))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 29)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)    
  ; set the drop list
  (dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("star" "feet" "hand" "moon") '(0 1 2 3))

  ; add an item as Strings Combo
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "6\t \tStringCombo"))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 36)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)    
  ; set the drop list
  (dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("Item1" "Item2" "Item3" "Item4" ))

  ; add an item as Angle Units Combo
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "7\t \t0" ))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 37)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
  ; set the drop list
  ; note all the drop list items are to be as strings even though we are assigning the Angle Units Combo style
  (dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("0"  "90" "180" "270" ) )


  ; add An item that displays switchable images
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "8\t \tSwitchable Images"))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 3 1 2)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)

  (FillColCountList)
  (princ)
)


;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-AddRow_OnClicked ( / GRIDCELLIMAGEINT GRIDCELLTYPEINT GRIDCOLUMNVALUE GRIDROWCOUNT NROW )
(setq GridCellTypeInt 	(dcl_ComboBox_GetCurSel GRID_Dcl-1_CBx-GridCellType)
      GridCellImageInt 	(dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images)
      GridColumnValue 	(dcl_Control_GetText GRID_Dcl-1_TB-Value)
      GridRowCount 	(dcl_Grid_GetRowCount GRID_Dcl-1_grid1)
      )
; add check box item
  
(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat (itoa GridRowCount) "\t \t" GridColumnValue))) ; \t - separetes columns"
; set the new grid cell style for column 1
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
; check off the new grid cell
(dcl_Grid_SetItemCheck GRID_Dcl-1_grid1 nRow 1 0)

; set the new grid cell style for column 2
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 GridCellTypeInt)

  
(if (>(dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images) 0)
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 2 (1-(dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images)));set the new image to cell for column 2
)  
  
)


(defun c:grid_Dcl-1_TB-AddCol_OnClicked ( / txt cWidth cImg)
    (setq txt (dcl_Control_GetText GRID_Dcl-1_TB-ColTitle))
    (setq cImg (1- (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-ColImages)))
    (if (>= cImg 0)
      (Setq cWidth (+ (dcl_Grid_CalcColWidth GRID_Dcl-1_grid1 txt) 24))
      (Setq cWidth (dcl_Grid_CalcColWidth GRID_Dcl-1_grid1 txt))
    )
    (dcl_Grid_InsertColumn GRID_Dcl-1_grid1 (dcl_Grid_GetColumnCount GRID_Dcl-1_grid1) (list txt (dcl_ComboBox_GetCurSel GRID_Dcl-1_CBx-GridColJust) cWidth (1- (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-ColImages))))
    (FillColCountList)
    (princ)
)


(defun c:grid_Dcl-1_TB-DeleteColumn_OnClicked ( / DelCol)
  (Setq DelCol (1+ (dcl_ComboBox_GetCurSel GRID_Dcl-1_CBx-GridColCount)))
  (dcl_Grid_DeleteColumn GRID_Dcl-1_grid1 DelCol)
  (FillColCountList)
  (princ)
)



;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-SelectAll_OnClicked ( / GridRowCount)
(setq  GridRowCount (dcl_Grid_GetRowCount GRID_Dcl-1_grid1))
(while (>= GridRowCount 0)
(dcl_Grid_SetItemCheck GRID_Dcl-1_grid1 GridRowCount 1 1)
(setq GridRowCount (1- GridRowCount))  
  )
)

;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-ClearAll_OnClicked ()
(setq  GridRowCount (dcl_Grid_GetRowCount GRID_Dcl-1_grid1))
(while (>= GridRowCount 0)
(dcl_Grid_SetItemCheck GRID_Dcl-1_grid1 GridRowCount 1 0)
(setq GridRowCount (1- GridRowCount))  
  )
)

;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-DeleteRows_OnClicked ()
    (setq GridRowCount (dcl_Grid_GetRowCount GRID_Dcl-1_grid1))
    (while (>= GridRowCount 0)
        (if (= (dcl_Grid_GetItemCheck GRID_Dcl-1_grid1 GridRowCount 1) T)
            (dcl_Grid_DeleteRow GRID_Dcl-1_grid1 GridRowCount)
        )
        (setq GridRowCount (1- GridRowCount))
    )
)



;EVENTS
(defun c:Project1_Dcl-1_grid1_OnBeginLabelEdit (nRow nCol / newValue)
(setq newValue
(strcat
"Event: OnBeginLabelEdit
Arguments: nRow: " (itoa nRow) " ; nCol: " (itoa nCol))
      )
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)


(defun c:Project1_Dcl-1_grid1_OnButtonClicked (nRow nCol / newValue)
(setq newValue
(strcat
"Event: OnButtonClicked
Arguments: nRow: " (itoa nRow) " ; nCol: " (itoa nCol))
      )
 
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)

(defun c:Project1_Dcl-1_grid1_OnSelChanged (nRow nCol / newValue)
(setq newValue
(strcat
"Event: OnSelChanged
Arguments: nRow: " (itoa nRow) " ; nCol: " (itoa nCol))
      )
 
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)


(defun c:Project1_Dcl-1_grid1_OnEndLabelEdit (nRow nCol / newValue)
(setq newValue
(strcat
"Event: OnEndLabelEdit
Arguments: nRow: " (itoa nRow) " ; nCol: " (itoa nCol))
      )
 
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)


(defun c:Project1_Dcl-1_grid1_OnColumnClick (nColumn / newValue)
(setq newValue
(strcat
"Event: OnColumnClick
Arguments: nColumn: " (itoa nColumn))
      )
 
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)

(princ)

