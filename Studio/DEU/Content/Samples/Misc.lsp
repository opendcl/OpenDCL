;;;
;;; Verschiedene Beispiele
;;;
;;; Dieses Beispiel demonstriert verschiedene Steuerlemente.
;;;

;; Hauptprogramm
(defun c:Misc (/ cmdecho)

	;; Sicherstellen, dass die OpenDCL-Laufzeitumgebung geladen wurde (ohne Meldungen an der Befehlszeile)
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Projekt laden
	(dcl_Project_Load (*ODCL:Samples:FindFile "Misc.odcl"))

	;; Dialog anzeigen
	(dcl_Form_Show Misc_DemoModal)

	;; Dies ist eine modale Dialogbox. Das bedeutet, dass das Programm an dieser
	;; Zeile stehen bleibt und (dcl_Form_Show) solange keinen Wert zurückgibt,
	;; bis der modale Dialog geschlosswen wird.
	;; In der Zwischenzeit übernehmen die Ereignisfunktionen die Dialogsteuerung.

	(princ)
)

;|<<OpenDCL Ereignisfunktionen>>|;

;; Diese Funktion wird ausgelöst, wenn zwischen den Karteikarten gewechselt wird
(DEFUN c:DemoModal_TabControl1_Changed (nSelIndex)
	(COND
		;; cond Karteikarte 1
		(	(= nSelIndex 1) (AddBlocksToListBox))
		;; cond Karteikarte 2
		(	(= nSelIndex 2)
			(IF (zerop (dcl_TREE_COUNTITEMS Misc_DemoModal_TreeControl1))
				(PROGN
					(dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1 "Dia 1" "Slide1.sld")
					(dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1 "Diabibliothek" "Slide Library.slb")
					(dcl_TREE_ADDPARENT Misc_DemoModal_TreeControl1 "Dia 3" "Slide3.sld")
					(dcl_TREE_ADDCHILD Misc_DemoModal_TreeControl1
									'(	("Slide Library.slb" "Element 1" "Child1")
										("Slide Library.slb" "Element 2" "Child2")
										("Slide Library.slb" "Element 3" "Child3")
									 )
					)
					(dcl_TREE_SELECTITEM Misc_DemoModal_TreeControl1 "Slide1.sld")
				)
			)
		)
		;; cond Karteikarte 3
		((= nSelIndex 3) (dcl_COMBOBOX_ADDCOLOR Misc_DemoModal_ComboBox4 156))
	)
)


(defun c:Misc_DemoModal_cmdRun_OnClicked (/ cnt wait)
	(setq cnt 0)
	(while (< cnt 100)
		(setq wait 10000)
		(while (> (setq wait (1- wait)) 0))
		(dcl_Control_SetValue Misc_DemoModal_ProgressBar1 (setq cnt (1+ cnt)))
	)
	(princ)
)



;; Diese Funktion wird ausgeführt, wenn sich die Auswahl in der Liste der Blocknamen ändert
(DEFUN c:DemoModal_BlockNameList_SelectionChanged (nSelCount sSelText /)
	(dcl_CONTROL_SETBLOCKNAME Misc_DemoModal_BlockView sSelText)
)

;; Diese Funktion wird ausgeführt, wenn sich die Auswahl in der Baumstruktur ändert
(DEFUN c:DemoModal_TreeControl1_SelectionChanged (sSelText sSelKey / strParent strFile Path)

    ;; Elternobjekt abfragen
    (if (setq strParent (dcl_TREE_GETPARENT Misc_DemoModal_TreeControl1 sSelKey))
        (if (setq strFile (findfile (strcat *ODCL:Prefix strParent)))
            (dcl_SlideView_Load Misc_DemoModal_SlideView1 strFile sSelKey)
            (dcl_SlideView_Clear Misc_DemoModal_SlideView1)
        ); if

        (if (setq strFile (findfile (strcat *ODCL:Prefix sSelKey)))
            (dcl_SlideView_Load Misc_DemoModal_SlideView1 strFile "")
            (dcl_SlideView_Clear Misc_DemoModal_SlideView1)
        ); if
    ); if
    (dcl_Control_Redraw Misc_DemoModal_SlideView1)
); c:DemoModal_TreeControl1_SelectionChanged


