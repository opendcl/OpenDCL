// Methods_Properties.h : header file
//

#pragma once

#include "PropertyIds.h"

class CPropertyObject;
class CDclControlObject;


bool SetCtrlProperty(PropertyId id);
bool GetCtrlProperty(PropertyId id);

int ShowToolTip();

int SetControlFocus();
int SetProperty();
int GetProperty();
int ZOrder();
void UpdateControl(CWnd *pWnd, RefCountedPtr< CPropertyObject > pProperty, CDclControlObject *pControl, PropertyId nPropertyId);
bool Property_SetByList(CDclControlObject *pControl);
bool SetPropertyObject(RefCountedPtr< CPropertyObject > pProperty, RefCountedPtr< CPropertyObject > pPropArg, CDclControlObject *pCtrl = NULL);
int Control_GetCurPos();
int Control_SetPos();
bool DoSetPosByList();
void SetReturnValue(int nType, RefCountedPtr< CPropertyObject > pProperty);
int Control_FlushGraphicButtons();
int ForceUpdateNow();
int GetControlHwnd();
int ProgressBar_SetPos();
