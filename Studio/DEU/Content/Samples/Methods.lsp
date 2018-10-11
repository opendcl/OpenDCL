;;;
;;; Methoden Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung verschiedener Methoden.
;;;

;; Hauptprogramm
(defun c:Methods (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl-Project-Load (*ODCL:Samples-FindFile "Methods.odcl"))

	;; Dialog anzeigen
	(dcl-Form-Show Methods/Main)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl-Form-Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

(defun PopulateManipulationTabFields (/ curPos tabSize)
	(setq curPos           (dcl-Control-GetPos Methods/Main/SampleControl)
			nLeft            (car curPos)
			nTop             (cadr curPos)
			nWidth           (caddr curPos)
			nHeight          (cadddr curPos)
			tabSize          (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			BottomFromBottom (- (cadr tabSize) nTop nHeight)
			RightFromRight   (- (car tabSize) nLeft nWidth)
			TopFromBottom    (- (cadr tabSize) nTop)
			LeftFromRight    (- (car tabSize) nLeft)
	)
	(dcl-Control-SetText Methods/Main/BottomFromBottom
						 (itoa BottomFromBottom)
	)
	(dcl-Control-SetText Methods/Main/RightFromRight
						 (itoa RightFromRight)
	)
	(dcl-Control-SetText Methods/Main/TopFromBottom (itoa TopFromBottom))
	(dcl-Control-SetText Methods/Main/LeftFromRight (itoa LeftFromRight))
	(dcl-Control-SetText Methods/Main/LeftBox (itoa nLeft))
	(dcl-Control-SetText Methods/Main/TopBox (itoa nTop))
	(dcl-Control-SetText Methods/Main/WidthBox (itoa nWidth))
	(dcl-Control-SetText Methods/Main/HeightBox (itoa nHeight))
)

;|*OpenDCL Event Handlers*|;

(defun c:Methods/Main/cmdClose#OnClicked (/)
	(dcl-Form-Close Methods/Main)
)

(defun c:Methods/Main/TabControl#OnChanged (nSelIndex /
											BottomFromBottom
											RightFromRight  TopFromBottom
											LeftFromRight   nLeft
											nTop            nWidth
											nHeight
											 )
	(if (= nSelIndex 1) ; Karteikarte Regler
		(progn (dcl-Control-SetValue
					 Methods/Main/AngleSlider1
					 (atoi (dcl-Control-GetText Methods/Main/AngleBox))
				 )
				 (dcl-Control-SetValue
					 Methods/Main/ScrollBar1
					 (atoi (dcl-Control-GetText Methods/Main/VScrollLabel))
				 )
				 (dcl-Control-SetValue
					 Methods/Main/SliderBar1
					 (atoi (dcl-Control-GetText Methods/Main/VSliderLabel))
				 )
				 (dcl-Control-SetValue
					 Methods/Main/ScrollBar2
					 (atoi (dcl-Control-GetText Methods/Main/HScrollLabel))
				 )
				 (dcl-Control-SetValue
					 Methods/Main/SliderBar2
					 (atoi (dcl-Control-GetText Methods/Main/HSliderLabel))
				 )
				 (dcl-Control-SetText
					 Methods/Main/SpinnedBox
					 (itoa (dcl-Control-GetValue Methods/Main/SpinButton1))
				 )
		)
	) ;_ if
	(if (= nSelIndex 2) ; Karteikarte Steuerung
		(PopulateManipulationTabFields)
	) ;_ if
)

;; ------------------- Karteikarte Bild -------------------------------

(defun c:Methods/Main/DrawText#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(dcl-PictureBox-DrawText Methods/Main/PictureBox
							 (List (List 10 10 -19 20 "Linie 1" "TL")
									 (List 10 25 -19 30 "Linie 2" "TL")
									 (List 10 40 -19 40 "Linie 3" "TL")
									 (List 10 55 -19 50 "Linie 4" "TL" T)
									 (List 10 70 -19 56 "Linie 5" "TL" T)
							 )
	)
)

