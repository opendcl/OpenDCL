// Methods_Properties.h : header file
//

#pragma once

#include "PropertyIds.h"


bool SetCtrlProperty( Prop::Id id );
bool GetCtrlProperty( Prop::Id id );

namespace Control
{
ADSRESULT SetProperty();
ADSRESULT GetProperty();
ADSRESULT ShowToolTip();
ADSRESULT SetFocus();
ADSRESULT ZOrder();
ADSRESULT GetPos();
ADSRESULT SetPos();
ADSRESULT Redraw();
ADSRESULT GetName();
};

namespace ProgressBar
{
ADSRESULT SetPos();
};
