#pragma once

#include "PropertyIds.h"
#include "ControlTypes.h"
#include <vector>


typedef std::vector< CString > TEnumNames;

const TEnumNames& GetPropEnumNames( Prop::Id id, ControlType type = _CtlInvalid );
