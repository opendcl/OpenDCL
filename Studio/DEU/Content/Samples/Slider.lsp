;;;
;;; Beispiel eines benutzerdefinierten Schiebereglers
;;;
;;; Autor: Fred Tomke
;;; Datum: 27.10.2009
;;;
;;; Demonstriert das Erstellen und die Steuerung eines benutzerdefinierten
;;; Schiebereglers basierend auf einem OpenDCL-Bild-Steuerelement
;;; 
;;; Modified by: Owen Wengerd [2009-10-30]
;;; I rewrote large portions of Fred's fine work to eliminate all globals
;;; and reformatted to more closely resemble the other OpenDCL samples


;; Hauptprogramm
(defun c:Slider (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl-Project-Load (*ODCL:Samples:FindFile "Slider.odcl"))

	;; Dialog anzeigen
	(dcl-Form-Show Slider/Form1)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl-Form-Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)


;; --------------------------------------------------------------------------------------
;; Beschreibung: Konvertiert die y-Koordinate des Reglerknopfs in Prozentangaben
;; Argumente: intTop = neue y-Koordinate des Reglerknopfs innerhalb des Bildsteuerelements als Integer
;; Rückgabe: berechnete Prozentangabe in Integer

(defun slider_get_percent (intPos / intBorder intButtonHeight intHeight)
	(setq intBorder (slider_get_border_size))
	(setq intButtonHeight (slider_get_button_height))
	(setq intHeight (- (dcl-Control-GetHeight Slider/Form1/pic_slider) intBorder intButtonHeight 1))
	(cond
		((<= intHeight 0) 0)
		((>= intPos intHeight) 0)
		((<= intPos 0) 100)
		((fix (+ 0.5 (* 100.0 (/ (float (- intHeight intPos)) (float intHeight))))))
	); cond
); slider_get_percent


;; --------------------------------------------------------------------------------------
;; Beschreibung: Konvertiert die Prozentangabe in die y-Koordinate des Reglerknopfs
;; Argumente: intPercent = Prozentangabe als Integer
;; Rückgabe: neue y-Koordinate des Reglerknopfs innerhalb des Bildsteuerelements als Integer

(defun slider_get_pos (intPercent / intBorder intButtonHeight intHeight)
	(setq intBorder (slider_get_border_size))
	(setq intButtonHeight (slider_get_button_height))
	(setq intHeight (- (dcl-Control-GetHeight Slider/Form1/pic_slider) intBorder intButtonHeight 1))
	(cond
		((<= intHeight 0) 0)
		((<= intPercent 0) intHeight)
		((>= intPercent 100) 0)
		((- intHeight (fix (+ 0.5 (* (/ intPercent 100.0) (float intHeight))))))
	); cond
); slider_get_pos


;; --------------------------------------------------------------------------------------
;; Beschreibung: aktualisiert die Reglerposition im gedrückten Zustand
;; Argumente: intPercent = Prozentangabe als Integer
;; Rückgabe: <nicht verwendet>

(defun slider_change_percent (intPercent)
	(cond
		((< intPercent 0) (setq intPercent 0))
		((> intPercent 100) (setq intPercent 100))
	); cond
	(if (/= intPercent(atoi (dcl-Control-GetText Slider/Form1/edt_percent)))
		(progn
			(dcl-Control-SetText Slider/Form1/edt_percent (itoa intPercent))
			(dcl-Control-Redraw Slider/Form1/pic_slider)
		); progn
	); if
); slider_change_percent


;; --------------------------------------------------------------------------------------
;; Beschreibung: Initiiert den Drag-Vorgang durch Aktivieren des OnMouseMove-Ereignis
;; Argumente: keine
;; Rückgabe: <nicht verwendet>

(defun slider_start_dragging ()
	(dcl-Control-SetProperty Slider/Form1/pic_slider "MouseMove" "c:Slider_Form1_pic_slider_OnMouseMove")
); slider_start_dragging


;; --------------------------------------------------------------------------------------
;; Beschreibung: Stoppt den Drag-Vorgang durch Deaktivieren des OnMouseMove-Ereignis
;; Argumente: keine
;; Rückgabe: <nicht verwendet>

