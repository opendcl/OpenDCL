;;;
;;; Miscellaneous Sample
;;;
;;; This sample demonstrates miscellaneous controls.
;;;

;; Main program
(defun c:Misc (/ cmdecho)

	;; Ensure OpenDCL Runtime is (quietly) loaded
	(setq cmdecho (getvar "CMDECHO"))
	(setvar "CMDECHO" 0)
	(command "_OPENDCL")
	(setvar "CMDECHO" cmdecho)

	;; Load the project
	(dcl-Project-Load (*ODCL:Samples-FindFile "Misc.odcl"))

	;; Show the main form
	(dcl-Form-Show Misc/DemoModal)

	;; This is a modal form, so (dcl-Form-Show) does not return until
	;; the modal form is closed. In the meantime, the event handlers
	;; manage the form.

	(princ)
)

;|*OpenDCL Event Handlers*|;

;; this function gets fired when a tab gets changed
(DEFUN c:DemoModal_TabControl1_Changed (nSelIndex / nCount)
	(COND
		;; cond tab 1
		(	(= nSelIndex 1) (AddBlocksToListBox))
		;; cond tab 2
		(	(= nSelIndex 2)
			(SETQ nCount (dcl-Tree-COUNTITEMS Misc/DemoModal/TreeControl1))
			(IF (= nCount 0)
				(PROGN
					(dcl-Tree-ADDPARENT Misc/DemoModal/TreeControl1 "slide1" "t1")
					(dcl-Tree-ADDPARENT Misc/DemoModal/TreeControl1 "slide library" "t2")
					(dcl-Tree-ADDPARENT Misc/DemoModal/TreeControl1 "slide3" "t3")
					(dcl-Tree-ADDCHILD Misc/DemoModal/TreeControl1
									'(	("t2" "Child1" "C1")
										("t2" "Child2" "C2")
										("t2" "Child3" "C3")
									 )
					)
					(dcl-Tree-SELECTITEM Misc/DemoModal/TreeControl1 "t1")
				)
			)
		)
		;; cond tab 3
		((= nSelIndex 3) (dcl-ComboBox-ADDCOLOR Misc/DemoModal/ComboBox4 156))
	)
)


(defun c:Misc/DemoModal/cmdRun#OnClicked (/ cnt wait)
	(setq cnt 0)
	(while (< cnt 100)
		(setq wait 10000)
		(while (> (setq wait (1- wait)) 0))
		(dcl-Control-SetValue Misc/DemoModal/ProgressBar1 (setq cnt (1+ cnt)))
	)
	(princ)
)



;; this function gets fired when the BlockNameList selection is changed
(DEFUN c:DemoModal_BlockNameList_SelectionChanged (nSelCount sSelText /)
	(dcl-Control-SETBLOCKNAME Misc/DemoModal/BlockView sSelText)
)

;; this function gets fired when the TreeControl selection is changed
(DEFUN c:DemoModal_TreeControl1_SelectionChanged (sSelText sSelKey / sParent FileName Path)
	;; get the parent info
	(SETQ sParent (dcl-Tree-GETPARENT Misc/DemoModal/TreeControl1 sSelKey))
	(setq FileName (strcat
					*ODCL:Prefix
					(IF (NULL sParent)
						(strcat sSelText ".sld")
						;; else
						(strcat
							(dcl-Tree-GETITEMTEXT Misc/DemoModal/TreeControl1
													sParent
							)
							".slb"
						)
					)
				)
	)
	(IF (FINDFILE FileName)
		(dcl-SlideView-LOAD Misc/DemoModal/SlideView1
							FileName
							(IF (NULL sParent)
								""
								sSelText
							)
		)
		;; else
		(dcl-SlideView-CLEAR Misc/DemoModal/SlideView1)
	)
)


(DEFUN c:DemoModal_BrowseFolders_Clicked (/ path)
	(IF (SETQ path (dcl-SELECTFOLDER "Select a folder" nil nil 81))
		(ALERT path)
	)
)


