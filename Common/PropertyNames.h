#pragma once

#include <set>


enum Prop::Id;
typedef std::set< Prop::Id > T_PropertyIdSet;


LPCTSTR GetPropertyName( Prop::Id eProp );
Prop::Id GetPropertyId( LPCTSTR pszName );
void GetPropertyIdSet( T_PropertyIdSet& IdSet ); //for iterating over all defined IDs
