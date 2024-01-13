(defun c:DistSample2

    ;;==========================================================================
    ;;
    ;;  DistSample2.lsp
    ;;
    ;;  Einfaches Beispiel, wie mit einem hardcodierten eingebetteten OpenDCL-Projekt verfahren wird.
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
    ;;  2.  Das OpenDCL-Projekt DistSample.odcl wurde als DistSample.odcl.lsp
    ;;      (Base64-codierter Datenstrom, umgebrochen als Lisp-Quelle) gespeichert.
    ;;      Das Projekt beinhaltet einen Dialog in der Variable DistSample/MainForm
    ;;      und einer darin enthaltenen Schaltfläche in der Variable DistSample/MainForm/OkButton,
    ;;      das das OnClick-Ereignis abfängt. Das Projekt ist nicht passwortgeschützt.
    ;;
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Hinweis:
    ;;
    ;;  1.  Beachten Sie die Funktion _Load_ODCL_Stream die den Base64-codierten
    ;;      Datenstrom der OpenDCL-Projektdatei lädt.
    ;;
    ;;==========================================================================

    (	/

        ;;  Lokale Variablen

        c:DistSample/MainForm/OkButton#OnClicked
        _Load_ODCL_Stream
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Stream ( password alias / project rtype )

        ;;  Das ist eine formatierte Kopie des Inhalts der Datei DistSample.odcl.lsp.
        ;;  Die einzige Art der Formatierung bestand darin, dass alle Zeilen auf eine
        ;;  einheitliche Länge gebracht wurden

        (setq project
            '("YWt6A5QaAAB4rwI7BuKTJjUxLT9qgLSNNMQWNjYi6H4s3XyMUlDyPxw+JT4y9g85Ybbdyni/uTRs"
              "aVCixQyXrBmcXQ+XNAmAfkmQMmobmzjbABkQBRGRV/x6a3IJEBiQAg2w4ucRsEcN5fiMFQdqFKNe"
              "GA5n9Ub5N7KdbEw2XX6eE1MuuclWS4ld91Yslj5xxK/3XdJXiPDMqbP3Q+YcEeyMQDDpz1a5rJbc"
              "IHmv6u9akzuNHM9tw6o3LuDaGEGZ2vKDAE3F8UPPY9tCFFL0IFkFo6o0DjhRKZXcMFv8zCXFkvlr"
              "PjtvD2dVMMtnGg90R1Lu4YtkizsHP69kOWs09At98LtxotIrkZwFXAGfHN2BMp+ZdEErhsXF4HbS"
              "JdZTnOqeWHH7tlbj7Fj7P+I68llPOFxHsgwS8BB1ZQQfXAfZV3Ud/ASZ48/1AlkmaYc2FcOPpvlr"
              "D4kJ/0OVeRT0pU2MxXQhL4Zr1DWWvgTZmilr0HcGj0lbwdjOm6txKNyWxb0bgGCb33Iet+6z74IE"
              "gND+m4YcqpsALg7zqaJ/ZDXlrTHnqATSHJv3jBvjvCfxwx7iF2gY49jLcuFOw+Tf+TQd0juYoy8I"
              "ZCKHC78dUABRhtLsASpAEA+bbVxhD4/2nk1UOB10uWtA0058oIOUZhUrAjQcECQOrp2NNmy5zqC7"
              "qnDjrpOuUBGcNujH7N8g7MxKAxkr1eYF2hyS/7xxa8DBN7IZYgBI4C0GoJ9lhlQJNDC3GWvA06gl"
              "D7CtuBXQwEexsRSAWZkwzw/N5IG7o/FKP+8oZKLYDRQh8jngTn8lQoKzzNSaeCNryCuwF6eM7NK1"
              "3FKVJPnG9gPtt8wtGDdFgbzydo+USgW9EmlGVIdaSaXs5+1ID0wRyhvKk5GbkjXpu/SEMEr24z1Q"
              "FeO9DCrbR0JgjpjOAgZJtJ84Bg2GWIA+cBFfGfl8G0eFMQ2ykhn9YiWj+0cYEr6AuCkcRmjeq1RA"
              "X0CiEMCvyix4nMP794zwzuAec2dlHyVuPCcn2KX0KzRXgOMkEI+BVLVAAeGBYJjhoRinl4MRyZqb"
              "dvSrWMrbxK2FoRCPh4WCA4BAAeHBsaGZkY2Jh5FTgH2PnfvgwsOBZKYBOlKBHJgQho+BbeoaF1g="
             )
        )

        ;;  Mit dcl-project-load wird das Projekt geladen und der Rückgabewert
        ;;	an die aufrufende Funktion übergeben, wenn der Vorgang erfolgreich war. 
        (dcl-project-import project password alias)
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
                (_Load_ODCL_Stream nil "DistSample")

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
  "DistSample2"
)
(princ)

;;;######################################################################
;;;######################################################################
