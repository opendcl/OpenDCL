;;;
;;; GRID Beispiel
;;;
;;; Dieses Beispiel demonstriert das Datenblatt mit all seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:Grd (/ cmdecho)

  ;; Stellt sicher, dass die Laufzeitumgebung im Hintergrund geladen wird
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Lädt das Projekt
  (dcl-Project-Load (*ODCL:Samples-FindFile "Grid.odcl"))

  ;; Zeigt den Hauptdialog an
  (dcl-Form-Show Grid/Dcl-1)

  ;; Dies ist ein modaler Dialog, so dass das Programm nach (dcl-Form-Show)
  ;; erst fortfährt, wenn der Dialog geschlossen wurde. In der Zwischenzeit
  ;; behandeln die Ereignisse den Dialog.

  (princ)
)

;|«OpenDCL Event Handlers»|;

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
              ; Gesamtanzahl der Spalten zur Auswahlliste hinzufügen
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
                (-1 . "Vorgabewert aus der Spaltendefinition")
                (0 . "Nicht editierbar")
                (1 . "Kontrollkästchen")
                (2 . "Optionsfelder")
                (3 . "Schaltbare Bilder")
                (4 . "Schaltflächen")
                (5 . "Auswahl-Schaltflächen")
                (6 . "Texte")
                (7 . "Winkel")
                (8 . "Ganzzahlen")
                (9 . "Dezimalzahlen")
                (10 . "Großbuchstaben")
                (11 . "Kleinbuchstaben")
                (12 . "Passwort")
                (13 . "Mehrzeiliger Text")
                (14 . "Währung")
                (15 . "Datum")
                (16 . "Zeit")
                (17 . "Prozent")
                (18 . "Auswahlliste")
                (19 . "Pfeilspitzen")
                (20 . "ACI-Farbenliste")
                (21 . "Textstile")
                (22 . "Plotstilnamen")
                (23 . "Plotstiltabellen")
                (24 . "Plotter")
                (25 . "Schriftartenliste")
                (26 . "Laufwerke")
                (27 . "Layer")
                (28 . "Bemaßungsstile")
                (29 . "Bilderliste")
                (30 . "ACI-Farben")
                (31 . "RGB-Farben")
                (32 . "Linienstärken")
                (33 . "Linientypen")
                (34 . "Ordner")
                (35 . "Dateien")
                (36 . "Text-Kombinationsfeld")
                (37 . "Winkel-Kombinationsfeld")
                (38 . "Ganzzahlen-Kombinationsfeld")
                (39 . "Dezimalzahlen-Kombinationsfeld")
                (40 . "Großbuchstaben-Kombinationsfeld" )
                (41 . "Kleinbuchstaben-Kombinationsfeld")
                (42 . "Symbolnamen")
                (43 . "Symbolnamen-Kombinationsfeld")
                )
        ImageComboBoxList (list '("---" 100) '("Stern" 0) '("Füße" 1) '("Hand" 2) '("Mond" 3))) ; Liste zur Auswahlliste hinzufügen

  (dcl-ComboBox-Clear Grid/Dcl-1/CBx-GridCellType)			; Auswahlliste leeren

        (foreach lstItem GridCellTypeList					; Liste zur Auswahlliste hinzufügen
            (setq nRow (dcl-ComboBox-AddString Grid/Dcl-1/CBx-GridCellType (strcat (itoa (car lstItem)) " - " (cdr lstItem)))) ;; Fügt ein Element hinzu
            (dcl-ComboBox-setitemdata Grid/Dcl-1/CBx-GridCellType nRow (car lstItem))					       ;; Weist Daten zu
        ); foreach

  (dcl-ComboBox-SetCurSel Grid/Dcl-1/CBx-GridCellType 0)			; Wählt das erste Element der Liste aus

  (AddListImageComboBox ImageComboBoxList Grid/Dcl-1/IC-Images)		; Fügt Elemente zur Auswahlliste mit Bildern hinzu

  (dcl-ImageComboBox-SetCurSel Grid/Dcl-1/IC-Images 0)			; Wählt das erste Element der Liste aus

  (AddListImageComboBox ImageComboBoxList Grid/Dcl-1/IC-ColImages)	; Fügt Elemente zur Auswahlliste mit Bildern hinzu

  (dcl-ImageComboBox-SetCurSel Grid/Dcl-1/IC-ColImages 0)			; Wählt das erste Element der Liste aus

  (dcl-ComboBox-SetCurSel Grid/Dcl-1/CBx-GridColJust 0)			; Wählt das erste Element der Liste aus

  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1
                   (strcat "0\t \tKontrollkästchen")
         )
  )
  (dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 1 0)
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 1)

  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1
                   (strcat "1\t \tOptionsfelder")
         )
  )
  (dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 1 1)
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 2)

  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1
                   (strcat "2\t \tSchaltflächen")
         )
  )
  (dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 1 2)
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 4)
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)

  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 
                   (strcat "3\t \tAuswahl-Schaltflächen")
         )
  )
  (dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 1 3)
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 5)
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)
                  
  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "4\t \tElement1"))	; Fügt ein Element zum Datenblatt hinzu
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 18)			; Setzt den neuen Zellenstil
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)		
  (dcl-Grid-SetItemDropList Grid/Dcl-1/grid1 nRow 2 '("Element1" "Element2" "Element3" "Element4"))	; Setzt die Auswahlliste
  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "5\t \tStern")) 	; Fügt ein Element als Auswahlliste mit Bildern hinzu	
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 29)			; Setzt den neuen Zellenstil
  (dcl-Grid-SetItemImage Grid/Dcl-1/grid1 nRow 2 0)
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1) 
  (dcl-Grid-SetItemDropList Grid/Dcl-1/grid1 nRow 2
                '("Stern" "Füße" "Hand" "Mond")
                '(0 1 2 3)
               )			; Setzt die Auswahlliste
          
  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "6\t \tText-Liste"))	; Fügt ein Element als Auswahlliste mit Zeichenketten hinzu
              
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 36)			; Setzt den neuen Zellenstil
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)
  (dcl-Grid-SetItemDropList Grid/Dcl-1/grid1 nRow 2
                               '("Element1" "Element2" "Element3" "Element4"))	; Setzt die Auswahlliste


  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "7\t \t0"))		; Fügt ein Element als Auswahlliste mit Winkeln hinzu
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 37)			; Setzt den neuen Zellenstil
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)

  ; Beachten Sie, dass alle Listenelemente Zeichenketten sein müssen,
  ; obgleich es eine Auswahlliste mit Winkeln ist. Gleiches gilt für
  ; Listen mit Realzahlen und Ganzzahlen
  (dcl-Grid-SetItemDropList Grid/Dcl-1/grid1 nRow 2 '("0" "90" "180" "270"))	; Setzt die Auswahlliste
                    
  (Setq nRow (dcl-Grid-AddString Grid/Dcl-1/grid1 "8\t \tWechselnde Bilder"))	; Fügt ein Element mit wechselnden Bildern hinzu
                    
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 3 1 2)			; Setzt den neuen Zellenstil
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)
  (FillColCountList)
  (dcl-ListBox-Clear Grid/Dcl-1/LB-Status)				; Leert das Protokoll
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
  ; Fügt ein Element mit Kontrollkästchen hinzu
  (Setq nRow (dcl-Grid-AddString
           Grid/Dcl-1/grid1
           (strcat (itoa GridRowCount) "\t \t" GridColumnValue)
         )
  )									; \t für einzelne Spalten
              
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 1 1)			; Setzt den Zellenstil für die Zelle in Spalte 1
  (dcl-Grid-SetItemCheck Grid/Dcl-1/grid1 nRow 1 0)			; Schaltet den Zustandsschalter im Kontrollkästchen aus
  (dcl-Grid-SetItemStyle Grid/Dcl-1/grid1 nRow 2 GridCellTypeVal)		; Setzt den Zellenstil für die Zelle in Spalte 2
  (if (> (dcl-ImageComboBox-GetCurSel Grid/Dcl-1/IC-Images) 0)		; Setzt ein neues Bild für die Zelle in Spalte 2
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
  (setq newValue (strcat "Ereignis: OnBeginLabelEdit
Argumente: nRow: "         (itoa nRow)
               " ; nCol: "
               (itoa nCol)
           )
  )
  (dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue))
)




