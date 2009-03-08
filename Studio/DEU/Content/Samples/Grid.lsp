(IF (NOT *MasterDemo*)
  (princ "\nOpenDCL Beispiel-Programm.\nGeben Sie \"GRD\" ein, um das Beispiel zu starten.\n")
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
	 "0 - Nicht editierbar"
	 "1 - Kontrollkästchen"
	 "2 - Optionsfelder"
	 "3 - Schaltbare Bilder"
	 "4 - Schaltflächen"
	 "5 - Auswahl-Schaltflächen"
	 "6 - Texte"
	 "7 - Winkel" 
	 "8 - Ganzzahlen"
	 "9 - Dezimalzahlen"
	 "10 - Großbuchstaben"
	 "11 - Kleinbuchstaben" 
	 "12 - Passwort"
	 "13 - Mehrzeiliger Text"
	 "14 - Währung"
	 "15 - Datum"
	 "16 - Zeit"
	 "17 - Prozent"
	 "18 - Auswahlliste"
	 "19 - Pfeilspitzen"
	 "20 - ACI-Farbenliste"
	 "21 - Textstile"
	 "22 - Plotstilnamen"
	 "23 - Plotstiltabellen"
	 "24 - Plotter"
	 "25 - Schriftartenliste"
	 "26 - Laufwerke"
	 "27 - Layer"
	 "28 - Bemaßungsstile"
	 "29 - Bilderliste"
	 "30 - ACI-Farben"
	 "31 - RGB-Farben"
	 "32 - Linienstärken"
	 "33 - Linientypen"
	 "34 - Ordner"
	 "35 - Dateien"
	 "36 - Text-Kombinationsfeld"
	 "37 - Winkel-Kombinationsfeld"
	 "38 - Ganzzahlen-Kombinationsfeld"
	 "39 - Dezimalzahlen-Kombinationsfeld"
	 "40 - Großbuchstaben-Kombinationsfeld"
	 "41 - Kleinbuchstaben-Kombinationsfeld"
	 "42 - Symbolnamen"
	 "43 - Symbolnamen-Kombinationsfeld"
			 )

ImageComboBoxList (list
		    '("---" 100)
		    '("Stern" 0)
		    '("Fuß" 1)
		    '("Hand" 2)
		    '("Mond" 3)
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
   

(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "0\t \tKontrollkästchen")))
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 0)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 1)  
  
(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "1\t \tOptionsfeld")))
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 1)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 2)
;(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)    
  
(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "2\t \tSchaltfläche")))
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 2)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 4)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
  
(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "3\t \tAuswahl-Schaltfläche")))
(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 3)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 5)
(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)



  ; add an item as Drop Down List
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "4\t \tElement1"))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 18)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
  ; set the drop list
  (dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("Element1" "Element2" "Element3" "Element4"))  


   ; add an item as Image Drop Down List
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "5\t \tStern"))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 29)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)    
  ; set the drop list
  (dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("Stern" "Fuß" "Hand" "Mond") '(0 1 2 3))

  ; add an item as Strings Combo
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "6\t \tAuswahlliste mit Texten"))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 36)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)    
  ; set the drop list
  (dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("Element1" "Element2" "Element3" "Element4" ))

  ; add an item as Angle Units Combo
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "7\t \t0" ))
  ; set the new grid cell style.
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 37)
  (dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
  ; set the drop list
  ; note all the drop list items are to be as strings even though we are assigning the Angle Units Combo style
  (dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("0"  "90" "180" "270" ) )


  ; add An item that displays switchable images
  (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "8\t \tSchaltbare Bilder"))
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
"Ereignis: OnBeginLabelEdit
Argumente: nRow: " (itoa nRow) " ; nCol: " (itoa nCol))
      )
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)


(defun c:Project1_Dcl-1_grid1_OnButtonClicked (nRow nCol / newValue)
(setq newValue
(strcat
"Ereignis: OnButtonClicked
Argumente: nRow: " (itoa nRow) " ; nCol: " (itoa nCol))
      )
 
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)

(defun c:Project1_Dcl-1_grid1_OnSelChanged (nRow nCol / newValue)
(setq newValue
(strcat
"Ereignis: OnSelChanged
Argumente: nRow: " (itoa nRow) " ; nCol: " (itoa nCol))
      )
 
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)


(defun c:Project1_Dcl-1_grid1_OnEndLabelEdit (nRow nCol / newValue)
(setq newValue
(strcat
"Ereignis: OnEndLabelEdit
Argumente: nRow: " (itoa nRow) " ; nCol: " (itoa nCol))
      )
 
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)


(defun c:Project1_Dcl-1_grid1_OnColumnClick (nColumn / newValue)
(setq newValue
(strcat
"Ereignis: OnColumnClick
Argumente: nColumn: " (itoa nColumn))
      )
 
(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue )
)

(princ)