(defun c:Methods/Main/DrawWrappedText#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(dcl-PictureBox-DrawWrappedText
		Methods/Main/PictureBox
		'((10
			 10
			 220
			 -19
			 "Dieser Beispieltext wird automatisch umgebrochen, wenn der Text die vorgegebene Breite überschreiten würde."
			 "L"
			)
			(10
			 70
			 220
			 -19
			 "Dieser Beispieltext wird automatisch umgebrochen, wenn der Text die vorgegebene Breite überschreiten würde."
			 "C"
			)
			(10
			 130
			 220
			 -19
			 "Dieser Beispieltext wird automatisch umgebrochen, wenn der Text die vorgegebene Breite überschreiten würde."
			 "R"
			)
		 )
	)
)

(defun c:Methods/Main/DrawLine#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(dcl-PictureBox-DrawLine Methods/Main/PictureBox
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
(defun c:Methods/Main/DrawArc#OnClicked (/ nBoundingLeft nBoundingTop)
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(setq nBoundingLeft 50)
	(setq nBoundingTop 50)

	;; Die OpenDCL Laufzeitumgebung erwartet, dass größere Listen in einem bereits ausgewerteten Format übergeben werden.
	;; Bei Listen in der Art und Weise '((nBoundingLeft nBoundingTop ...) (nBoundingLeft nBoundingTop ...)) können aus
	;; unbekannten Gründen Fehler auftreten. Bauen Sie größere Listen wie folgt auf:
		
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
	(dcl-PictureBox-DrawArc Methods/Main/PictureBox ArgumentList)
)

(defun c:Methods/Main/DrawCircle#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(dcl-PictureBox-DrawCircle Methods/Main/PictureBox
								 '((10 10 140 140 1) (10 160 140 40 2))
	)
)

(defun c:Methods/Main/DrawHatchRect#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(dcl-PictureBox-DrawHatchRect Methods/Main/PictureBox
									'((10 10 40 40 1 1)
									(10 60 40 40 2 2)
									(60 10 40 40 3 3)
									(60 60 40 40 4 4)
									(10 110 40 40 5 5)
									 )
	)
)

(defun c:Methods/Main/DrawFillRect#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(dcl-PictureBox-DrawFillRect Methods/Main/PictureBox
								 '((10 10 40 40 1)
									 (10 60 40 40 2)
									 (60 10 40 40 3)
									 (60 60 40 40 4)
									 (10 110 40 40 5)
									)
	)
)

(defun c:Methods/Main/PaintPicture#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(setq PictureList (list (list 10 10 102 T T)
							(list 10 50 105 T T)
							(list 10 90 106 T T)
							(list 10 130 107 T T)
							(list 10 170 108 T T)
						)
	)
	(dcl-PictureBox-PaintPicture Methods/Main/PictureBox PictureList)
)

(defun c:Methods/Main/DrawPoints#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(dcl-PictureBox-DrawPoint Methods/Main/PictureBox 14 10 1)
	(dcl-PictureBox-DrawPoint Methods/Main/PictureBox 18 10 2)
	(dcl-PictureBox-DrawPoint Methods/Main/PictureBox 22 10 3)
	(dcl-PictureBox-DrawPoint Methods/Main/PictureBox 26 10 4)
)

