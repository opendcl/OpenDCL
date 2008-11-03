// Methods_Control.h : header file
//

#pragma once

#include "PropertyIds.h"


bool SetCtrlProperty( Prop::Id id );
bool GetCtrlProperty( Prop::Id id );

template< Prop::Id _id >
ADSRESULT T_GetCtrlProperty() { return (GetCtrlProperty( _id )? RSRSLT : RSERR); }

template< Prop::Id _id >
ADSRESULT T_SetCtrlProperty() { return (SetCtrlProperty( _id )? RSRSLT : RSERR); }

namespace Control
{
ADSRESULT GetHWND();
ADSRESULT GetName();
ADSRESULT GetProperties();
ADSRESULT GetProperty();
ADSRESULT GetPos();
ADSRESULT Redraw();
ADSRESULT SetFocus();
ADSRESULT SetPos();
ADSRESULT SetProperty();
ADSRESULT ShowToolTip();
ADSRESULT ZOrder();
};
