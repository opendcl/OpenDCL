Contents of DistSample.zip

    ReadMe.txt      - This file.
    
    DistSample.odc  - OpenDCL project.
    DistSample.odce - Same as DistSample.odc but encoded.
    DistSample.lsp  - LISP Source.
    DistSample.prv  - Visual LISP project (make) file.
    DistSample.vlx  - Compiled version of DistSample.lsp complete with
                      DistSample.odce embedded as a text file resource. 
                      Ready to run as is.
                      
Overview

    This zip file contains the lisp source code and OpenDCL files to 
    demonstrate how to embed an OpenDCL project into a vlx file, and 
    then how to use said resource as one might normally use a regular 
    OpenDCL project file.
    
    Why would you want to do this? The primary reason is one of 
    simplified distribution, though some developers might want to use 
    it as a mild form of security to protect their dialog designs or to 
    ensure non modified dialog versions are always utilized. Note the 
    use of the word 'mild'. If you are very concerned about security 
    there are additional things that can be done to increase the security 
    of vlx file content including resources but that is outside the 
    scope and intent of this tutorial.
    
Requirements    

    AutoCAD 2004+
    
    OpenDCL 4.0, beta 17 or newer.
    
DistSample.odc

    Contains one form with a simple label (the label is of no consequence 
    but mentioned for completeness) and one button.
    
    Varnames:
    
        Form   = DistSample_MainForm
        Label  = DistSample_MainForm_MainLabel
        Button = DistSample_MainForm_OkButton
        
    Enabled events:
        
        DistSample_MainForm           = none
        DistSample_MainForm_MainLabel = none
        DistSample_MainForm_OkButton  = OnClick
        
    Project is not pass worded.        
    
DistSample.odce        

    Exactly the same as DistSample.odc but saved to .odce format via 
    the OpenDCL dialog editor (File > Saveas > DistSample.odce).
    
DistSample.lsp

    LISP Source that demonstrates how to use both the DistSample.odc 
    project (when called as DistSample.lsp, for example during 
    development) as well as the DistSample.odce (when called from 
    DistSample.vlx, which attempts to use DistSample.odce as an 
    embedded text resource).
    
DistSample.vlx

    Compiled version of DistSample.lsp. Compiled by using the Visual 
    LISP make file DistSample.prv.
    
DistSample.prv

    Visual LISP project (make) file. Basically has this content:
    
        (PRV-DEF (:target . "DistSample.vlx")
        	 (:active-x . T)
        	 (:separate-namespace)
        	 (:protected . T)
        	 (:load-file-list (:lsp "DistSample.lsp"))
        	 (:require-file-list (:txt "DistSample.odce"))
        	 (:ob-directory)
        	 (:tmp-directory)
        	 (:optimization . st)
        )    

Credits

    Owen Wengerd, for his passionate and spirited commitment to the 
    OpenDCL project, and for his willingness to try new things, like 
    coding up the logic that made ODCL project embedding possible 
    and easy to use. Kudos Owen.
    
    Michael Puckett, author of all code and text in this little 
    tutorial.

Questions?

    Please visit the following sites for more information:
    
        http://sourceforge.net/projects/opendcl
        http://www.theswamp.org/index.php?board=38.0
        
Thank you

    Michael Puckett, puckettm@gmail.com