(defun c:Methods/Main/DrawEdge#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
	(dcl-PictureBox-DrawEdge Methods/Main/PictureBox
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

(defun c:Methods/Main/LoadPictureFile#OnClicked (/ sFileName)
	(setq sFileName
			 (GetFiled "Wählen Sie eine Bilddatei" "" "bmp;ico;jpg;gif;wmf" 8)
	)
	(if (/= sFileName nil)
		(dcl-PictureBox-LoadPictureFile Methods/Main/PictureBox
										sFileName
		)
		(dcl-PictureBox-StoreImage Methods/Main/PictureBox)
	)
)

(defun c:Methods/Main/StoreImage#OnClicked ()
	(dcl-PictureBox-StoreImage Methods/Main/PictureBox)
)

(defun c:Methods/Main/Clear#OnClicked ()
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
)

(defun c:Methods/Main/PictureBox#OnMouseDblClick (Button Flags X Y /)
	(dcl-PictureBox-Clear Methods/Main/PictureBox)
)

;; ------------------- Karteikarte Regler -------------------------------

(defun c:Methods/Main/ScrollBar1#OnScroll (nValue /)
	(dcl-Control-SetText Methods/Main/VScrollLabel (itoa nValue))
)

(defun c:Methods/Main/ScrollBar2#OnScroll (nValue /)
	(dcl-Control-SetText Methods/Main/HScrollLabel (itoa nValue))
)

(defun c:Methods/Main/AngleSlider1#OnScroll (nValue /)
	(dcl-Control-SetText Methods/Main/AngleBox (itoa nValue))
)

(defun c:Methods/Main/SliderBar2#OnReleasedCapture (nValue /)
	(dcl-Control-SetText Methods/Main/HSliderLabel (itoa nValue))
)

(defun c:Methods/Main/SliderBar1#OnReleasedCapture (nValue /)
	(dcl-Control-SetText Methods/Main/VSliderLabel (itoa nValue))
)

(defun c:Methods/Main/SpinButton1#OnChanged (nValue nDelta/)
	(dcl-Control-SetText Methods/Main/SpinnedBox (itoa nValue))
)

(defun c:Methods/Main/AngleBox#OnEditChanged (sText /)
	(dcl-Control-SetValue Methods/Main/AngleSlider1 (atoi sText))
)

(defun c:Methods/Main/VScrollLabel#OnEditChanged (sText /)
	(dcl-Control-SetValue Methods/Main/ScrollBar1 (atoi sText))
)

(defun c:Methods/Main/VSliderLabel#OnEditChanged (sText /)
	(dcl-Control-SetValue Methods/Main/SliderBar1 (atoi sText))
)

(defun c:Methods/Main/HScrollLabel#OnEditChanged (sText /)
	(dcl-Control-SetValue Methods/Main/ScrollBar2 (atoi sText))
)

(defun c:Methods/Main/HSliderLabel#OnEditChanged (sText /)
	(dcl-Control-SetValue Methods/Main/SliderBar2 (atoi sText))
)


(defun c:Methods/Main/SpinnedBox#OnEditChanged (sText /)
	(dcl-Control-SetValue Methods/Main/SpinButton1 (atoi sText))
)


;; ------------------- Karteikarte Steuerung -------------------------------

(defun c:Methods/Main/LeftBox#OnEditChanged (sText / curPos tabSize nVal)
	(setq curPos  (dcl-Control-GetCurPos Methods/Main/SampleControl)
			tabSize (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			nVal    (atoi sText)
	)
	(dcl-Control-SetLeft Methods/Main/SampleControl nVal)
	(dcl-Control-SetLeftFromRight Methods/Main/SampleControl
									(setq nVal (- (car tabSize) nVal))
	)
	(dcl-Control-SetText Methods/Main/LeftFromRight nVal)
)

(defun c:Methods/Main/TopBox#OnEditChanged (sText / curPos tabSize nVal)
	(setq curPos  (dcl-Control-GetCurPos Methods/Main/SampleControl)
			tabSize (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			nVal    (atoi sText)
	)
	(dcl-Control-SetTop Methods/Main/SampleControl nVal)
	(dcl-Control-SetTopFromBottom Methods/Main/SampleControl
									(setq nVal (- (cadr tabSize) nVal))
	)
	(dcl-Control-SetText Methods/Main/TopFromBottom nVal)
)

(defun c:Methods/Main/WidthBox#OnEditChanged (sText / curPos tabSize nVal)
	(setq curPos  (dcl-Control-GetCurPos Methods/Main/SampleControl)
			tabSize (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			nVal    (atoi sText)
	)
	(dcl-Control-SetWidth Methods/Main/SampleControl nVal)
	(dcl-Control-SetRightFromRight
		Methods/Main/SampleControl
		(setq nVal (- (car tabSize) nVal (car curPos)))
	)
	(dcl-Control-SetText Methods/Main/RightFromRight nVal)
)

(defun c:Methods/Main/HeightBox#OnEditChanged (sText / curPos tabSize nVal)
	(setq curPos  (dcl-Control-GetCurPos Methods/Main/SampleControl)
			tabSize (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			nVal    (atoi sText)
	)
	(dcl-Control-SetHeight Methods/Main/SampleControl nVal)
	(dcl-Control-SetBottomFromBottom
		Methods/Main/SampleControl
		(setq nVal (- (cadr tabSize) nVal (cadr curPos)))
	)
	(dcl-Control-SetText Methods/Main/BottomFromBottom nVal)
)

(defun c:Methods/Main/VisibleCheck#OnClicked (nValue /)
	(dcl-Control-SetVisible Methods/Main/SampleControl
							(not (zerop nValue))
	)
)

(defun c:Methods/Main/EnableOption#OnClicked (nValue /)
	(dcl-Control-SetEnabled Methods/Main/SampleControl T)
)

(defun c:Methods/Main/DisableOption#OnClicked (nValue /)
	(dcl-Control-SetEnabled Methods/Main/SampleControl nil)
)

(defun c:Methods/Main/LeftCheck#OnClicked (nValue /)
	(dcl-Control-SetUseLeftFromRight Methods/Main/SampleControl nValue)
)

(defun c:Methods/Main/TopCheck#OnClicked (nValue /)
	(dcl-Control-SetUseTopFromBottom Methods/Main/SampleControl nValue)
)

(defun c:Methods/Main/RightCheck#OnClicked (nValue /)
	(dcl-Control-SetUseRightFromRight Methods/Main/SampleControl nValue)
)

(defun c:Methods/Main/BottomCheck#OnClicked (nValue /)
	(dcl-Control-SetUseBottomFromBottom Methods/Main/SampleControl
										nValue
	)
)

(defun c:Methods/Main/LeftFromRight#OnEditChanged (sText / curPos tabSize nVal)
	(setq curPos  (dcl-Control-GetCurPos Methods/Main/SampleControl)
			tabSize (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			nVal    (atoi sText)
	)
	(dcl-Control-SetLeftFromRight Methods/Main/SampleControl nVal)
	(dcl-Control-SetLeft Methods/Main/SampleControl
						 (setq nVal (- (car tabSize) nVal))
	)
	(dcl-Control-SetText Methods/Main/LeftBox nVal)
)

(defun c:Methods/Main/TopFromBottom#OnEditChanged (sText / curPos tabSize nVal)
	(setq curPos  (dcl-Control-GetCurPos Methods/Main/SampleControl)
			tabSize (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			nVal    (atoi sText)
	)
	(dcl-Control-SetTopFromBottom Methods/Main/SampleControl nVal)
	(dcl-Control-SetTop Methods/Main/SampleControl
						(setq nVal (- (cadr tabSize) nVal))
	)
	(dcl-Control-SetText Methods/Main/TopBox nVal)
)

(defun c:Methods/Main/RightFromRight#OnEditChanged (sText / curPos tabSize nVal)
	(setq curPos  (dcl-Control-GetCurPos Methods/Main/SampleControl)
			tabSize (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			nVal    (atoi sText)
	)
	(dcl-Control-SetRightFromRight Methods/Main/SampleControl nVal)
	(dcl-Control-SetWidth Methods/Main/SampleControl
							(setq nVal (- (car tabSize) nVal (car curPos)))
	)
	(dcl-Control-SetText Methods/Main/WidthBox nVal)
)

(defun c:Methods/Main/BottomFromBottom#OnEditChanged (sText / curPos tabSize nVal)
	(setq curPos  (dcl-Control-GetCurPos Methods/Main/SampleControl)
			tabSize (dcl-TabStrip-GetControlArea Methods/Main/TabControl)
			nVal    (atoi sText)
	)
	(dcl-Control-SetBottomFromBottom Methods/Main/SampleControl nVal)
	(dcl-Control-SetHeight Methods/Main/SampleControl
							 (setq nVal (- (cadr tabSize) nVal (cadr curPos)))
	)
	(dcl-Control-SetText Methods/Main/HeightBox nVal)
)

(defun c:Methods/Main#OnSize (nWidth nHeight /)
	(if (= 2 (dcl-TabStrip-GetCurSel Methods/Main/TabControl))
		(PopulateManipulationTabFields)
	)
)

(princ)

;|*OpenDCL Samples Epilog*|;

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
		(if *ODCL:AllSamples
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
	"Methods"
)
(princ)

;;;######################################################################
;;;######################################################################
