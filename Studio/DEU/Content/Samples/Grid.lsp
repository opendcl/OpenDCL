;;;
;;; Datenblatt Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung des Datenblatts und seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:Grd (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "Grid.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show GRID_Dcl-1)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

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
										; Füge die Gesamtzahl an Spalten zur Auswahlliste
		(setq ColCnt (dcl_Grid_GetColumnCount GRID_Dcl-1_grid1))
		(setq cnt 1)
		(dcl_ComboBox_Clear GRID_Dcl-1_CBx-GridColCount)
		(while (< cnt ColCnt)
			(dcl_ComboBox_AddString GRID_Dcl-1_CBx-GridColCount (itoa cnt))
			(setq cnt (1+ cnt))
		) ;_ while
		(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridColCount (- ColCnt 2))
	)
	(setq GridCellTypeList  (list
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
		  ImageComboBoxList (list '("---" 100)     '("Stern" 0)
								  '("Fuß" 1)      '("Hand" 2)
								  '("Mond" 3)
								 )
	)                                   ; Liste zur Auswahlliste hinzufügen
        (dcl_ComboBox_Clear GRID_Dcl-1_CBx-GridCellType)
	(dcl_ComboBox_AddList GRID_Dcl-1_CBx-GridCellType GridCellTypeList)
										; Ersten Eintrag der Liste auswählen
	(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridCellType 0)
										; Elemente zur Auswahlliste mit Symbolen hinzufügen
	(AddListImageComboBox ImageComboBoxList GRID_Dcl-1_IC-Images)
										; Ersten Eintrag der Liste auswählen
	(dcl_ImageComboBox_SetCurSel GRID_Dcl-1_IC-Images 0)
										; Elemente zur Auswahlliste mit Symbolen hinzufügen
	(AddListImageComboBox ImageComboBoxList GRID_Dcl-1_IC-ColImages)
										; Ersten Eintrag der Liste auswählen
	(dcl_ImageComboBox_SetCurSel GRID_Dcl-1_IC-ColImages 0)
	(dcl_ComboBox_SetCurSel GRID_Dcl-1_CBx-GridColJust 0)
	(Setq nRow
			 (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "0\t \tKontrollkästchen"))
	)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 0)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 1)
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1
								   (strcat "1\t \tOptionsschalter")
			   )
	)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 1)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 2)

        (Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1
								   (strcat "2\t \tSchaltflächen")
			   )
	)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 2)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 4)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
	(Setq nRow
			 (dcl_Grid_AddString GRID_Dcl-1_grid1 (strcat "3\t \tWahlschaltflächen"))
	)
	(dcl_Grid_SetItemImage GRID_Dcl-1_grid1 nRow 1 3)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 5)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
										; Das Element als Auswahlliste hinzufügen
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "4\t \tElement 1"))
										; Neuen Zellenstil setzen
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 18)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1) ; Auswahlliste setzen
	(dcl_Grid_SetItemDropList GRID_Dcl-1_grid1
							  nRow
							  2
							  '("Element 1" "Element 2" "Element 3" "Element 4")
	)                                   ; Das Element als Auswahlliste von Symbolen hinzufügen
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "5\t \tStern"))
										; Neuen Zellenstil setzen
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 29)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1) ; Auswahlliste setzen
	(dcl_Grid_SetItemDropList GRID_Dcl-1_grid1        nRow
							  2
							  '("Stern" "Fuß" "Hand" "Mond")
							  '(0 1 2 3)
							 )          ; Das Element als Kombinationsfeld für Textwerte hinzufügen
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "6\t \tText-Kombinationsfeld"))
										; Neuen Zellenstil setzen
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 36)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1) ; Auswahlliste setzen
	(dcl_Grid_SetItemDropList GRID_Dcl-1_grid1
							  nRow
							  2
							  '("Element 1" "Element 2" "Element 3" "Element 4")
	)                                   ; Das Element als Kombinationsfeld für Winkelwerte hinzufügen
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "7\t \t0"))
										; Neuen Zellenstil setzen
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 37)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1) ; Auswahlliste setzen


        ; Beachten Sie, dass alle Auswahllisten zunächst mit Zeichenketten arbeiten,
        ; obwohl der Zelle der Stil der Auswahlliste mit Realzahlen zugewiesen wurde.
    
	(dcl_Grid_SetItemDropList GRID_Dcl-1_grid1 nRow 2 '("0" "90" "180" "270"))
										; Das Element als schaltbare Symbole hinzufügen
	(Setq nRow (dcl_Grid_AddString GRID_Dcl-1_grid1 "8\t \tSchaltbare Symbole"))
										; Neuen Zellenstil setzen
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 3 1 2)
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
	(FillColCountList)
	(princ)
)