(DEFUN c:DemoModal_BrowseFolders_Clicked (/ path)
	(IF (SETQ path (dcl_SELECTFOLDER "Verzeichnis wählen" nil nil 81))
		(ALERT path)
	)
)


;; Diese Funktion listet alle Blöcke der aktuellen Zeichnungsdatei auf
(DEFUN AddBlocksToListBox (/ BlockInfo BlkLst Blk)
	(dcl_LISTBOX_CLEAR Misc_DemoModal_BlockNameList)
	;; Hole den ersten Block in der Blocktabelle
	;; Durchlaufe die Liste der Blockdefinitionen
	;; Holen den Blocknamen
	;; Ignoriere anonyme Blöcke, Bemaßungen, externe Referenzen usw...
	;; Füge den Blocknamen zur Liste 'BlkLst hinzu
	;; Gehe zum nächsten Block
	(SETQ BlockInfo (TBLNEXT "BLOCK" T))
	(WHILE BlockInfo
		(SETQ blk (CDR (ASSOC 2 BlockInfo)))
		(IF (NOT (WCMATCH blk "`*U*,`*D*,`*X*,`*T*,_*,*|*,A$*"))
			(SETQ BlkLst (cons blk BlkLst))
		)
		(SETQ BlockInfo (TBLNEXT "BLOCK"))
	)
	(setq BlkLst (reverse BlkLst))
	(IF (NULL BlkLst)
		(dcl_CONTROL_SETVISIBLE Misc_DemoModal_lblNoBlocks T)
		(dcl_CONTROL_SETVISIBLE Misc_DemoModal_lblNoBlocks nil)
	)
	(dcl_LISTBOX_ADDLIST Misc_DemoModal_BlockNameList BlkLst)
	;;
	;; alternative Methode mit wiederkehrendem AddString
	;; statt eine Liste zu füllen, wird der Eintrag ergänzt.
	;; In diesem Falle sollte bei dem Listenfeld vielleicht
	;; die Eigenschaft Sorted aktiviert sein
	;; (SETQ BlockInfo (TBLNEXT "BLOCK" T))
	;; (WHILE BlockInfo
	;; 	(SETQ blk (CDR (ASSOC 2 BlockInfo)))
	;; 	(IF (NOT (WCMATCH blk "`*U*,`*D*,`*X*,`*T*,_*,*|*,A$*"))
	;; 		(dcl_ListBox_AddString Misc_DemoModal_BlockNameList blk)
	;; 	)
	;; 	(SETQ BlockInfo (TBLNEXT "BLOCK"))
	;; )
	(PRINC)
)

(princ)

;|<<OpenDCL Beispiel Abschluss>>|;

;;;######################################################################
;;;######################################################################
;;; Der folgende Abschnitt dient dazu, die Beispiel-Dateien zu lokalisieren.
;;; Die Pfadangabe wird um den Abschnitt des Beispielordner, erweitert, der
;;; durch das Installationsprogramm in der Registry eingetragen wurde.
;;; Die globalen Variable *ODCL:Prefix und die Function *ODCL:Samples:FindFile
;;; werden in allen Beispieldateien verwendet.
;;;
(or *ODCL:Samples:FindFile
	(defun *ODCL:Samples:FindFile (file)
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
						"HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\OpenDCL"
						"SamplesFolder"
					)
				) ;_ 64-bit Variante aktueller Nutzer
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
		(if *ODCL:MasterDemo
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
	"Misc"
)
(princ)

;;;######################################################################
;;;######################################################################

;|«Visual LISP© Format Options»
(80 4 50 2 nil "end of " 80 50 0 0 2 nil nil nil T)
;*** DO NOT add text below the comment! ***|;
