/*
Please refer to readme.html for full Instructions

Text[...]=[title,text]

Style[...]=[TitleColor,TextColor,TitleBgColor,TextBgColor,TitleBgImag,TextBgImag,TitleTextAlign,TextTextAlign, TitleFontFace, TextFontFace, TipPosition, StickyStyle, TitleFontSize, TextFontSize, Width, Height, BorderSize, PadTextArea, CoordinateX , CoordinateY, TransitionNumber, TransitionDuration, TransparencyLevel ,ShadowType, ShadowColor]
*/

var FiltersEnabled = 0 // if your not going to use transitions or filters in any of the tips set this to 0

Text[10]=["OpenDCL Control Palette","This toolbar provides a palette of controls which can be added to your forms.  Click on the toolbar to select a control and place it on the dialog"]
Text[11]=["Form","The construction grid visible on the form can be modified by accessing the Tools menu "]
Text[12]=["The Project Window","This window allows you to manage various aspects of your OpenDCL project"]
Text[13]=["Properties and Events","You can set various properties for forms and controls in this window. Some of these can be changed later at run-time via Lisp."]
Text[14]=["Tab Order","Use this window to manage the tab order of your controls"]
Text[15]=["Tools Menu","Set defaults and manage project options"]
Text[16]=["Font Override","Use this control to change the selected text item's font properties from the default setting (default set in the Tools Menu)"]



Style[0]=["white","black","#666666","FFFCEA","","","","","","Verdana","right","sticky","",+1,200,"",1,2,10,10,"","","","",""]
Style[1]=["white","black","#666666","FFFCEA","","","","","","Verdana","right","","",+1,200,"",1,2,10,10,"","","","",""]
Style[2]=["white","black","#000099","#E8E8FF","","","","","","","left","","","",200,"",2,2,10,10,"","","","",""]
Style[3]=["white","black","#000099","#E8E8FF","","","","","","","float","","","",200,"",2,2,10,10,"","","","",""]
Style[4]=["white","black","#000099","#E8E8FF","","","","","","","fixed","","","",200,"",2,2,1,1,"","","","",""]
Style[5]=["white","black","#000099","#E8E8FF","","","","","","","","sticky","","",200,"",2,2,10,10,"","","","",""]
Style[6]=["white","black","#000099","#E8E8FF","","","","","","","","keep","","",200,"",2,2,10,10,"","","","",""]
Style[7]=["white","black","#000099","#E8E8FF","","","","","","","","","","",200,"",2,2,40,10,"","","","",""]
Style[8]=["white","black","#000099","#E8E8FF","","","","","","","","","","",200,"",2,2,10,50,"","","","",""]
Style[9]=["white","black","#000099","#E8E8FF","","","","","","","","","","",200,"",2,2,10,10,51,0.5,75,"simple","gray"]
Style[10]=["white","black","black","white","","","right","","Impact","cursive","center","",3,5,200,150,5,20,10,0,50,1,80,"complex","gray"]
Style[11]=["white","black","#000099","#E8E8FF","","","","","","","","","","",200,"",2,2,10,10,51,0.5,45,"simple","gray"]
Style[12]=["white","black","#000099","#E8E8FF","","","","","","","","","","",200,"",2,2,10,10,"","","","",""]

applyCssFilter()

