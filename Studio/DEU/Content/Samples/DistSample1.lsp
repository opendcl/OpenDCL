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
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Voraussetzungen:
    ;;
    ;;  1.  AutoCAD 2004+, OpenDCL 5.0 oder neuer.
    ;;
    ;;  2.  DistSample.odcl | .lsp. mit einem Dialog in der Variable DistSample_MainForm
    ;;      und einer darin enthaltenen Schaltfläche in der Variable DistSample_MainForm_OkButton,
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

        c:DistSample_MainForm_OkButton_OnClicked
        _Load_ODCL_Runtime
        _Load_ODCL_Embedded_Project
        _Load_ODCL_File_Project
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Runtime ( / )

        (or

            ;;  OpenDCL ist bereits geladen, mit T beenden

            dcl_getversionex

            ;;  Ist das Laden nach Bedarf aktiviert (DEMANDLOAD), nutze das OPENDCL-Kommando
            ;;  um die OpenDCL-Laufzeitumgebung zu laden. Ist das Laden nach Bedarf deaktiviert,
            ;;  nehmen wir an, dass dies gewünscht ist und laden deshalb nicht dazu.

            (and

                ;;  Ist das Laden nach Bedarf aktiviert

                (= 2 (boole 1 (getvar "DEMANDLOAD") 2))

                ;;  dann lade die OpenDCL Laufzeitumgebung

                (vl-catch-all-apply 'vl-cmdf '("OPENDCL"))

                ;;  Und prüfe, ob die Funktionen nun geladen sind

                dcl_getversionex
            )

            ;;  Konnte oder sollte die Laufzeitumgebung nicht geladen werden
            ;;  wird der Anwender informiert.

            (princ "\nFehler: OpenDCL-Laufzeitumgebung konnte nicht geladen werden.\n")
        )

        dcl_getversion

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Embedded_Project ( projname password alias / bytes rtype )

        ;;  Versuche das eingebetete OpenDCL-Projekt als Lisp-Quelle aus der Textquelle
        ;;  der aktuellen VLX auszulesen. War dies erfolgreich, muss das Projekt mit der
        ;;  Funktion dcl_project_import geladen und der Rückgabewert an die aufrufende
        ;;  Funktion übergeben werden

        (cond

            ;;  An diesem Punkt muss die OpenDCL-Laufzeitumgebung bereits geladen
            ;;	sein. Wenn nicht, ist zuvor die Initialisierung fehlgeschlagen oder
            ;;	es ist eine ältere OpenDCL-Laufzeitumgebung geladen worden.
            ;;  So oder so, an dieser Stell gehts nicht weiter...

            (	(null dcl_project_import)

                (princ "\nFür diese Funktion wird die Laufzeitumgebung von OpenDCL 5 oder höher vorausgesetzt.\n")

                nil
            )

            ;;  Prüfe den Zugriff auf die Projektdaten innerhalb der Textquelle in der
            ;;  VLX und gebe eine Meldung aus, wenn dies fehlgeschalgen ist

            (	(or
                    (null (setq bytes (vl-get-resource projname)))
                    (not (eq 'str (setq rtype (type bytes))))
                    (eq "" bytes)
                )

                (princ
                    (strcat
                        "\nKann die Quell für das OpenDCL-Projekt <"
                        projname
                        "> nicht aus der VLX-Datei laden.\n"
                    )
                )

                nil

            )

            ;;  Mit dcl_project_load wird das Projekt geladen und der Rückgabewert
            ;;	an die aufrufende Funktion übergeben, wenn der Vorgang erfolgreich war. 

            (	(dcl_project_import bytes password alias)  )
        )

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_File_Project ( projname reload password alias / samples )

        ;;  An dieser Stelle wird versucht, eine OpenDCL-Projektdatei in einem der
        ;; AutoCAD-Supportpfade zu finden und zu laden. Schlägt das fehl, soll sie
        ;; aus dem Ordner der Beispieldateien von OpenDCL Studio geladen werden.

        (cond

            ;;  An diesem Punkt muss die OpenDCL-Laufzeitumgebung bereits geladen
            ;;	sein. Wenn nicht, ist zuvor die Initialisierung fehlgeschlagen oder
            ;;	es ist eine ältere OpenDCL-Laufzeitumgebung geladen worden.
            ;;  So oder so, an dieser Stell gehts nicht weiter...

            (	(null dcl_project_load)

                (princ "\nFür diese Funktion wird die Laufzeitumgebung von OpenDCL 5 oder höher vorausgesetzt.\n")

                nil
            )

            ;;  Mit dcl_project_load wird das Projekt geladen und der Rückgabewert
            ;;	an die aufrufende Funktion übergeben, wenn der Vorgang erfolgreich war. 

            (	(dcl_project_load projname reload password alias)  )

            ;;  Da diese Datei neben anderen Beispieldateien installiert wurde und
            ;;	dieser normalerweise nicht in den AutoCAD-Supportpfade eingetragen
            ;;	ist, wird versucht die Datei von dort zu laden

            ;;  Löschen Sie den folgenden Abschnitt, wenn es sich nicht um ein
            ;;	OpenDCL-Beispiel handelt

            (	(setq samples
                    (cond
                        (	(vl-registry-read
                                "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
                                "SamplesFolder"
                            )
                        ) ; 32-bit Variante aktueller Nutzer
                        (	(vl-registry-read
                                "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
                                "SamplesFolder"
                            )
                        ) ; 32-bit Variante alle Nutzer
                        (	(vl-registry-read
                                "HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
                                "SamplesFolder"
                            )
                        ) ; 64-bit Variante aktueller Nutzer
                        (	(vl-registry-read
                                "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
                                "SamplesFolder"
                            )
                        ) ; 64-bit Variante alle Nutzer
                    )
                )

                (dcl_project_load (strcat samples projname) reload password alias)
            )
        )

    ) ;;------------------------------------------------------------------------

    (defun c:DistSample_MainForm_OkButton_OnClicked ( )

        (dcl_MessageBox "Drücken Sie die Schaltfläche OK, um abzubrechen..." "Abbrechen und schließen ...")

        (dcl_form_close DistSample_MainForm)

    ) ;;------------------------------------------------------------------------

    (defun _Main ( / odclProjName )

        ;;  Die Funktionen in dieser Datei laden und ausführen.

        (setq odclProjName "DistSample.odcl") ;; die .LSP-Erweiterung weglassen!!!

        (if

            (and

                (_Load_ODCL_Runtime)

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

                    (_Load_ODCL_File_Project odclProjName t)
                )
            )

            (if
                (null
                    (dcl_Form_Show DistSample_MainForm)
                )

                (princ "\nKann den Dialog nicht anzeigen: DistSample_MainForm\n")
            )
        )

        (princ)

    ) ;;------------------------------------------------------------------------

    ;;==========================================================================
    ;;
    ;;  Hauptfunktion ausführen ...
    ;;
    ;;==========================================================================

    (_Main)

)

(princ "\nOpenDCL DistSample1 (ver 1.3) wurde geladen. Geben Sie den Befehl \"DistSample1\" ein, um ihn auszuführen.\n")

(princ)