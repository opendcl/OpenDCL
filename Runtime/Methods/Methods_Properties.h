// Methods_Properties.h : header file
//

#pragma once

#include "PropertyIds.h"

class CPropertyObject;
class CDclControlObject;


bool SetCtrlProperty(Prop::Id id);
bool GetCtrlProperty(Prop::Id id);

int ShowToolTip();

int SetControlFocus();
int SetProperty();
int GetProperty();
int ZOrder();
void UpdateControl(CWnd *pWnd, TPropertyPtr pProperty, CDclControlObject *pControl, Prop::Id nPropertyId);
bool Property_SetByList(CDclControlObject *pControl);
bool SetPropertyObject(TPropertyPtr pProperty, TPropertyPtr pPropArg, CDclControlObject *pCtrl = NULL);
int Control_GetCurPos();
int Control_SetPos();
bool DoSetPosByList();
void SetReturnValue(int nType, TPropertyPtr pProperty);
int Control_FlushGraphicButtons();
int ForceUpdateNow();
int GetControlHwnd();
int ProgressBar_SetPos();
