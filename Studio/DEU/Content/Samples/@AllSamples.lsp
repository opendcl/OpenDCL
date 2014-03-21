;;;######################################################################
;;;
;;;OpenDCL Beispiel: AllSamples
;;;
;;; Dieses Beispiel zeigt einen Dialog zum Start der einzelnen OpenDCL Beispiel.
;;;


;|«Global Constants»|;

;; Liste der Beispiele in der Form (Registerkarte Beispielname Lisp-Dateiname Beschreibung)
(setq *OdclAllSamples '(
  ("Basic" "Verschiedenes" "Misc.lsp" "Blockvorschau, Dias, Listen und verschiedene Steuerelemente")
  ("Basic" "Methoden" "Methods.lsp" "Bilder, Regler, Steuerelemente verändern")
  ("Basic" "Ereignisse" "EventHandling.lsp" "Ereignisse behandeln")
  ("Basic" "Message box" "MessageBox.lsp" "Meldungen und Abfragen verwenden")
  ("Basic" "DWG-Vorschau" "ViewDWG.lsp" "Mit der DWG-Vorschau arbeiten")
  ("Basic" "Nicht-modaler Dialog" "Modeless.lsp" "Mit nicht-modalen Dialogen arbeiten")
  ("Basic" "Palette" "AllControls.lsp" "Palette mit Beispielen aller Steuerelemente")
  ("Basic" "HTML" "HTML.lsp" "Webseiten-Steuerelement")
  ("Basic" "Video" "Animation.lsp" "Eine AVI-Datei mit dem Video-Steuerelement abspielen")
  ("Basic" "Schraffur" "Hatches.lsp" "Zeigt Schraffurmuster")
  ("Basic" "Tooltip" "ToolTip.lsp" "Mit Tooltips an Steuerlementen arbeiten")
  ("Basic" "Drücken & Ziehen" "DragNDrop.lsp" "Drag&drop zwischen verschiedenen Steuerelementen")
  ("Basic" "Erweiterte Liste" "ListView.lsp" "Die erweiterte Liste nutzen")
  ("Basic" "Kopieren & Einfügen" "ListBoxCopyPaste.lsp" "Kopieren und verschieben zwischen Steuerelementen")
  ("Basic" "Splitter" "Splitter.lsp" "Mit Splitter-Steuerlementen arbeiten")
  ("Basic" "DWG-Liste" "DwgList.lsp" "Mit der DWG-Liste arbeiten")
  ("Basic" "Listenfeld" "ListBox.lsp" "Mit dem Listenfeld und der Dateiauswahl arbeiten")
  ("Advanced" "Baumstruktur" "Tree.lsp" "Mit der Baumstruktur arbeiten")
  ("Advanced" "Datenblatt" "Grid.lsp" "Mit dem Datenblatt arbeiten")
  ("Advanced" "Auswahl" "Selections.lsp" "Punkt oder Objekte aus modalen Dialogen heraus wählen")
  ("Advanced" "Schieberegler" "Slider.lsp" "Benutzerdefinierter Schieberegler basierend auf einem Bildsteuerelement")
  ("Advanced" "Registerkarte in den Optionen" "OptionsTab.lsp" "Eine benutzerdefinierte Registerkarte in den Optionen anzeigen und verwenden")
  ("Advanced" "Splash" "Splash.lsp" "Einen Splash-Dialog mit OnTimer anzeigen")
  ("Advanced" "Mover" "FormMover.lsp" "Bewegen und skalieren einer Dialogbox, nachdem sie angezeigt wurde")
  ("Advanced" "Auswahlliste" "ComboBoxLab.lsp" "Vergleich zwischen versch. Arten von Auswahllisten und ihrem Verhalten")
  ("Advanced" "VLX 1" "DistSample1.vlx" "ODCL in einer Quelle eingebettet")
  ("Advanced" "VLX 2" "DistSample2.vlx" "ODCL als Datenstrom")
))

