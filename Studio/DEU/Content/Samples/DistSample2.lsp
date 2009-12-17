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
    ;;--------------------------------------------------------------------------
    ;;
    ;;  Voraussetzungen:
    ;;
    ;;  1.  AutoCAD 2004+, OpenDCL 5.0 oder neuer.
    ;;
    ;;  2.  Das OpenDCL-Projekt DistSample.odcl wurde als DistSample.odcl.lsp
    ;;      (Base64-codierter Datenstrom, umgebrochen als Lisp-Quelle) gespeichert.
    ;;      Das Projekt beinhaltet einen Dialog in der Variable DistSample_MainForm
    ;;      und einer darin enthaltenen Schaltfläche in der Variable DistSample_MainForm_OkButton,
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

        c:DistSample_MainForm_OkButton_OnClicked
        _Load_ODCL_Runtime
        _Load_ODCL_Stream
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

    (defun _Load_ODCL_Stream ( password alias / project rtype )

        ;;  Das ist eine formatierte Kopie des Inhalts der Datei DistSample.odcl.lsp.
        ;;  Die einzige Art der Formatierung bestand darin, dass alle Zeilen auf eine
        ;;  einheitliche Länge gebracht wurden

        (setq project
            '("YWt6AykbAACPzuumBuKTJjUxLT9qgFBxx173ev1VMzR6v9Z6B2gd9j8cPiU+MvYPPd5TjFa7XoYm"
	      "N5y8+wUpUCOEMgJaUCOXsGGeuOP33NKi1qqRshMFkUM7NI5vdwnkxwnARxmwskmZr9jFrNUXzili"
	      "uX6Y7jx8oxY8bOkPt7KJHPSde+K9ZhhH6dr4e54TZy6wyVarC1/4TpwWOEWkrvxdkg/w8AyquPuT"
	      "5p4W8owwUOnHTplsFtsgTa/677oecO1o9VOW3rBGm08UCA8zoqaRTNVFR2ylH7S9uNyWyw6qONMP"
	      "0JaeENOqV7gwT1RZSBNxEnU7JzrvY9uYavfhXIMHLMoh3URVsVakvVb/2adJIv1hBiDFpLuJOoC8"
	      "uziB57yxawDUjgkJQW+myS7ouw+/hQNqZHmX6XpXipxaXMLyX2znvUqZGJbH7t9pXORMgOOZBvoE"
	      "mQ0NaYdgGSGeOeCIZeGDoxW7vlaMUNCvGw//XYV5CBo39nGSxXQhL4ZrNEPaH8EtjNV0AS2G5GFl"
	      "uWldnbu9B/BhmpOKHq/O6LYhtY87zg0wZwqXY2rCvRGQfvNb8mWcmBfpJICdQyg/ijpHxzyf6rEA"
	      "G+qpYRzh2otaQV/ERTepB/3RIKijLwgUIocLvz1QAFGG0uwBipN1xaDr8fYCvrt9/LHWnvNWYiuG"
	      "yrzOgpmLeaGg83uhVoyip96Qv23g7fxVA5+sZr1PEghR5zdPh+yq9S/oHnKm8SoGQC6G37yJddo9"
	      "TLm5v010sZiGJyHbVrNXh8q8yasAtccFCY7U1RGQkc0kgVcp6mIi2agBD7Fdce3w3F9Xt9Fm6/Cb"
	      "b6CMxveTkgNpNDYxayBvvFvHdQ2a4PozX1EfZYTG9y1zG6V3zdvtxF94/0YM7N0AQJlHVddZqw+v"
	      "0jfmruZLpEylipGsg9u1FCcDiar24z2QU5aK6dWN2IHOmzGBdO9pDMVwbSNQ6qV1+PfZwqq8ISAQ"
	      "nHHqaQFHp8mvaiFdYqlXVgMoID+H1Xu+ksEwfyanOn9OSJ+CvFY4AbrMWj01Lrtxs9sVsy0kMcpd"
	      "zS6g85JZptXyn+5d1jwgJYwKZI+f84rToOhujrGI/oGInVHiAdkRi+iICJ/amXSK06B8gH+B/gG+"
	      "QZ5hjnGGeYJ9gKX1ATHKRU3syYEhfZnx9dsh0oJhAIOBerXL8IY="
	    )
        )

        (cond

            ;;  An diesem Punkt muss die OpenDCL-Laufzeitumgebung bereits geladen
            ;;	sein. Wenn nicht, ist zuvor die Initialisierung fehlgeschlagen oder
            ;;	es ist eine ältere OpenDCL-Laufzeitumgebung geladen worden.
            ;;  So oder so, an dieser Stell gehts nicht weiter...

            (	(null dcl_project_import)

                (princ "\nFür diese Funktion wird die Laufzeitumgebung von OpenDCL 5 oder höher vorausgesetzt.\n")

                nil
            )

            ;;  Mit dcl_project_load wird das Projekt geladen und der Rückgabewert
            ;;	an die aufrufende Funktion übergeben, wenn der Vorgang erfolgreich war. 

            (	(dcl_project_import project password alias)   )
        )

    ) ;;------------------------------------------------------------------------

    (defun c:DistSample_MainForm_OkButton_OnClicked ( )

        (dcl_MessageBox "Drücken Sie die Schaltfläche OK, um abzubrechen..." "Abbrechen und schließen ...")

        (dcl_form_close DistSample_MainForm)

    ) ;;------------------------------------------------------------------------

    (defun _Main ( / odclProjName )

        ;;  Die Funktionen in dieser Datei laden und ausführen.

        (if

            (and

                (_Load_ODCL_Runtime)

                ;;  Versuche das OpenDCL-Projekt aus den hardcodierten Projektdaten
                ;;  des Quellcodes zu laden

                (_Load_ODCL_Stream nil nil)

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

(princ "\nOpenDCL DistSample2 (ver 1.3) wurde geladen. Geben Sie den Befehl \"DistSample2\" ein, um ihn auszuführen.\n")

(princ)