(defun c:Grid/Dcl-1/grid1#OnButtonClicked (nRow nCol / newValue)
  (setq newValue (strcat "Ereignis: OnButtonClicked
Argumente: nRow: "         (itoa nRow)
               " ; nCol: "
               (itoa nCol)
           )
  )
  (dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue))
)

(defun c:Grid/Dcl-1/grid1#OnSelChanged (nRow nCol / newValue)
  (setq newValue (strcat "Ereignis: OnSelChanged
Argumente: nRow: "         (itoa nRow)
               " ; nCol: "
               (itoa nCol)
           )
  )
  (dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue))
)


(defun c:Grid/Dcl-1/grid1#OnEndLabelEdit (nRow nCol / newValue)
  (setq newValue (strcat "Ereignis: OnEndLabelEdit
Argumente: nRow: "         (itoa nRow)
               " ; nCol: "
               (itoa nCol)
           )
  )
  (if Grid/Dcl-1/LB-Status (dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue)))
)


(defun c:Grid/Dcl-1/grid1#OnColumnClick (nColumn / newValue)
  (setq newValue (strcat "Ereignis: OnColumnClick
Argumente: nColumn: "      (itoa nColumn)
           )
  )
  (dcl-ListBox-SetTopIndex Grid/Dcl-1/LB-Status (dcl-ListBox-AddString Grid/Dcl-1/LB-Status newValue))
)

(princ)

;|«OpenDCL Samples Epilog»|;

;;;######################################################################
;;;######################################################################
;;; Der folgende Abschnitt dient dazu, die Beispiel-Dateien zu lokalisieren.
;;; Die Pfadangabe wird um den Abschnitt des Beispielordner, erweitert, der
;;; durch das Installationsprogramm in der Registry eingetragen wurde.
;;; Die globalen Variable *ODCL:Prefix und die Function *ODCL:Samples-FindFile
;;; werden in allen Beispieldateien verwendet.
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

;; Ist der Hauptdialog der OpenDCL-Beispiele aktiv, starte das Beispiel sofort.
;; Andernfalls gib einen Text in der Befehlszeile aus, mit welchem Kommando das Beispiel
;; gestartet werden kann. Auf diesem Wege wird sichergestellt, dass der Name des Beispiels
;; nur an einer Stelle definiert werden muss. Das macht es einfacher, den Code wiederzuverwenden.

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
  "Grd"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
