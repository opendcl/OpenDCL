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
        _Load_ODCL_Runtime
        _Load_ODCL_Stream
        _Main

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Runtime ( / )

        (or

            ;;  OpenDCL ist bereits geladen, mit T beenden

            dcl-getversionex

            ;;  Ist das Laden nach Bedarf aktiviert (DEMANDLOAD), nutze das OPENDCL-Kommando
            ;;  um die OpenDCL-Laufzeitumgebung zu laden. Ist das Laden nach Bedarf deaktiviert,
            ;;  nehmen wir an, dass dies gewünscht ist und laden deshalb nicht dazu.

            (and

                ;;  Ist das Laden nach Bedarf aktiviert

                (= 2 (boole 1 (getvar "DEMANDLOAD") 2))

                ;;  dann lade die OpenDCL Laufzeitumgebung

                (vl-catch-all-apply 'vl-cmdf '("OPENDCL"))

                ;;  Und prüfe, ob die Funktionen nun geladen sind

                dcl-getversionex
            )

            ;;  Konnte oder sollte die Laufzeitumgebung nicht geladen werden
            ;;  wird der Anwender informiert.

            (princ "\nFehler: OpenDCL-Laufzeitumgebung konnte nicht geladen werden.\n")
        )

        dcl-getversion

    ) ;;------------------------------------------------------------------------

    (defun _Load_ODCL_Stream ( password alias / project rtype )

        ;;  Das ist eine formatierte Kopie des Inhalts der Datei DistSample.odcl.lsp.
        ;;  Die einzige Art der Formatierung bestand darin, dass alle Zeilen auf eine
        ;;  einheitliche Länge gebracht wurden

        (setq project
            '("YWt6AzUbAABgsXpTBuKTJkUxai9qgDggaVrvHjZSSid8djhbtmV1He5mLmhubRwSdpJQjCa+XobW"
              "Ws/OzsnMyornhjZS08qNeaam51YUG93MAJmwsLCwJzpqa3cNWQmYAIlCkEOZ4BEYzUUstVfaBTdp"
              "WA5/jUbzbzKdQuw2vn+O1HZWmC3q1EVuPUWcFj515K/2/ZIP8HAPqjK9aPIPSjwHZbMb0OMNd0ov"
              "0f8WsjQckNxXj6b2KJTyLGDdGHkTnKgB+csQnTYCyS2gEEP0IF05s6pk2ztVKRXcMGDI9S2nzxPF"
              "HmISjW47WDSzd7KeRhhPuW6zSWrYu/Dm5CV0ISqGR3RCUkmNJLdRV4I7n7V0wT7HycGKOLNejO+r"
              "sitB3smy6VeJjxPvQYvPqwHSjm37YVcilnrJGmqAlJE5crMLagdGUMZU7pzEAKgz/FeQvE+KjM54"
              "5pYnMgwQ9Otrz5fxbrdNPyd6PapF7edFuZlWhUJRx+Xpf4FFKdSvH5piEuNhmyfwEh+aBydA7i1m"
              "g6IjWWiS1M2KHTR9eStpvR/SE5sfFiEr4ucfSOyq9S/oHqAy/7SMAytmCWvA2Y7j4WoYTA0UUOAs"
              "BkTtYZnNKqvS7DFUAj3Tyeq34fXpAwMlW2Hqg4Uyq8bGJKcBiokdm1VuHWXySbuNtGKRQL/7v52i"
              "tX02pkzypcUH3jaLLmsL97xNfwHbL2AE+gPioOzCXFL4WjVL5fJLHAkth9eOmyl6cYaG854tNIBQ"
              "zlAy81FhbAfh8YV64SoakCtG1I53+596iWVapZChUwct1YzuO7kP2pdWVXdbL9GXJqK1azgT0MPL"
              "al+BV/8SetVulq1YWi6oKbsQQUFtwFWxXKZyDHYopbM9D6GgDDIdiPYi1DsmTozgZZnW+YlHsHQL"
              "mhPAHhNsyPLY08iyqIFDBECN3SYsobB0j82h8SStKyzrM6k/WTapbNIwqTR6lwRzAf7QPwSQJ1VD"
              "Kd6tVHqTCKwNDsGOB5aXCybqh8K0Bk/v1zQF6ahnEzOlt83wg1YF54tlD3sbthf1cr6VrMyPJATQ"
              "ubOraL2GvYbJDoLXC8jzoIEEJTQsngtJ3iGUZcOBi8hOgE6AToBOgE6AToBOgE6AToDTawEZijYE"
              "WrGewXiQYW/1Qc6CYQCCAXkdxe4a"
      )
        )

        (cond

            ;;  An diesem Punkt muss die OpenDCL-Laufzeitumgebung bereits geladen
            ;;	sein. Wenn nicht, ist zuvor die Initialisierung fehlgeschlagen oder
            ;;	es ist eine ältere OpenDCL-Laufzeitumgebung geladen worden.
            ;;  So oder so, an dieser Stell gehts nicht weiter...

            (	(null dcl-project-import)

                (princ "\nFür diese Funktion wird die Laufzeitumgebung von OpenDCL 5 oder höher vorausgesetzt.\n")

                nil
            )

            ;;  Mit dcl-project-load wird das Projekt geladen und der Rückgabewert
            ;;	an die aufrufende Funktion übergeben, wenn der Vorgang erfolgreich war. 

            (	(dcl-project-import project password alias)   )
        )

    ) ;;------------------------------------------------------------------------

    (defun c:DistSample/MainForm/OkButton#OnClicked ( )

        (dcl-MessageBox "Drücken Sie die Schaltfläche OK, um abzubrechen..." "Abbrechen und schließen ...")

        (dcl-form-close DistSample/MainForm)

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
                    (dcl-Form-Show DistSample/MainForm)
                )

                (princ "\nKann den Dialog nicht anzeigen: DistSample/MainForm\n")
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