(defun slider_stop_dragging ()
	(dcl-Control-SetProperty Slider/Form1/pic_slider "MouseMove" "")
); slider_stop_dragging


;; --------------------------------------------------------------------------------------
;; Beschreibung: Überprüft und gibt das große Inkrement aus
;; Argumente: keine
;; Rückgabe: großes Inkrement als Integer

(defun slider_get_large_increment (/ intVal)
	(setq intVal (atoi (dcl-ComboBox-GetEBText Slider/Form1/cbb_large)))
	(cond
		((<= intVal 5) 5)
		((>= intVal 50) 50)
		(intVal)
	); cond
); slider_get_large_increment


;; --------------------------------------------------------------------------------------
;; Beschreibung: Überprüft und gibt das kleine Inkrement aus
;; Argumente: keine
;; Rückgabe: kleines Inkrement als Integer

(defun slider_get_small_increment (/ intVal)
	(setq intVal (atoi (dcl-ComboBox-GetEBText Slider/Form1/cbb_small)))
	(cond
		((<= intVal 1) 1)
		((>= intVal 20) 20)
		(intVal)
	); cond
); slider_get_small_increment


;; --------------------------------------------------------------------------------------
;; Beschreibung: Berechnet die Reglerknopfgröße in Abhängigkeit vom gewählten Stil
;; Argumente: keine
;; Rückgabe: Reglerknopfgröße als Integer

