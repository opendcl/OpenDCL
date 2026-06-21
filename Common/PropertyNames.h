#pragma once

#include <set>


namespace Prop { enum Id; }
typedef std::set< Prop::Id > T_PropertyIdSet;


LPCTSTR GetPropertyName( Prop::Id eProp );
LPCTSTR GetPropertyApiName( Prop::Id eProp );
Prop::Id GetPropertyId( LPCTSTR pszName );
void GetApiPropertyIdSet( T_PropertyIdSet& IdSet ); //for iterating over all defined IDs
