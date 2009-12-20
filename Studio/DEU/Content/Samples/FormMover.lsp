;;;
;;; Dialogpositionierung Beispiel
;;;
;;; Dieses Beispiel demonstriert die Dialogpositionierung und -bewegung zur Laufzeit.
;;; Für die zeitliche Verzögerung kommt (dcl_DelayedInvoke) zum Einsatz, um einen
;;; animierten Effekt zu erzielen.

;; Hauptprogramm
(defun c:Mover (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "FormMover.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show FormMover_Form1)
	(dcl_Form_Resize FormMover_Form1 300 200)
	(dcl_Form_Center FormMover_Form1)
	(c:FormMover_Form1_GraphicButton1_OnClicked)

	;; Dies ist ein nicht-modaler Dialog. Das bedeutet, dass (dcl_Form_Show)
	;; sofort die Kontrolle zurückgibt und das Programm weiterläuft, während
	;; der Dialog aktiv ist.
	;; Die Ereignisfunktionen übernehmen erst nachher die Kontrolle und müssen
	;; daher global definiert sein.

	(princ)
)

;; Diese Funktion bewegt den Dialog zu den 4 Bildschirmecken und abschließend in die Bildschirmmitte.
(defun C:FormMover ( / ce FormSize delay wd ht ScreenSize ScreenX ScreenY)
	(Setq FormSize (dcl_Form_GetRectangle FormMover_Form1))
	(setq wd (caddr FormSize))
	(setq ht (cadddr FormSize))
	(setq ScreenSize (dcl_GetScreenSize))
	(setq ScreenX (car ScreenSize))
	(setq ScreenY (cadr ScreenSize))

	(cond
		(	(= *FormPos* 0)
			(dcl_Form_SetPos FormMover_Form1 0 0);_TL
		)
		(	(= *FormPos* 1)
			(dcl_Form_SetPos FormMover_Form1 0 (- ScreenY ht));_ BL
		)
		(	(= *FormPos* 2)
			(dcl_Form_SetPos FormMover_Form1 (- ScreenX wd) (- ScreenY ht));_ BR
		)
		(	(= *FormPos* 3)
			(dcl_Form_SetPos FormMover_Form1 (- ScreenX wd) 0);_ TR
		)
		(	(= *FormPos* 4)
			(dcl_Form_Center FormMover_Form1)
		)
	);_ cond

	(setq *FormPos* (1+ *FormPos*));_ bump the form position count
	(if (<= *FormPos* 4)
		(dcl_DelayedInvoke 800 "C:FormMover");_ repeat
	)
	(princ)
)

;; Diese Funktion bewegt den Dialog zur linken unteren Bildschirmecke und skaliert ihn dabei.
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

;|<<OpenDCL Event Handlers>>|;

(defun c:FormMover_Form1_GraphicButton1_OnClicked ( /)
	(setq *FormPos* 0)
	(C:FormMover)
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

(defun c:FormMover_Form1_cmdClose_OnClicked ( /); Schließen
	(dcl_Form_Close FormMover_Form1)
)

;; Erweitern oder Verkleinern
(defun c:FormMover_Form1_cmdExpand_OnClicked ( / ); Erweitern
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
	"Mover"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
