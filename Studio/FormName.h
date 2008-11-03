#pragma once

#include "PtrTypes.h"

enum FormType;

CString GetFormDisplayName( FormType type );
CString GetFormDisplayName( TDclFormPtr pForm );
CString GetFormApiName( FormType type );
CString GetFormApiName( TDclFormPtr pForm );
