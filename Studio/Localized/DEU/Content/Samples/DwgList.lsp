;;;
;;; DWG-Liste Beispiel
;;;
;;; Dieses Beispiel demonstriert die Anwendung des Steuerlements DWG-Liste mit seinen Ereignissen.
;;;

;; Hauptprogramm
(defun c:DwgList (/ cmdecho)

  ;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
  (setq cmdecho (getvar "CMDECHO"))
  (setvar "CMDECHO" 0)
  (command "_OPENDCL")
  (setvar "CMDECHO" cmdecho)

  ;; Projekt laden
  (dcl-Project-Load (*ODCL:Samples-FindFile "DwgList.odcl"))

  ;; Dialog anzeigen
  (dcl-Form-Show DwgList/DwgListForm)

  ;; Dies ist ein nicht-modaler Dialog. Das bedeutet, dass (dcl-Form-Show)
  ;; sofort die Kontrolle zurückgibt und das Programm weiterläuft, während
  ;; der Dialog aktiv ist.
  ;; Die Ereignisfunktionen übernehmen erst nachher die Kontrolle und müssen
  ;; daher global definiert sein.

  (princ)
)

;|*OpenDCL Event Handlers*|;

(defun c:DwgList/DwgListForm#OnInitialize (/)
  (dcl-ComboBox-AddPath DwgList/DwgListForm/ComboBox1
    (dcl-DWGList-GetDir DwgList/DwgListForm/DwgList1)
  )
  (princ)
)

(defun c:DwgList/DwgListForm/DwgList1#OnDblClicked (/ filename)
  (if (and (Setq filename (dcl-DWGList-GetFileName DwgList/DwgListForm/DwgList1))
                 (equal (strcase (substr filename (- (strlen filename) 3) 4)) ".DWG"))
    (dcl-sendstring (strcat "_-INSERT " filename "\n"))
  )
  (princ)
)

(defun c:DwgList/DwgListForm/cmdUP#OnClicked (/ path)
  (setq path (dcl-DWGList-GetDir DwgList/DwgListForm/DwgList1))
  (cond
    ( (= path ""))
    ( (wcmatch path "::*"))
    ( (dcl-DWGList-Dir DwgList/DwgListForm/DwgList1 (strcat path "\\.."))
      (dcl-ComboBox-AddPath DwgList/DwgListForm/ComboBox1
        (dcl-DWGList-GetDir DwgList/DwgListForm/DwgList1)
      )
    )
  )
  (princ)
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
  "DwgList"
)
(princ)

;;;######################################################################
;;;######################################################################