(defun slider_get_button_height (/ intButtonStyle)
	(setq intButtonStyle (dcl-ComboBox-GetCurSel Slider/Form1/cbb_style))
	(cond
		((member intButtonStyle '(-1 0 1)) 12)
		((= intButtonStyle 3) 3)
		((= intButtonStyle 4) 16)
		(T 0)
	); cond
); slider_get_button_height


;; --------------------------------------------------------------------------------------
;; Beschreibung: Berechnet die Randstärke in Abhängigkeit vom gewählten Stil
;; Argumente: keine
;; Rückgabe: Randgröße als Integer

(defun slider_get_border_size (/ intBorderStyle)
	(setq intBorderStyle (dcl-Control-GetBorderStyle Slider/Form1/pic_slider))
	(cond
		((= intBorderStyle 0) 0)
		((= intBorderStyle 1) 4)
		((= intBorderStyle 2) 2)
		(T 0)
	); cond
); slider_get_border_size


;; --------------------------------------------------------------------------------------
;; Beschreibung: Ergänzt eine neue Zeile im Ereignisprotokoll und scrollt dorthin
;; Argumente: strLine = neue Zeile als Text
;; Rückgabe: <nicht verwendet>

(defun slider_log_event (strLine)
	(if (= 1 (dcl-Control-GetValue Slider/Form1/chb_event))
		(dcl-ListBox-SetTopIndex Slider/Form1/lbx_eventlog (1- (dcl-ListBox-AddString Slider/Form1/lbx_eventlog strLine)))
	); if
); slider_log_event


;|<<OpenDCL Event Handlers>>|;

;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn dcl-Form-show ausgeführt wird. Setzt Vorgabewerte für den Dialog und die Steuerelemente
;; Argumente: keine
;; Rückgabe: <nicht verwendet>

(defun c:Slider/Form1#OnInitialize (/)
;|
	;; Entfernen Sie die Auskommentierung, um die Kombinationsfelder mit den
	;; Inkrementangaben jedes mal zurückzusetzen, wenn der Dialog angezeigt wird.
	(mapcar 'dcl-ComboBox-Clear (list Slider/Form1/cbb_small Slider/Form1/cbb_large))
	(dcl-ComboBox-AddList Slider/Form1/cbb_small (list "1" "2" "5"))
	(dcl-ComboBox-AddList Slider/Form1/cbb_large (list "5" "10" "20"))
	(dcl-ComboBox-SetCurSel Slider/Form1/cbb_small 0)
	(dcl-ComboBox-SetCurSel Slider/Form1/cbb_large 0)
|;
	(dcl-ListBox-Clear Slider/Form1/lbx_eventlog)
	(dcl-ComboBox-SetCurSel Slider/Form1/cbb_border (dcl-Control-GetBorderStyle Slider/Form1/pic_slider))
); c:Slider/Form1#OnInitialize


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn der Anwender im Eingabefeld ENTER drückt, die ESC-Taste drückt oder den Dialog schließt.
;; Argumente: intIsESC = Grund der Dialogbeendigung als Integer; 0, wenn im Eingabefeld ENTER gedrückt wurde
;; Rückgabe: T, um den Dialog nicht zu schließen

(defun c:Slider/Form1#OnCancelClose (intIsESC /)
	(slider_log_event (strcat "OnCancelClose / " (vl-prin1-to-string intIsESC)))
	(/= intIsESC 1)
); c:Slider/Form1#OnCancelClose


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn sich der Textinhalt geändert hat
;; Argumente: strNewValue = neuer Textwert als Text
;; Rückgabe: <nicht verwendet>

(defun c:Slider/Form1/edt_percent#OnEditChanged (strNewValue / intPercent)
	(slider_log_event (strcat "OnEditChanged / " strNewValue))
	(setq intPercent (atoi strNewValue))
	(cond
		((< intPercent 0) (dcl-Control-SetText Slider/Form1/edt_percent "0"))
		((> intPercent 100) (dcl-Control-SetText Slider/Form1/edt_percent "100"))
	); cond
	(dcl-PictureBox-Refresh Slider/Form1/pic_slider)
); c:Slider/Form1/edt_percent#OnEditChanged


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn der Anwender den Stil des Schiebereglers ändert
;; Argumente: intStyle = Index als Integer
;;            strValue = Bezeichnung als Text
;; Rückgabe: <nicht verwendet>

(defun c:Slider/Form1/cbb_style#OnSelChanged (intStyle strValue /)
	(dcl-PictureBox-Refresh Slider/Form1/pic_slider)
); c:Slider/Form1/cbb_style#OnSelChanged


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn der Anwender den Stil des Rands ändert
;; Argumente: intBorder = Index als Integer
;;            strValue = Bezeichnung als Text
;; Rückgabe: <nicht verwendet>

(defun c:Slider/Form1/cbb_border#OnSelChanged (intBorder strValue /)
	(dcl-Control-SetBorderStyle Slider/Form1/pic_slider intBorder)
); c:Slider/Form1/cbb_border#OnSelChanged


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, sobald das Bild-Steuerelement neugezeichnet werden muss
;; Argumente: isHasFocus = T, wenn das Bildsteuerlement den Fokus hat
;; Rückgabe: <nicht verwendet>

(defun c:Slider/Form1/pic_slider#OnPaint (isHasFocus / intPos intStyle lstPos lstLines intLeft)
	(slider_log_event (strcat "OnPaint / " (vl-prin1-to-string isHasFocus)))
	(setq intPos (slider_get_pos (atoi (dcl-Control-GetText Slider/Form1/edt_percent))))
	(setq lstPos (dcl-Control-GetPos Slider/Form1/pic_slider))
	(setq intStyle (dcl-ComboBox-GetCurSel Slider/Form1/cbb_style))
	(cond
		(	(or (not intStyle) (<= intStyle 0)) ; Standard-Regler
			(dcl-PictureBox-PaintPicture Slider/Form1/pic_slider (list (list 0 intPos 100 T nil))))

	(	(= intStyle 1) ; Einfaches gefülltes Rechteck
		(dcl-PictureBox-DrawSolidRect Slider/Form1/pic_slider
			(list
				(list 0 intPos (caddr lstPos) 2 0)
				(list 0 (+ intPos 2) (caddr lstPos) 8 171)
				(list 0 (+ intPos 10) (caddr lstPos) 2 0))))

	(	(= intStyle 2) ; Füllstandsanzeige
		(dcl-PictureBox-DrawSolidRect Slider/Form1/pic_slider
			(list (list 0 intPos (caddr lstPos) (- (cadddr lstPos) intPos) 171))))

	(	(= intStyle 3) ; Füllendes Dreieck
		(repeat (1+ (setq intLen (1- (caddr lstPos))))
			(setq lstLines (cons (list 0 (last lstPos) (setq intLen (1- intLen)) 0 163) lstLines))
		); repeat
		(dcl-PictureBox-DrawLine Slider/Form1/pic_slider lstLines)
		(dcl-PictureBox-DrawSolidRect Slider/Form1/pic_slider
			(list (list 0 intPos (caddr lstPos) 4 0))))

	(	(= intStyle 4) ; Knopf
		(setq intLeft (/ (caddr lstPos) 2))
		(dcl-PictureBox-DrawSolidRect Slider/Form1/pic_slider
			(list (list intLeft 0 1 (last lstPos) 8)))
		(dcl-PictureBox-PaintPicture Slider/Form1/pic_slider
			(list (list (- intLeft 8) intPos 101 T nil))))
	); cond
); c:Slider/Form1/pic_slider#OnPaint


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn sich die Maus über dem Bild bewegt
;; Argumente: keine
;; Rückgabe: <nicht verwendet>
;; Hinweis: To make sure that picturebox has focus for OnKeyPressed event and mousewheel

(defun c:Slider/Form1/pic_slider#OnMouseEntered (/)
	(dcl-Control-SetFocus Slider/Form1/pic_slider)
); c:Slider/Form1/pic_slider#OnMouseEntered


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn der Anwender eine Maustaste auf dem Steuerelement klickt
;; Argumente: intButton = Schaltfläche als Integer
;;            intFlags = Summe der einzelnen Bitwerte als Integer
;;            intX = X-Koordinate der Mausposition innerhalb des Bild-Steuerelements als Integer
;;            intY = Y-Koordinate der Mausposition innerhalb des Bild-Steuerelements als Integer
;; Rückgabe: <nicht verwendet>
;; Hinweis: Das Ereignis MouseDblClick ruft die selbe Funktion auf wie das Ereignis MouseDown.
;;	    Beide rufen daher diese Funktion auf.
;;          Linke Maustaste über dem Reglerknopf = großer Schritt nach oben
;;          Linke Maustaste unter dem Reglerknopf = großer Schritt nach unten
;;          Linke Maustaste auf dem Reglerknopf = direkt zum Zielpunkt
;;          Rechte Maustaste irgendwohin = direkt zum Zielpunkt

(defun c:Slider/Form1/pic_slider#OnMouseDown (intButton intFlags intX intY / intPercent intTop intButtonHeight)
	(slider_log_event (strcat "OnMouseDown / " (vl-prin1-to-string (list intButton intFlags intX intY))))
	(setq intPercent (atoi (dcl-Control-GetText Slider/Form1/edt_percent)))
	(setq intTop (slider_get_pos intPercent))
	(setq intButtonHeight (slider_get_button_height))
	(cond
		(	(/= intButton 1)
			(slider_change_percent (slider_get_percent (- intPos (/ intButtonHeight 2)))))
		(	(< intY intTop)
			(slider_change_percent (+ intPercent (slider_get_large_increment))))
		(	(< intY (+ intTop intButtonHeight))
			(slider_start_dragging))
		(T
			(slider_change_percent (- intPercent (slider_get_large_increment))))
	); cond
); c:Slider/Form1/pic_slider#OnMouseDown


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird ausgeführt, wenn die Maustaste über dem Steuerelement losgelasen wird
;; Argumente: intButton = Schaltfläche als Integer
;;            intFlags = Summe der einzelnen Bitwerte als Integer
;;            intX = X-Koordinate der Mausposition innerhalb des Bild-Steuerelements als Integer
;;            intY = Y-Koordinate der Mausposition innerhalb des Bild-Steuerelements als Integer
;; Rückgabe: <nicht verwendet>

(defun c:Slider/Form1/pic_slider#OnMouseUp (intButton intFlags intX intY / intPercent intTop intButtonHeight)
	(slider_log_event (strcat "OnMouseUp / " (vl-prin1-to-string (list intButton intFlags intX intY))))
	(slider_stop_dragging)
); c:Slider/Form1/pic_slider#OnMouseUp


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird ausgeführt, wenn der Anwender die Maus über dem Steuerelement bewegt
;; Argumente: intFlags = Summe der einzelnen Bitwerte als Integer
;;            intX = X-Koordinate der Mausposition innerhalb des Bild-Steuerelements als Integer
;;            intY = Y-Koordinate der Mausposition innerhalb des Bild-Steuerelements als Integer
;; Rückgabe: <nicht verwendet>
;; Hinweis: Wird ausgeführt, wenn der Reglerknopf mit gedrückter Maustaste über dem Steuerelement bewegt wird

(defun c:Slider_Form1_pic_slider_OnMouseMove (intFlags intX intY /)
	;; Das Protokoll der OnMouseMove-Ereignisse kann die Liste leicht überfüllen
	;(slider_log_event (strcat "OnMouseMove / " (vl-prin1-to-string (list intFlags intX intY))))
	(if (= (logand intFlags 1) 1)
		(slider_change_percent (slider_get_percent (- intY (/ (slider_get_button_height) 2))))
	); if
); c:Slider_Form1_pic_slider_OnMouseMove


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn der Anwender mit dem Mausrad scrollt
;; Argumente: intFlags = Summe der einzelnen Bitwerte als Integer
;;            intZDelta = number of lines which are scrolled, negative, when down
;;            intX = X-Koordinate der Mausposition innerhalb des Bild-Steuerelements als Integer
;;            intY = Y-Koordinate der Mausposition innerhalb des Bild-Steuerelements als Integer
;; Rückgabe: <nicht verwendet>
;; Hinweis: Herunterscrollen = Kleiner Schritt runter
;;          Hochscrollen = Kleiner Schritt nach oben
;;          Wird beim Scrollen die Umschalttaste gehalten, wird in großen Schritten gescrollt

(defun c:Slider/Form1/pic_slider#OnMouseWheel (intFlags intZDelta intX intY / intPercent intInc)
	(slider_log_event (strcat "OnMouseWheel / " (vl-prin1-to-string (list intFlags intZDelta intX intY))))
	(setq intInc (if (zerop (logand intFlags 4)) (slider_get_small_increment) (slider_get_large_increment)))
	(setq intPercent (atoi (dcl-Control-GetText Slider/Form1/edt_percent)))
	(slider_change_percent (+ intPercent (fix (* intZDelta intInc 0.01))))
); c:Slider/Form1/pic_slider#OnMouseWheel


;; --------------------------------------------------------------------------------------
;; Ereignis: Wird aufgerufen, wenn eine Taste gedrückt wurde
;; Argumente: strCharacter = Zeichen als Text
;;            intRepeatCount = Anzahl der Wiederholungen als Integer
;;            intFlags = Summe der einzelnen Bitwerte als Integer
;; Rückgabe: <nicht verwendet>
;; Hinweis: Bild auf = großer Schritt nach oben
;;          Bild runter = großer Schritt nach unten
;;          Ende = Ende
;;          Pos1 = Anfang
;;          Links oder Hoch = kleiner Schritt nach oben
;;          Rechts oder Runter = kleiner Schritt nach unten

(defun c:Slider/Form1/pic_slider#OnKeyDown (strCharacter intRepeatCount intFlags / intChar intSmall intLarge)
	(setq intChar (ascii strCharacter))
	(slider_log_event (strcat "OnKeyDown / " strCharacter "{" (itoa intChar) "} " (itoa intFlags)))
	(setq intPercent (atoi (dcl-Control-GetText Slider/Form1/edt_percent)))
	(if (= (logand intFlags 256) 256)
		(cond
			((= intChar 33) (slider_log_event " \=> Bild auf") (slider_change_percent (+ intPercent (slider_get_large_increment))))
			((= intChar 34) (slider_log_event " \=> Bild ab") (slider_change_percent (- intPercent (slider_get_large_increment))))
			((= intChar 35) (slider_log_event " \=> Ende") (slider_change_percent 100))
			((= intChar 36) (slider_log_event " \=> Pos1") (slider_change_percent 0))
			((= intChar 37) (slider_log_event " \=> Links") (slider_change_percent (+ intPercent (slider_get_small_increment))))
			((= intChar 38) (slider_log_event " \=> Hoch") (slider_change_percent (+ intPercent (slider_get_small_increment))))
			((= intChar 39) (slider_log_event " \=> Rechts") (slider_change_percent (- intPercent (slider_get_small_increment))))
			((= intChar 40) (slider_log_event " \=> Runter") (slider_change_percent (- intPercent (slider_get_small_increment))))
		); cond
	); if
); c:Slider/Form1/pic_slider#OnKeyDown


;; --------------------------------------------------------------------------------------
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
	"Slider"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
