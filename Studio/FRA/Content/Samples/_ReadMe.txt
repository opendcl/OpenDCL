OpenDCL Samples
The purpose of these samples is to demonstrate the various features of OpenDCL.
They are designed to run without having to add the “Samples” folder to your AutoCAD
support path. For them to run properly you should load “_MasterDemo.lsp” first, by
dragging it into the Acad window. If you want to run the samples without having to
load the MasterDemo, you will need to add “C:\Program Files\OpenDCL Studio\<ENU>\Samples”
(depending where you installed OpenDCL) to your Acad support paths. There are some
utility functions in “_OpendclUtils.LSP” that handle the demand loading of the
OpenDCL.arx & the project loading of the .ODCL files. If you do not wish to use
demand loading (command “OPENDCL”) refer to the “ManualLoading.lsp” file for a
demonstration of loading the arx manually.


