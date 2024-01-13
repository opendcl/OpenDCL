;;;
;;; Splitter Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung des Splitter-Steuerelements und seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:splitter (/ lstDistance lstStyle lstThickness lstLastContent strStyle strThick strInner strOuter
                     set_style set_thickness check_distance set_inside set_outside
                     c:splitter_splitter_OnInitialize
                     c:splitter_splitter_OnCancelClose
                     c:splitter_splitter_spl_ver_OnSplitterMoved
         c:Splitter_splitter_grd_OnBeginLabelEdit
         c:Splitter_splitter_grd_OnEndLabelEdit
         )

  ;; Variablen setzen
  
  (setq lstDistance (list "10" "30" "50" "70"))
  (setq lstThickness (list "0" "1" "2" "3" "4" "5" "6" "7" "8"))
  (setq lstStyle (list "Einfache Linie" "Doppellinie" "Haarlinie" "Flach" "Eingedrückt" "3D-Rahmen"))
  (setq strStyle "Stil")
  (setq strThick "Stärke (in Pixel)")
  (setq strInner "Mindestabstand links/oben")
  (setq strOuter "Mindestabstand rechts/unten")

  ;; Grundlegende Funktionen

  (defun set_style (n)
    (dcl-Control-SetEdgeStyle splitter_splitter_spl_hor n)
    (dcl-Control-SetEdgeStyle splitter_splitter_spl_ver n)
  ); set_style

  (defun set_thickness (n)
    (dcl-Control-SetHeight splitter_splitter_spl_hor n)
    (dcl-Control-SetWidth splitter_splitter_spl_ver n)
  ); set_thickness

  (defun set_outside (n)
    (dcl-Control-SetClosestOutside splitter_splitter_spl_hor n)
    (dcl-Control-SetClosestOutside splitter_splitter_spl_ver n)
  ); set_outside

  (defun set_inside (n)
    (dcl-Control-SetClosestInside splitter_splitter_spl_hor n)
    (dcl-Control-SetClosestInside splitter_splitter_spl_ver n)
  ); set_inside

  (defun check_distance (strValue)
    (if (not (member strValue lstDistance))
      (setq lstDistance (vl-sort (cons strValue lstDistance)
         '(lambda (a b) (< (atoi a) (atoi b)))))
    ); if
  ); check_distance

  (defun check_thickness (strValue)
    (if (not (member strValue lstThickness))
      (setq lstThickness (vl-sort (cons strValue lstThickness)
         '(lambda (a b) (< (atoi a) (atoi b)))))
    ); if
  ); check_thickness

  ;; Dieses Ereignis wird ausgelöst, sobald der Dialog angezeigt werden soll.
  ;; Hier werden Vorgabewerte für den Dialog und seine Steuerelemente gesetzt.

  (defun c:splitter_splitter_OnInitialize (/ intRow lstRow intInside intOutside intThickness)
    (setq intInside (dcl-Control-GetClosestInside splitter_splitter_spl_ver))
    (check_distance (itoa intInside))

    (setq intOutside (dcl-Control-GetClosestOutside splitter_splitter_spl_ver))
    (check_distance (itoa intOutside))

    (setq intThickness (dcl-Control-GetWidth splitter_splitter_spl_ver))
    (check_thickness (itoa intThickness))

    (setq intStyle (dcl-Control-GetEdgeStyle splitter_splitter_spl_ver))
    
    (foreach lstRow (list (list strInner 38 lstDistance (itoa intInside))
        (list strOuter 38 lstDistance (itoa intOutside))
        (list strThick 38 lstThickness (itoa intThickness))
        (list strStyle 18 lstStyle (nth intStyle lstStyle))
        )
      (if (setq intRow (dcl-Grid-AddString Splitter_splitter_grd (car lstRow)))
  (progn
    (dcl-Grid-SetCellStyle Splitter_splitter_grd intRow 1 (cadr lstRow))
    (dcl-Grid-SetCellDropList Splitter_splitter_grd intRow 1 (caddr lstRow))
    (dcl-Grid-SetCellText Splitter_splitter_grd intRow 1 (last lstRow))
  ); progn
      ); if
    ); foreach

    (dcl-Control-SetText Splitter_splitter_edt_info "")
  ); c:splitter_splitter_OnInitialize

  ;; Dieses Ereignis wird ausgelöst, wenn der Anwender im Datenblatt die Eingabetaste drückt.
  ;; Diese Funktion muss den Wert T zurückgeben, damit der Dialog nicht geschlossen wird.
  ;; Für den Fall, dass er aber geschlossen werden soll, muss diese Funktion den Wert nil zurückgeben
  
  (defun c:splitter_splitter_OnCancelClose (intIsESC /)
    (dcl-Grid-CancelCellEdit Splitter_splitter_grd)
    (/= intIsESC 1)
  ); c:splitter_splitter_OnCancelClose

  ;; Dieses Ereignis wird ausgelöst, wenn der vertikale Splitter bewegt wird, um die
  ;; Spaltenbreite des Datenblatts neu zu berechnen.
  
  (defun c:splitter_splitter_spl_ver_OnSplitterMoved (intUpperLeftX intUpperLeftY intWidth intHeight /)
    (setq intWidth (fix (* 0.5 (dcl-Control-GetWidth Splitter_splitter_grd))))
    (dcl-Grid-SetColumnWidth Splitter_splitter_grd 0 intWidth)
    (dcl-Grid-SetColumnWidth Splitter_splitter_grd 1 intWidth)
  ); c:splitter_splitter_spl_ver_OnSplitterMoved

  ;; Dieses Ereignis wird ausgelöst, wenn der Editiervorgang einer Zelle startet
  ;; Dient u.a. dazu den aktuellen Wert zu sichern, um ihn bei Bedarf zurückzuschreiben
    
  (defun c:Splitter_splitter_grd_OnBeginLabelEdit (intRow intCol /)
    (setq lstLastContent (list intRow intCol (dcl-Grid-GetCellText Splitter_splitter_grd intRow intCol)))
  ); c:Splitter_splitter_grd_OnBeginLabelEdit

  ;; Dieses Ereignis wird ausgelöst, wenn der Editiervorgang der Zelle beendet wird.
  
  (defun c:Splitter_splitter_grd_OnEndLabelEdit (intRow intCol / strValue strProp intPos)
    (cond
      ((not lstLastContent) nil)
      ((/= intRow (car lstLastContent)) nil)
      ((or (not (setq strValue (dcl-Grid-GetCellText Splitter_splitter_grd intRow 1)))
     (= (vl-string-trim " " strValue) ""))
       (dcl-Grid-SetCellText Splitter_splitter_grd intRow 1 (last lstLastContent)))
      ((or (not (setq strProp (dcl-Grid-GetCellText Splitter_splitter_grd intRow 0)))
     (= strProp ""))
       (dcl-Grid-SetCellText Splitter_splitter_grd intRow 1 (last lstLastContent)))
      ((= strProp strStyle)
       (if (setq intPos (vl-position strValue lstStyle))
   (set_style intPos)
   (dcl-Grid-SetCellText Splitter_splitter_grd intRow 1 (last lstLastContent))
       ))
      ((= strProp strThick)
       (check_thickness strValue)
       (dcl-Grid-SetCellDropList Splitter_splitter_grd intRow 1 lstThickness)
       (set_thickness (atoi strValue)))
      ((= strProp strInner)
       (check_distance strValue)
       (dcl-Grid-SetCellDropList Splitter_splitter_grd intRow 1 lstDistance)
       (set_inside (atoi strValue)))
      ((= strProp strOuter)
       (check_distance strValue)
       (dcl-Grid-SetCellDropList Splitter_splitter_grd intRow 1 lstDistance)
       (set_outside (atoi strValue)))
    ); cond
    (setq lstLastContent nil)
  ); c:Splitter_splitter_grd_OnEndLabelEdit

  ;; Dieses Ereignis wird ausgelöst, wenn sich die aktuelle Auswahl im Datenblatt ändert
  
  (defun c:Splitter_splitter_grd_OnSelChanged (intRow intCol / strProp)
    (setq strProp (dcl-Grid-GetCellText Splitter_splitter_grd intRow 0))
    (cond
      ((= strProp strStyle) (dcl-Control-SetText Splitter_splitter_edt_info "Diese Eigenschaft bestimmt die Darstellung des Splitters."))
      ((= strProp strThick) (dcl-Control-SetText Splitter_splitter_edt_info "Diese Eigenschaft bestimmt die Stärke des Splitters in Pixel."))
      ((= strProp strInner) (dcl-Control-SetText Splitter_splitter_edt_info "Diese Eigenschaft bestimmt den Wert, den ein Splitter dem linken oder oberen Rand am nächsten kommen darf."))
      ((= strProp strOuter) (dcl-Control-SetText Splitter_splitter_edt_info "Diese Eigenschaft bestimmt den Wert, den ein Splitter dem rechten oder unteren Rand am nächsten kommen darf."))
      (T (dcl-Control-SetText Splitter_splitter_edt_info ""))
    ); cond
    nil
  ); c:Splitter_splitter_grd_OnSelChanged

  ;|<<Dialog anzeigen>>|;

  ;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Projekt laden
  (dcl-Project-Load (*ODCL:Samples-FindFile "Splitter.odcl"))

  ;; Dialog anzeigen
  (dcl-Form-Show splitter_splitter)

  ;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
  ;; Zeile stehen bleibt und (dcl-Form-Show) solange keinen Wert zurückgibt,
  ;; bis der modale Dialog geschlosswen wird.
  ;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

  (princ)
); splitter

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
  "Splitter"
)
(princ)

;;;######################################################################
;;;######################################################################