;;;******************************************************************************************************************************
(defun c:Project1_Dcl-1_TB-AddRow_OnClicked
	   (/ GRIDCELLIMAGEINT GRIDCELLTYPEINT GRIDCOLUMNVALUE GRIDROWCOUNT NROW)
	(setq GridCellTypeInt  (dcl_ComboBox_GetCurSel GRID_Dcl-1_CBx-GridCellType)
		  GridCellImageInt (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images)
		  GridColumnValue  (dcl_Control_GetText GRID_Dcl-1_TB-Value)
		  GridRowCount     (dcl_Grid_GetRowCount GRID_Dcl-1_grid1)
	)                                   ; Zelle mit Kontrollkästchen ergänzen
	(Setq nRow (dcl_Grid_AddString
				   GRID_Dcl-1_grid1
				   (strcat (itoa GridRowCount) "\t \t" GridColumnValue)
			   )
	)                                   ; \t mit dem Tabulatorzeichen werden die Zellen einer Zeile voneinader getrennt
										; Neuen Zellenstil für Spalte 1 setzen
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 1 1)
										; Kontrollkästchen in neuer Zelle deaktivieren
	(dcl_Grid_SetItemCheck GRID_Dcl-1_grid1 nRow 1 0)
										; Neuen Zellenstil für Spalte 2 setzen
	(dcl_Grid_SetItemStyle GRID_Dcl-1_grid1 nRow 2 GridCellTypeInt)
	(if (> (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images) 0)
		(dcl_Grid_SetItemImage
			GRID_Dcl-1_grid1
			nRow
			2
			(1- (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-Images))
		)                               ; Setze das neue Symbol für die Zelle in Spalte 2
	)
)


(defun c:grid_Dcl-1_TB-AddCol_OnClicked (/ txt cWidth cImg)
	(setq txt (dcl_Control_GetText GRID_Dcl-1_TB-ColTitle))
	(setq cImg (1- (dcl_ImageComboBox_GetCurSel GRID_Dcl-1_IC-ColImages)))
	(if (>= cImg 0)
		(Setq cWidth (+ (dcl_Grid_CalcColWidth GRID_Dcl-1_grid1 txt) 46))
		(Setq cWidth (+ (dcl_Grid_CalcColWidth GRID_Dcl-1_grid1 txt) 15))
	)
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




(defun c:Project1_Dcl-1_grid1_OnBeginLabelEdit (nRow nCol / newValue)
	(setq newValue (strcat "Ereignis: OnBeginLabelEdit
Argumente: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue)
)




(defun c:Project1_Dcl-1_grid1_OnButtonClicked (nRow nCol / newValue)
	(setq newValue (strcat "Ereignis: OnButtonClicked
Argumente: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue)
)

(defun c:Project1_Dcl-1_grid1_OnSelChanged (nRow nCol / newValue)
	(setq newValue (strcat "Ereignis: OnSelChanged
Argumente: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue)
)


(defun c:Project1_Dcl-1_grid1_OnEndLabelEdit (nRow nCol / newValue)
	(setq newValue (strcat "Ereignis: OnEndLabelEdit
Argumente: nRow: "         (itoa nRow)
						   " ; nCol: "
						   (itoa nCol)
				   )
	)
	(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue)
)


(defun c:Project1_Dcl-1_grid1_OnColumnClick (nColumn / newValue)
	(setq newValue (strcat "Ereignis: OnColumnClick
Argumente: nColumn: "      (itoa nColumn)
				   )
	)
	(dcl_Control_SetCaption GRID_Dcl-1_GridStatus newValue)
)

(princ)

;|<<OpenDCL Samples Epilog>>|;

;;;######################################################################
;;;######################################################################
;;; Der folgende Abschnitt dient dazu, die Beispiel-Dateien zu lokalisieren.
;;; Die Pfadangabe wird um den Abschnitt des Beispielordner, erweitert, der
;;; durch das Installationsprogramm in der Registry eingetragen wurde.
;;; Die globalen Variable *ODCL:Prefix und die Function *ODCL:Samples:FindFile
;;; werden in allen Beispieldateien verwendet.
;;;
(or *ODCL:Samples:FindFile
	(defun *ODCL:Samples:FindFile (file)
		(setq *ODCL:Prefix
			(cond
				(	*ODCL:Prefix
				) ;_ Bereits definiert
				(	(vl-registry-read
						"HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
						"SamplesFolder"
					)
				) ;_ 32-bit Variante aktueller Nutzer
				(	(vl-registry-read
						"HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
						"SamplesFolder"
					 )
				) ;_ 32-bit Variante alle Nutzer
				(	(vl-registry-read
						"HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
						"SamplesFolder"
					)
				) ;_ 64-bit Variante aktueller Nutzer
				(	(vl-registry-read
						"HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
						"SamplesFolder"
					)
				) ;_ 64-bit Variante alle Nutzer
			)
		)
		(cond
			((findfile file)) ; überprüfe zunächst den Supportpfad
			(*ODCL:Prefix (findfile (strcat *ODCL:Prefix file)))
			(file)
		)
	)
)

;; Ist der Hauptdialog der OpenDCL-Beispiele aktiv, starte das Beispiel sofort.
;; Andernfalls gib einen Text in der Befehlszeile aus, mit welchem Kommando das Beispiel
;; gestartet werden kann. Auf diesem Wege wird sichergestellt, dass der Name des Beispiels
;; nur an einer Stelle definiert werden muss. Das macht es einfacher, den Code wiederzuverwenden.

(	(lambda (demoname)
		(if *ODCL:MasterDemo
			(progn
				(princ (strcat "'" demoname "\n"))
				(apply (read (strcat "C:" demoname)) nil)
			)
			(progn
				(princ (strcat "\n" demoname " OpenDCL-Beispiel ist geladen."))
				(princ (strcat " (Starten Sie das Beispiel mit dem Befehl " (strcase demoname) ")\n"))
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
