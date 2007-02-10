#pragma once

#include <set>


enum PropertyId;
typedef std::set< PropertyId > T_PropertyIdSet;


LPCTSTR GetPropertyName( PropertyId eProp );
PropertyId GetPropertyId( LPCTSTR pszName );
void GetPropertyIdSet( T_PropertyIdSet& IdSet ); //for iterating over all defined IDs