;; this function retrieves all the blocks and adds their names to the BlockName ListBox
(DEFUN AddBlocksToListBox (/ BlockInfo BlkLst Blk)
	(dcl-ListBox-CLEAR Misc/DemoModal/BlockNameList)
	(SETQ BlockInfo (TBLNEXT "BLOCK" T))
	;; get the first block in the block table record
	;; loop through all block records
	;; get block name
	;; ignore anonymous blocks, dim's, xref's, etc...
	;; add the block name to the list 'BlkLst
	;; goto next block
	(WHILE BlockInfo
		(SETQ blk (CDR (ASSOC 2 BlockInfo)))
		(IF (NOT (WCMATCH blk "`*U*,`*D*,`*X*,`*T*,_*,*|*,A$*"))
			(SETQ BlkLst (APPEND BlkLst (LIST blk)))
		)
		(SETQ BlockInfo (TBLNEXT "BLOCK"))
	)
	(IF (NULL BlkLst)
		(dcl-Control-SETVISIBLE Misc/DemoModal/lblNoBlocks T)
		(dcl-Control-SETVISIBLE Misc/DemoModal/lblNoBlocks nil)
	)
	(dcl-ListBox-ADDLIST Misc/DemoModal/BlockNameList BlkLst)
	;;
	;;_ alternate method adding one string at a time to the BlockList
;;;	; get the first block in the block table record
;;;	(setq BlockInfo (tblnext "BLOCK" T))
;;;	; add the first item to the BlockName ListBox with Add String
;;;	(if BlockInfo
;;;		(dcl-ListBox-AddString Misc/DemoModal/BlockNameList (cdr (assoc 2 BlockInfo)))
;;;	);_ if
;;;	; do loop until all the blocks have been added
;;;	(while BlockInfo
;;;		(setq BlockInfo (tblnext "BLOCK"))
;;;		; add the item to the BlockName ListBox
;;;		(if BlockInfo
;;;			(dcl-ListBox-AddString Misc/DemoModal/BlockNameList (cdr (assoc 2 BlockInfo)))
;;;		);_ if
;;;	);_ while
	(PRINC)
)

(princ)

;|*OpenDCL Samples Epilog*|;

;;;######################################################################
;;;######################################################################
;;; The following section of code is designed to locate OpenDCL Studio
;;; sample files in the samples folder by prefixing the filename with
;;; the path prefix that was saved in the registry by the installer.
;;; The global *ODCL:Prefix and function *ODCL:Samples-FindFile
;;; are used throughout the samples.
;;;
(or *ODCL:Samples-FindFile
	(defun *ODCL:Samples-FindFile (file)
		(setq *ODCL:Prefix
			(cond
				(	*ODCL:Prefix
				) ;_ already defined
				(	(vl-registry-read
						 "HKEY_CURRENT_USER\\SOFTWARE\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 32-bit location
				(	(vl-registry-read
						 "HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 32-bit location
				(	(vl-registry-read
						 "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\OpenDCL"
						 "SamplesFolder"
					)
				) ;_ 64-bit location
			)
		)
		(cond
			((findfile file)) ; check the support path first
			(*ODCL:Prefix (findfile (strcat *ODCL:Prefix file)))
			(file)
		)
	)
)

;; If AllSamples is active, run the main function immediately; otherwise
;; display a banner. The extra gymnastics allow the sample name to be
;; specified in only one place, thus making it easier to reuse this code.
(	(lambda (demoname)
		(if *ODCL:AllSamples
			(progn
				(princ (strcat "'" demoname "\n"))
				(apply (read (strcat "C:" demoname)) nil)
			)
			(progn
				(princ (strcat "\n" demoname " OpenDCL sample loaded"))
				(princ (strcat " (Enter " (strcase demoname) " command to run)\n"))
			)
		)
	)
	"Misc"
)
(princ)

;;;######################################################################
;;;######################################################################
