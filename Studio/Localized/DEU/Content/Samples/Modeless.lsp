;;;
;;; Beispiel einer nicht-modalen Dialogbox
;;;
;;; Dieses Beispiel demonstriert die Anwendung einer nicht-modalen Dialogbox und ihren Ereignissen.
;;;

;; Hauptprogramm
(defun c:Modeless (/ cmdecho)

  ;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Projekt laden
  (dcl-Project-Load (*ODCL:Samples-FindFile "Modeless.odcl"))

  ;; Dialog anzeigen
  (dcl-Form-Show Modeless/DemoModeless)
  (dcl-Form-Enable Modeless/DemoModeless T)

  ;; Dies ist ein nicht-modaler Dialog. Das bedeutet, dass (dcl-Form-Show)
  ;; sofort die Kontrolle zurückgibt und das Programm weiterläuft, während
  ;; der Dialog aktiv ist.
  ;; Die Ereignisfunktionen übernehmen erst nachher die Kontrolle und müssen
  ;; daher global definiert sein.

  (princ)
)

;|<<OpenDCL Ereignisfunktionen>>|;
;; Um den Lispausdruck "Command" ausführen zu können,
;; muss im ODCL-Projekt im Feld "Event Invoke" die
;; Option "Als Befehl ausführen" gewählt sein.

(defun c:DemoModeless_cmdDrawLine_OnClicked ( / strOldText ptStart ptEnde)
  (setq strOldText (dcl-Control-GetCaption Modeless/DemoModeless/Label1))
  (dcl-Control-SetCaption Modeless/DemoModeless/Label1 "Picken Sie jetzt zwei Punkt am Bildschirm. Der Dialog bleibt geöffnet, ist jedoch inaktiv!")
  (dcl-Form-Enable Modeless/DemoModeless nil)
  (if (and (setq ptStart (vl-catch-all-apply 'getpoint (list "\nStartpunkt: ")))
           (not (vl-catch-all-error-p ptStart))
           (setq ptEnde (vl-catch-all-apply 'getpoint (list ptStart "\nZielpunkt: ")))
           (not (vl-catch-all-error-p ptEnde)))
    (progn
      (command "_LINE" ptStart ptEnde "")
      (command "_ZOOM" "_W" ptStart ptEnde)
    ); progn
  ); if
  (dcl-Form-Enable Modeless/DemoModeless T)
  (dcl-Control-SetCaption Modeless/DemoModeless/Label1 strOldText)
  (princ)
)

(defun c:DemoModeless_cmdZoomWin_OnClicked ()
  (command "_ZOOM" "_W" PAUSE PAUSE)
  (princ)
)

(defun c:DemoModeless_cmdZoomExt_OnClicked ()
  (command "_ZOOM" "_E")
  (princ)
)

(defun c:DemoModeless_CloseButton_Clicked ()
  (dcl-Form-Close Modeless/DemoModeless)
  (princ)
)

(princ)

;|<<OpenDCL Beispiel Abschluss>>|;

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
  "Modeless"
)
(princ)

;;;######################################################################
;;;######################################################################