;; Hauptprogramm
(DEFUN C:OPENDCLDEMO (/ *error*)
  (defun *error* (msg)
    (while (< 0 (getvar "cmdactive")) (command))
    ;; do error stuff
    (if (dcl-Form-IsActive AllSamples/Main)
      (dcl-Form-Close AllSamples/Main)
    )
    (princ
      (strcat "\nAnwendungsfehler: " (itoa (getvar "errno")) " :- " msg)
    )
    (princ)
  )
  ;;------------------------

  ;; Stellt sicher, dass die OpenDCL Laufzeitumgebung geladen ist (ohne Textausgabe an der Befehlszeile)
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Lädt das Projekt
  (dcl-Project-Load (*ODCL:Samples-FindFile "@AllSamples.odcl") NIL "AllSamples")

  ;; Zeig den Dialog an.
  (dcl-Form-Show AllSamples/Main)
  ;; Die Ereignisfunktionen steuern an dieser Stelle den Dialog.
  (princ)
)

;; Lade das spezielle Beispiel
(DEFUN *ODCL:RunSample (filename)
  (setq *ODCL:AllSamples T) ; definiert, dass das Beispiel aus dem Masterdemo geladen wurde
  (if (eq "Fail" (load (*ODCL:Samples-FindFile filename) "Fail"))
    ((alert (strcat "Die Datei \"" filename "\" konnte nicht geladen werden!")))
  )
  (setq *ODCL:AllSamples nil)
)

;; Entlade OpenDCL komplett, um die die Installation der Aktualisierungen zu ermöglichen
(DEFUN *ODCL:Vanish ()
  (foreach project (dcl-GetProjects) (dcl-Project-Unload project T))
  (foreach module (arx)
    (if (wcmatch module "opendcl`.*") (arxunload module))
  )
  (princ)
)

