(defun c:DistSample1

    ;;==========================================================================
    ;;
    ;;  DistSample1.lsp
    ;;
    ;;  Einfaches Beispiel, das den Umgangmit eingebetteten OpenDCl-Projekte zeigt
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Geschichte:
    ;;
    ;;  1.0 2007/06/xx - Original-Code - Michael Puckett (MP).
    ;;
    ;;  1.1 2007/06/xx - ODCL_* Funktionen wurden in DCL_* umbenannt, um die 
    ;;                   Änderungen der Version OpenDCL 4.0 beta 18+ abzubilden. Owen Wengerd (OW).
    ;;
    ;;  1.2 2007/06/24 - Umbenannte Funktion _Load_ODCL_Runtimes zu _Load_ODCL_Runtime.
    ;;                   Angepasste Funktion _Load_ODCL_Runtime für das Laden nach Bedarf (MP)
    ;;
    ;;  1.3 2009/10/18 - Zugunsten der Koompatibilität mit Bricscad Ladevorgang
    ;;                   mit OPENDCL-Kommando; ergänze Funktionen für die Suche
    ;;                   nach den abhängigen Dateien im Beispielordner von OpenDCL
    ;;                   einige Funktionen umbenannt zugunsten der Konsistenz
    ;;                   mit anderen Beispielen des OpenDCl Studio. (OW)
    ;;
    ;;  1.4 2014/02/10 - Simplified code by removing some error condition
    ;;                   checks and added OpenDCL sample boilerplate. (OW)
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Voraussetzungen:
    ;;
    ;;  1.  AutoCAD 2004+, OpenDCL 5.0 oder neuer.
    ;;
    ;;  2.  DistSample.odcl | .lsp. mit einem Dialog in der Variable DistSample/MainForm
    ;;      und einer darin enthaltenen Schaltfläche in der Variable DistSample/MainForm/OkButton,
    ;;      das das OnClick-Ereignis abfängt. Das Projekt ist nicht passwortgeschützt.
    ;;
    ;;  3.  Wird die LSP-Datei des Beispiels verwendet, wird erwartet, dass die DistSample.odcl
    ;;      im Supportpfad oder aber im Pfad der Beispiel-Dateien gefunden wird
    ;;
    ;;  4.  Wird das Beispiel als VLX-Datei verwendet, wird erwartet, dass die DistSample.odcl.lsp
    ;;      wie oben beschrieben als Textquelle in der VLX eingebettet wurde.
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Hinweis:
    ;;
    ;;  1.  Beachten Sie die Funktion _Load_ODCL_Embedded_Project, die versucht, die OpenDCL-
    ;;      Projektdaten aus der VLX-Textquelle zu laden.
    ;;
    ;;==========================================================================

    (	/

        ;;  Lokale Variablen

        c:DistSample/MainForm/OkButton#OnClicked
        _Load_ODCL_Embedded_Project
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Embedded_Project ( projname password alias / bytes rtype )

        ;;  Versuche das eingebetete OpenDCL-Projekt als Lisp-Quelle aus der Textquelle
        ;;  der aktuellen VLX auszulesen. War dies erfolgreich, muss das Projekt mit der
        ;;  Funktion dcl_project_import geladen und der Rückgabewert an die aufrufende
        ;;  Funktion übergeben werden
        (cond
            ;;  Prüfe den Zugriff auf die Projektdaten innerhalb der Textquelle in der
            ;;  VLX und gebe eine Meldung aus, wenn dies fehlgeschalgen ist
            (	  (or
                    (null (setq bytes (vl-get-resource projname)))
                    (not (eq 'str (setq rtype (type bytes))))
                    (eq "" bytes)
                )

                (princ (strcat "\nKann die Quell für das OpenDCL-Projekt <" projname "> nicht aus der VLX-Datei laden.\n"))
                nil
            )

            ;;  Mit dcl_project_load wird das Projekt geladen und der Rückgabewert
            ;;	an die aufrufende Funktion übergeben, wenn der Vorgang erfolgreich war. 
            (	(dcl-project-import bytes password alias)  )
        )
    ) ;;------------------------------------------------------------------------

    (defun c:DistSample/MainForm/OkButton#OnClicked ( )
        (dcl-form-close DistSample/MainForm)
    ) ;;------------------------------------------------------------------------

    (defun _Main ( / odclProjName )
        ;;  Die Funktionen in dieser Datei laden und ausführen.

        ;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
        (setq cmdecho (getvar "CMDECHO"))
        (setvar "CMDECHO" 0)
        (command "_OPENDCL")
        (setvar "CMDECHO" cmdecho)

        (setq odclProjName "DistSample.odcl") ;; die .LSP-Erweiterung weglassen!!!

        (if
            ;;  Versuche das OpenDCL-Projekt zu laden
            (or
                ;;  ... zunächst aus der eingebetteten Quelle
                (_Load_ODCL_Embedded_Project odclProjName nil nil)

                ;;  ... war das Laden des OopenDCL-Projekts aus der
                ;; VLX-Quelle erfolglos, wird versucht, das Projekt
                ;; aus der separaten *.odcl-Datei zu laden (das kann
                ;; in der Phase der Produktentwicklung interessant
                ;; sein, diese Zeile sollte jedoch vor der Veröffent-
                ;; lichung entfernt werden.)
                (dcl-Project-Load (*ODCL:Samples-FindFile odclProjName))
            )

            (dcl-Form-Show DistSample/MainForm)
        )

        (princ)

    ) ;;------------------------------------------------------------------------

    (_Main) ;; Hauptfunktion ausführen ...
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
  "DistSample1"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
