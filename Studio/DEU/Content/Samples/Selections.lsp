;;;
;;; Auswahl Beispiel
;;;
;; Dieses Beispiel zeigt dem Umgang mit modalen Dialogen, die geschlossen werden,
;; um am Bildschirm einen Punkt zu picken, um hernach wieder am Bildschirm zu
;; erscheinen und den aktuellen Status aufgrund der Anwenderinteraktion abzubilden.
;; Drückt der Anwender die Schaltfläche ESC während der Interaktion mit AutoCAD,
;; wird der Dialog nicht wieder eingeblendet.

;; Hauptprogramm
(DEFUN c:Sel (/

              ;; lokale Variablen
              lstPoints lstObjects doContinue intResult

              ;; lokale Funktionen
              point_selection object_selection

              ;; lokale Ereignisse
              ;; da Funktionen mit modalen Dialogen an der Stelle (dcl_form_show ...)
              ;; angehalten werden, werden die Ereignisse nur in dieser Funktion
              ;; benötigt und können daher lokal definiert werden

              c:Selections_Form_OnInitialize
              c:Selections_Form_Close_OnClicked
              c:Selections_Form_PickPointButton_OnClicked
              c:Selections_Form_PickObjectButton_OnClicked
              c:Selections_Form_PointListBox_OnDblClicked
              c:Selections_Form_ObjectListBox_OnDblClicked
              )

    

    ;|<<Zusätzliche Funktionen>>|;


    ;; Punktwahlfunktion

    (defun point_selection (/ intBlip ptPoint strPoint doSel)

        ;; Punktmarkierungen aktivieren
        (setq intBlip (getvar "BLIPMODE"))
        (setvar "BLIPMODE" 1)

        (setq doSel T)
        (while doSel

            ;; Sicheres Punktpicken
            (setq ptPoint (vl-catch-all-apply 'getpoint (list "\nPunkt picken (oder ENTER, um zum Dialog zurückzukehren): ")))

            (cond
                ;; ENTER wurde gedrückt, doContinue auf T setzen,
                ;; damit der Dialog wieder angezeigt wird
                ((not ptPoint)
                 (setq ptPoint nil
                       doSel nil
                       doContinue T))

                ;; ESC wurde gedrückt, doContinue auf nil setzen,
                ;; damit der Dialog nicht wieder angezeigt wird
                ((vl-catch-all-error-p ptPoint)
                 (setq ptPoint nil
                       doSel nil
                       doContinue nil))

                ;; Punkt zu Zeichenkette wandeln
                ((not (setq strPoint (vl-prin1-to-string ptPoint)))
                 (setq ptPoint nil
                       doSel T))

                ;; Punkt zu Liste hinzufügen, wenn er nicht schon drin ist
                ;; dann wieder zum Dialog zurückkehren
                ((not (member strPoint lstPoints))
                 (setq lstPoints (reverse (cons strPoint (reverse lstPoints)))
                       doSel T))
            ); cond
        ); while
        
        ;; Punktmarkierungen zurücksetzen
        (setvar "BLIPMODE" intBlip)
    ); point_selection


    ;; Objektwahlfunktion

    (defun object_selection (/ intBlip ssAusw intLen entObj vlaObj strObj)

        ;; Punktmarkierungen aktivieren
        (setq intBlip (getvar "BLIPMODE"))
        (setvar "BLIPMODE" 1)

        ;; Sicheres Punktpicken
        (princ "\nObjekte wählen (oder ENTER, um zum Dialog zurückzukehren): ")
        (setq ssAusw (vl-catch-all-apply 'ssget nil))
        
        (cond
            ;; ENTER wurde gedrückt, doContinue auf T setzen,
            ;; damit der Dialog wieder angezeigt wird
            ((not ssAusw)
             (setq ssAusw nil
                   doContinue T))

            ;; ESC wurde gedrückt, doContinue auf T setzen,
            ;; damit der Dialog wieder angezeigt wird
            ((vl-catch-all-error-p ssAusw)
             (setq ssAusw nil
                   doContinue nil))

            ;; Prüfen, ob der Auswahlsatz Objekte hat
            ((zerop (setq intLen (sslength ssAusw)))
             (setq ssAusw nil
                   doContinue T))

            ;; Objekte zur Liste hinzufügen, sofern Sie nicht schon drin sind
            ;; dann wieder zum Dialog zurückkehren
            (T (repeat intLen
                   (setq entObj (ssname ssAusw (setq intLen (1- intLen))))
                   (setq vlaObj (vlax-ename->vla-object entObj))
                   (setq strObj (strcat (vla-get-ObjectName vlaObj) " (" (vla-get-Handle vlaObj) ")"))
                   (if (not (member strObj lstObjects))
                       (setq lstObjects (reverse (cons strObj (reverse lstObjects))))
                   ); if
               ); repeat
             (setq doContinue T))
        ); cond
        
        ;; Punktmarkierungen zurücksetzen
        (setvar "BLIPMODE" intBlip)
    ); object_selection

    

    ;|<<OpenDCL Event Handlers>>|;



    ;; Das Ereignis OnInitialize wird bei jedem Mal aufgerufen,
    ;; wenn der Dialog angezeigt werden soll
    
    (defun c:Selections_Form_OnInitialize (/)
        
        ;; Bereinigung, falls nötig
        (dcl_ListBox_Clear Selections_Form_PointListBox)
        (dcl_ListBox_Clear Selections_Form_ObjectListBox)

        ;; Punktliste füllen
        (if lstPoints
            (dcl_ListBox_AddList Selections_Form_PointListBox lstPoints)
        ); if

        ;; Objektliste füllen
        (if lstObjects
            (dcl_ListBox_AddList Selections_Form_ObjectListBox lstObjects)
        ); if
        
    ); c:Selections_Form_OnInitialize

    
    
    ;; Das Ereignis wird ausgelöst, drückt der Anwender auf die Schaltfläche Schließen
    ;; Dabei wird der Wert 1 zurückgegeben, der als Rückgabewert von dcl_form_show
    ;; dient (siehe dort)
    
    (defun c:Selections_Form_Close_OnClicked (/)
        (dcl_form_close Selections_Form 1)
    ); c:Selections_Form_Close_OnClicked

    
    
    ;; Das Ereignis wird ausgelöst, drückt der Anwender auf die Schaltfläche Punktwahl
    ;; Dabei wird der Wert 3 zurückgegeben, der als Rückgabewert von dcl_form_show
    ;; dient (siehe dort)
    
    (defun c:Selections_Form_PickPointButton_OnClicked (/)
        (dcl_form_close Selections_Form 3)
    ); c:Selections_Form_PickPointButton_OnClicked

    
    
    ;; Das Ereignis wird ausgelöst, drückt der Anwender auf die Schaltfläche Objektwahl
    ;; Dabei wird der Wert 4 zurückgegeben, der als Rückgabewert von dcl_form_show
    ;; dient (siehe dort)
    
    (defun c:Selections_Form_PickObjectButton_OnClicked (/)
        (dcl_form_close Selections_Form 4)
    ); c:Selections_Form_PickObjectButton_OnClicked

    
    
    ;; Das Ereignis wird ausgelöst, klickt der Anwender einen Listeneintrag doppelt
    
    (defun c:Selections_Form_PointListBox_OnDblClicked (/ intRow)
        (if (not (minusp (setq intRow (dcl_ListBox_GetCurSel Selections_Form_PointListBox))))
            (progn
                
                ;; Element aus Liste und Listenfeld löschen
                
                (setq lstPoints (vl-remove (dcl_ListBox_GetItemText Selections_Form_PointListBox intRow) lstPoints))
                (dcl_ListBox_DeleteItem Selections_Form_PointListBox intRow)
            ); progn
        ); if
    ); c:Selections_Form_PointListBox_OnDblClicked

    
    
    ;; Das Ereignis wird ausgelöst, klickt der Anwender einen Listeneintrag doppelt
    
    (defun c:Selections_Form_ObjectListBox_OnDblClicked (/ intRow)
        (if (not (minusp (setq intRow (dcl_ListBox_GetCurSel Selections_Form_ObjectListBox))))
            (progn

                ;; Element aus Liste und Listenfeld löschen
                
                (setq lstObjects (vl-remove (dcl_ListBox_GetItemText Selections_Form_ObjectListBox intRow) lstObjects))
                (dcl_ListBox_DeleteItem Selections_Form_ObjectListBox intRow)
            ); progn
        ); if
    ); c:Selections_Form_ObjectListBox_OnDblClicked

    ;; COM laden
    (vl-load-com)

    ;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
    (setq cmdecho (getvar "CMDECHO"))
    (setvar "CMDECHO" 0)
    (command "_OPENDCL")
    (setvar "CMDECHO" cmdecho)

    ;; Projekt laden
    (dcl_Project_Load (*ODCL:Samples:FindFile "Selections.odcl"))

    ;; Der wiederholte Aufruf einer modalen Dialogbox wird mit Hilfe einer
    ;; while-Schleife am einfachsten erreicht
    (setq doContinue T)
    (while doContinue
        ;; Um jedoch eine Endlosschleife zu vermeiden, ist die Bedingung
        ;; zur Wiederholung zu allererst zu negieren
        ;; Sie (die Bedingung) wird erst im weiteren Programmablauf
        ;; unter bestimmten Voraussetzungen wieder aktiviert.
        (setq doContinue nil)

        ;; wird der Dialog geschlossen, gibt die Funktion einen Wert zurück
        ;; Dies ist entweder 1 für OK, 2 für ESC oder Abbruch oder aber der
        ;; Wert, dem der Funktion dcl_form_close mitgegeben wurde.
        (setq intResult (dcl_form_show Selections_Form))

        ;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
        ;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
        ;; bis der modale Dialog geschlosswen wird.
        ;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

        ;; Nun können die Werte von intResult ausgewertet werden
        (cond
            
            ;; Schließen-Schaltfläche
            ;; Hier könnte was passieren mit den gewählten Punkten und Objekten
            ((= intResult 1) (setq doContinue nil))

            ;; ESC-Taste
            ((= intResult 2) (setq doContinue nil))

            ;; Punktwahl
            ((= intResult 3) (point_selection))

            ;; Objektwahl
            ((= intResult 4) (object_selection))

        ); cond
    ); while

    (redraw)
    (princ)
); c:Sel

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
	"Sel"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