;; Überprüfe auf eine jüngere Version; wurde eine gefunden,
;; frage, ob sie heruntergeladen und installiert werden soll
(DEFUN *ODCL:UpdateCheck (/ create_http request_http download_msi write_msi
                            parse_version <version get_curver get_curlang
                            append_status)
  (vl-load-com)
  (defun create_http () ; Erzeuge das WinHttpRequest Objekt und schließe ab
    (cond
      ((vlax-create-object "WinHttp.WinHttpRequest.5.1"))
      ((vlax-create-object "WinHttp.WinHttpRequest.5"))
    )
  )
  (defun request_http (url return / http disp pos location filename result) ; HTTP Anfrage ausführen
    (setq http (create_http))
    (setq result
      (vl-catch-all-apply
        (function
          (lambda ()
            (vlax-invoke-method http "Open" "GET" url :vlax-false)
            (vlax-invoke-method http "SetRequestHeader" "User-Agent" "OpenDCL AllSamples")
            (vlax-invoke-method http "Send")
            (vlax-get-property http return)
          )
        )
      )
    )
    (vlax-release-object http)
    (if (vl-catch-all-error-p result)
      (progn (append_status (strcat "  ERROR: " (vl-catch-all-error-message result))) (setq result nil))
    )
    result
  )
  (defun download_msi (lang dev / result) ; MSI via HTTP herunterladen, Rückgabe Antwort
    (append_status "OpenDCL Studio Installationsdatei herunterladen...")
    ;; Stelle die DownloadURL zusammen und lade die Datei herunter
    ;; Hinweis: um die Laufzeit MSI herunterzuladen, verwenden Sie
    ;; die URL http://opendcl.com/go?runtime und ignorieren die Sprache
    (setq url "http://opendcl.com/go?studio")
    (if lang (setq url (strcat url "&" (strcase lang T))))
    (if dev (setq url (strcat url "&dev")))
    (setq result (request_http url "ResponseBody"))
    (if result (append_status "  Herunterladen erfolgreich!"))

    (if (and (setq result (request_http url "ResponseBody"))
             (= (type result) 'VARIANT)
             (not (zerop (vlax-variant-type result))))
      (append_status "  Erfolgreich heruntergeladen!")
      (progn
        (append_status "  FEHLER: Die Antwort des Servers enthielt keine Daten!")
        (setq result nil)
      );
    );

    result
  )
  (defun write_msi (lang dev filename / result fso) ; heruntergeladene MSI ins Temp-Verzeichnis speichern, Rückgabe des Dateipfads
    (if (setq result (download_msi lang dev))
      (progn
        (setq fso (vlax-create-object "Scripting.FileSystemObject"))
        (setq result
          (vl-catch-all-apply
            (function
              (lambda (/ tempfolder filepath adostream filestream)
                (append_status "Installationsdatei auf den Datenträger speichern...")
                (setq tempfolder (vlax-invoke-method fso "GetSpecialFolder" 2))
                (setq filepath (strcat (vlax-get-property tempfolder "Path") "\\" filename))
                (vlax-release-object tempfolder)
                (cond
                  ( (setq adostream (vlax-create-object "ADODB.Stream"))
                    (setq result
                      (vl-catch-all-apply
                        (function
                          (lambda ()
                            (if (= :vlax-true (vlax-invoke-method fso "FileExists" filepath))
                              (vlax-invoke-method fso "DeleteFile" filepath :vlax-true)
                            )
                            (vlax-put adostream "Type" 1) ;1 = Binär
                            (vlax-invoke adostream "Open")
                            (vlax-invoke-method adostream "Write" result)
                            (vlax-put adostream "Position" 0)
                            (vlax-invoke adostream "SaveToFile" filepath)
                            (vlax-invoke-method adostream "Close")
                            filepath
                          )
                        )
                      )
                    )
                    (vlax-release-object adostream)
                  )
                  ( (setq filestream
                      (vlax-invoke-method fso "CreateTextFile" filepath :vlax-true :vlax-false)
                    )
                    (setq result
                      (vl-catch-all-apply
                        (function
                          (lambda ()
                            (foreach element (vlax-safearray->list (vlax-variant-value result))
                              (vlax-invoke filestream "Write" (chr (+ 256 (logand 255 element))))
                            )
                            (vlax-invoke-method filestream "Close")
                            filepath
                          )
                        )
                      )
                    )
                    (vlax-release-object filestream)
                  )
                )
                (if (vl-catch-all-error-p result)
                  (progn (append_status (strcat "  FEHLER: " (vl-catch-all-error-message result))) (setq filepath nil))
                  (append_status "  Datei erfolgreich gespeichert!")
                )
                filepath
              )
            )
          )
        )
        (if fso (vlax-release-object fso))
        (if (vl-catch-all-error-p result)
          (progn (append_status (strcat "  FEHLER: " (vl-catch-all-error-message result))) (setq result nil))
        )
        result
      )
    )
  )
  (defun parse_version (version / token) ; Wandelt den Versionsnamen in eine Liste
    (if (setq token (vl-string-position 46 version))
      (cons (atoi (substr version 1 token)) (parse_version (substr version (+ 2 token))))
      (list (atoi version))
    )
  )
  (defun <version (left right) ; Versionslisten vergleichen
    (cond
      ( (null left)
        (not (null right))
      )
      ( (< (car left) (car right)))
      ( (= (car left) (car right))
        (<version (cdr left) (cdr right))
      )
    )
  )
  (defun get_curver (dev / url) ; Aktuelle Version von opendcl.com abfragen
    (setq url
      (if dev
        "http://opendcl.com/version/version_dev.txt"
        "http://opendcl.com/version/version.txt"
      )
    )
    (request_http url "ResponseText")
  )
  (defun get_curlang () ; aktuell installierte Sprache abfragen
    (cond
      ( (vl-registry-read
          "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
          "Language"
        )
      ) ;_ 32-bit Speicherort
      ( (vl-registry-read
          "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
          "Language"
          )
      ) ;_ 32-bit Speicherort
      ( (vl-registry-read
          "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
          "Language"
        )
      ) ;_ 64-bit Speicherort
      ( "DEU"
      )
    )
  )
  (defun append_status (status) ; Status zur Listbox hinzufügen
    (dcl-ListBox-AddString AllSamples/Update/lbxStatus status)
    (dcl-Control-Redraw AllSamples/Update)
  )

  (dcl-Form-Show AllSamples/Update)
  (princ)
)


;|«OpenDCL Event Handlers»|;

(DEFUN c:AllSamples/Update#OnInitialize (/ curlang myver myver_string curver curver_string)
  (dcl-ListBox-Clear AllSamples/Update/lbxStatus)
  (dcl-Control-SetVisible AllSamples/Update/btnUpdate nil)

  (append_status "Ich suche Aktualisierungen. Bitte warten Sie...")

  ;; Installierte OpenDCL-Version anzeigen
  (setq curlang (get_curlang))
  (setq myver (parse_version (setq myver_string (dcl-GETVERSIONEX))))
  (dcl-Control-SetCaption AllSamples/Update/lblLanguage curlang)
  (dcl-Control-SetCaption AllSamples/Update/lblVersion myver_string)

  ;; Letzte verfügbare OpenDCL-Version anzeigen
  (setq curver (parse_version (setq curver_string (get_curver nil))))
  (if (<version curver myver)
    (progn
      (setq curver_string (get_curver T)) ; Build-Version abfragen
      (setq curver (parse_version curver_string))
      (dcl-Control-SetVisible AllSamples/Update/lblDevBuild T)
      (dcl-Control-SetVisible AllSamples/Update/lblDevBuildAvail T)
    )
    (progn
      (dcl-Control-SetVisible AllSamples/Update/lblDevBuild nil)
      (dcl-Control-SetVisible AllSamples/Update/lblDevBuildAvail nil)
    )
  )
  (dcl-Control-SetCaption AllSamples/Update/lblLanguageAvail curlang)
  (dcl-Control-SetCaption AllSamples/Update/lblVersionAvail curver_string)

  ;; Überprüfung, ob die installierte Version aktualisiert werden kann
  (dcl-Control-SetVisible AllSamples/Update/btnClose T)
  (if (<version myver curver)
    (progn
      (append_status "Eine neuere Version von OpenDCL Studio ist verfügbar!")
      (dcl-Control-SetEnabled AllSamples/Update/btnUpdate T)
      (dcl-Control-SetVisible AllSamples/Update/btnUpdate T)
    )
    (append_status "OpenDCL Studio ist aktuell!")
  )
)

(DEFUN c:AllSamples/Update/btnClose#OnClicked (/)
  (dcl-Form-Close AllSamples/Update 2)
)

(DEFUN c:AllSamples/Update/btnUpdate#OnClicked (/ msipath curlang)
  (dcl-Control-SetEnabled AllSamples/Update/btnUpdate nil)
  (dcl-Control-SetVisible AllSamples/Update/btnClose nil)
  (setq msipath
    (write_msi
      (setq curlang (dcl-Control-GetCaption AllSamples/Update/lblLanguageAvail))
      (dcl-Control-GetVisible AllSamples/Update/lblDevBuildAvail)
      (strcat
        "OpenDCL.Studio."
        (strcase curlang)
        "."
        (dcl-Control-GetCaption AllSamples/Update/lblVersionAvail)
        ".msi"
      )
    )
  )
  (if msipath
    (progn
      (dcl-SendString "(*ODCL:Vanish)\n")
      (startapp (strcat "msiexec.exe /i " msipath))
      (dcl-Form-Close AllSamples/Update 1)
    )
    (dcl-Control-SetVisible AllSamples/Update/btnClose T)
  )
)
(defun c:AllSamples/Main#OnInitialize (/)
  (dcl-Grid-Clear AllSamples/Main/Basic)
  (foreach sample *OdclAllSamples
    (dcl-Grid-AddRow "AllSamples" "Main" (nth 0 sample) (nth 1 sample) (nth 2 sample) (nth 3 sample))
  )
  (c:AllSamples/Main#OnDocActivated)
  (princ)
)

(defun c:AllSamples/Main#OnSize (NewWidth NewHeight /)
  (dcl-Grid-SetColumnWidth AllSamples/Main/Basic 2 (- (dcl-Control-GetWidth AllSamples/Main/Basic) (dcl-Grid-GetColumnWidth AllSamples/Main/Basic 0) 18))
  (dcl-Grid-SetColumnWidth AllSamples/Main/Advanced 2 (- (dcl-Control-GetWidth AllSamples/Main/Advanced) (dcl-Grid-GetColumnWidth AllSamples/Main/Advanced 0) 18))
  (princ)
)

(defun c:AllSamples/Main#OnEnteringNoDocState (/)
  (dcl-Control-SetEnabled AllSamples/Main/Basic NIL)
  (dcl-Control-SetEnabled AllSamples/Main/Advanced NIL)
  (dcl-Control-SetEnabled AllSamples/Main/btnUpdateCheck NIL)
  (princ)
)

(defun c:AllSamples/Main#OnDocActivated (/)
  (dcl-Control-SetEnabled AllSamples/Main/Basic T)
  (dcl-Control-SetEnabled AllSamples/Main/Advanced T)
  (dcl-Control-SetEnabled AllSamples/Main/btnUpdateCheck T)
  (princ)
)

(defun c:AllSamples/Main/Basic#OnDblClicked (Row Column /)
  (if (<= Column 0)
    (dcl-SendString (strcat "(*ODCL:RunSample \"" (dcl-Grid-GetCellText AllSamples/Main/Basic Row 1) "\")(princ)\n"))
  )
  (princ)
)

(defun c:AllSamples/Main/Advanced#OnDblClicked (Row Column /)
  (if (<= Column 0)
    (dcl-SendString (strcat "(*ODCL:RunSample \"" (dcl-Grid-GetCellText AllSamples/Main/Advanced Row 1) "\")(princ)\n"))
  )
  (princ)
)

(defun c:AllSamples/Main/Basic#OnSelChanged (Row Column /)
  (if (>= Column 0)
    (dcl-Grid-SetCurCell AllSamples/Main/Basic Row -1)
  )
  (princ)
)

(defun c:AllSamples/Main/Advanced#OnSelChanged (Row Column /)
  (if (>= Column 0)
    (dcl-Grid-SetCurCell AllSamples/Main/Advanced Row -1)
  )
  (princ)
)

(defun c:AllSamples/Main/btnReadme#OnClicked (/ readme)
  (setq readme (*ODCL:Samples-FindFile "@ReadME.txt"))
  (if readme
    (startapp "notepad" readme)
    (alert (strcat "Ich kann die Datei \"@ReadME.txt\" nicht finden!"))
  )
  (princ)
)

(defun c:AllSamples/Main/btnUpdateCheck#OnClicked ()
  (dcl-SendString "(*ODCL:UpdateCheck)\n")
  (princ)
)

(princ)

;;;######################################################################
;;;######################################################################
;;; Der folgende Abschnitt dient dazu, die Beispiel-Dateien zu lokalisieren.
;;; Die Pfadangabe wird um den Abschnitt des Beispielordner, erweitert, der
;;; durch das Installationsprogramm in der Registry eingetragen wurde.
;;; Die globalen Variable *ODCL:Prefix und die Function *ODCL:Samples:FindFile
;;; werden in allen Beispieldateien verwendet.
;;;
(or *ODCL:Samples-FindFile
  (defun *ODCL:Samples-FindFile (file)
    (setq *ODCL:Prefix
      (cond
        (	*ODCL:Prefix
        ) ;_ bereits definiert
        (	(vl-registry-read
            "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
            "SamplesFolder"
          )
        ) ;_ 32-bit Variante
        (	(vl-registry-read
            "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
            "SamplesFolder"
           )
        ) ;_ 32-bit Variante
        (	(vl-registry-read
            "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
            "SamplesFolder"
          )
        ) ;_ 64-bit Variante
      )
    )
    (cond
      ((findfile file)) ; überprüfe zunächst den Supportpfad
      (*ODCL:Prefix (findfile (strcat *ODCL:Prefix file)))
      (file)
    )
  )
)

(princ "\nOPENDCLDEMO\n")
(C:OPENDCLDEMO)

;;;######################################################################
;;;######################################################################

 ;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 2 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
