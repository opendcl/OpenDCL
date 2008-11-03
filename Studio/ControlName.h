#pragma once

#include "PtrTypes.h"

enum ControlType;

CString GetControlDisplayName( ControlType type );
CString GetControlDisplayName( TDclControlPtr pDclControl );
CString GetControlDisplayName( CLSID clsid );
CString GetControlApiName( ControlType type );
CString GetControlApiName( TDclControlPtr pDclControl );
CString GetControlApiName( CLSID clsid );
