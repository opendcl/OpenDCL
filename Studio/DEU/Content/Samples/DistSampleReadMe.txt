--------------------------------------------------------------------------------

AutoLISP Application Code Deployment Samples

--------------------------------------------------------------------------------

Overview

    The DistSample*.* files are lisp source code and OpenDCL files to
    demonstrate how to embed an OpenDCL project (saved as a .odcl.lsp
    file) into a vlx file, and then how to use said resource as one 
    might normally use a regular OpenDCL project file.

    A new sample (DistSample2.lsp) demonstrates how the same dialog
    data can be embedded directly into the lisp source code as an
    inline text stream. Generally speaking, this would be the most
    frequent (easier/faster) used method of the two. Having said that
    it's nice to have the options. =)

    Why would you want to do this? The primary reason is one of
    simplified distribution, though some developers might want to use
    it as a mild form of security to protect their dialog designs or
    to ensure non modified dialog versions are always utilized. Note
    the use of the word 'mild'. If you are very concerned about
    security there are additional things that can be done to increase
    the security of vlx file content including resources but that is
    outside the scope and intent of this tutorial.

--------------------------------------------------------------------------------

Requirements

    AutoCAD 2004+

    OpenDCL 5.0+

--------------------------------------------------------------------------------

Contents

    DistSamplesReadMe.txt - This file.

    DistSample.odcl       - OpenDCL project.

    DistSample.odcl.lsp   - Same as DistSample.odcl but Base64 encoded and
                            wrapped in lisp (note, OpenDCL studio does this
                            for you when you save it to .lsp format).

    DistSample1.lsp       - LISP Source, uses DistSample.odcl if running as
                            lisp. If running as vlx attempts to load
                            DistSample.odcl.lsp as a text resource that has 
                            been compiled to the vlx file.

    DistSample2.lsp       - LISP Source. Same as DistSample1.lsp but uses
                            the DistSample.odcl.lsp data as an in-line
                            stream rather than a text resource embedded in 
                            the vlx file.

    DistSample1.prv       - Visual LISP project (make) file for DistSample1.vlx
                            output.

    DistSample2.prv       - Visual LISP project (make) file for DistSample2.vlx
                            output.

    DistSample1.vlx       - Compiled version of DistSample1.lsp.

    DistSample2.vlx       - Compiled version of DistSample2.lsp.

--------------------------------------------------------------------------------

DistSample.odcl

    Contains one form with a simple label (the label is of no
    consequence but mentioned for completeness) and one button.

    Varnames:

        Form   = DistSample/MainForm
        Label  = DistSample/MainForm/MainLabel
        Button = DistSample/MainForm/OkButton

    Enabled events:

        DistSample/MainForm           = none
        DistSample/MainForm/MainLabel = none
        DistSample/MainForm/OkButton  = OnClick

    Project is not pass worded.

--------------------------------------------------------------------------------

DistSample.odcl.lsp

    Exactly the same as DistSample.odcl but saved to .lsp format via
    the OpenDCL dialog editor (File > Saveas > DistSample.odcl.lsp).

--------------------------------------------------------------------------------

DistSample1.lsp

    LISP Source that demonstrates how to use both the DistSample.odcl
    project (when called as DistSample1.lsp, for example during
    development) as well as the DistSample.odcl.lsp (when called from
    DistSample.vlx, which attempts to use DistSample.odcl.lsp as an
    embedded text resource).

--------------------------------------------------------------------------------

DistSample2.lsp

    LISP Source that demonstrates how to use the DistSample.odcl.lsp
    project data as an (inline) embedded text stream.

--------------------------------------------------------------------------------

DistSample1.prv

    Visual LISP project (make) file. Basically has this content:
    
        (PRV-DEF (:target . "DistSample1.vlx")
        	 (:active-x . T)
        	 (:separate-namespace)
        	 (:protected . T)
        	 (:load-file-list (:lsp "DistSample1.lsp"))
        	 (:require-file-list (:txt "DistSample.odcl.lsp"))
        	 (:ob-directory)
        	 (:tmp-directory)
        	 (:optimization . st)
        )    

--------------------------------------------------------------------------------

DistSample2.prv

    Visual LISP project (make) file. Basically has this content:

        (PRV-DEF (:target . "DistSample2.vlx")
             (:active-x . T)
             (:separate-namespace)
             (:protected . T)
             (:load-file-list (:lsp "DistSample2.lsp"))
             (:require-file-list)
             (:ob-directory)
             (:tmp-directory)
             (:optimization . st)
        )        

--------------------------------------------------------------------------------

DistSample1.vlx

    Compiled version of DistSample1.lsp. Compiled by using the Visual
    LISP make file DistSample1.prv.

--------------------------------------------------------------------------------

DistSample2.vlx

    Compiled version of DistSample2.lsp. Compiled by using the Visual
    LISP make file DistSample2.prv.

--------------------------------------------------------------------------------

Credits

    Owen Wengerd, for his passionate and spirited commitment to the
    OpenDCL project, and for his willingness to try new things, like
    coding up the logic that made ODCL project embedding possible
    and easy to use. Kudos Owen.
    
    The growing OpenDCL community.

    Michael Puckett, author of code and text in this tutorial, c/w
    typos and mistakes.

--------------------------------------------------------------------------------

Questions?

    Please visit the following sites for more information:

        http://sourceforge.net/projects/opendcl
        http://www.opendcl.com

--------------------------------------------------------------------------------

Thank you

    Michael Puckett (puckettm@gmail.com).

--------------------------------------------------------------------------------