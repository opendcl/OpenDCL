;;;
;;; Kopieren/Einfügen Beispielen in Listenfeldern
;;;
;;; Dieses Beispiel demonstriert die Kopieren/Einfügen-Funktionalität in Listenfeldern.
;;;

;; Hauptprogramm
(defun c:CopyPaste (/ cmdecho)

  ;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Projekt laden
  (dcl-Project-Load (*ODCL:Samples-FindFile "ListBoxCopyPaste.odcl"))

  ;; Dialog anzeigen
  (dcl-Form-Show ListBoxCopyPaste/Form)

  ;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
  ;; Zeile stehen bleibt und (dcl-Form-Show) solange keinen Wert zurückgibt,
  ;; bis der modale Dialog geschlosswen wird.
  ;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

  (princ)
)

;|«OpenDCL Event Handlers»|;

(defun c:ListBoxCopyPaste/Form/TextButton1#OnClicked (/ items len DelLst x cnt)
    
  ;; Auswahl aus dem linken Listenfeld holen
  (setq items (dcl-ListBox-GetSelectedItems ListBoxCopyPaste/Form/ListBox1))
  (setq len (length items))
  (if (> len 0)
    ;; Wenn mindestens ein Element markiert ist...
    (progn
      ;; Status des Kontrollkästchens abfragen und wenn aktiv,
      ;; Element(e)zur rechten Seite hinzufügen
      (if (/= 1 (dcl-Control-GetValue ListBoxCopyPaste/Form/CheckBox1))
        (dcl-ListBox-Clear ListBoxCopyPaste/Form/ListBox2)
      )
      ;; ausgewählte Elemente zur rechten Seite hinzufügen
      (dcl-ListBox-AddList ListBoxCopyPaste/Form/ListBox2 items)
      ;; Status des Kontrollkästchens abfragen und wenn aktiv,
      ;; Elemente aus der linken Liste entfernen
      (if (= 1 (dcl-Control-GetValue ListBoxCopyPaste/Form/CheckBox2))
        (progn
          (Setq DelLst (dcl-ListBox-GetSelectedNths
                   ListBoxCopyPaste/Form/ListBox1
                 )
          )
          (Setq cnt 0)
          (while (< cnt (length DelLst))
            (setq x (nth cnt DelLst))
            (dcl-ListBox-DeleteString ListBoxCopyPaste/Form/ListBox1
                          (- x cnt)
            )
            (setq cnt (1+ cnt))
          )
        )
      )
    )
  )
)

;; Klickt der Anwender einen Eintrag in der linken Liste doppelt,
;; wird die gleiche Aktion ausgeführt, als hätte er die Schaltfläche gedrückt...
(defun c:ListBoxCopyPaste/Form/ListBox1#OnDblClicked (/)
  ;; run the function above...
  (c:ListBoxCopyPaste/Form/TextButton1#OnClicked)
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
  "CopyPaste"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
