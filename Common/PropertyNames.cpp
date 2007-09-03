#include "stdAfx.h"
#include "PropertyNames.h"
#include "Workspace.h"
#include "PropertyIds.h"
#include <map>
#include <string>
#include <locale>

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif //_UNICODE



//For now, property resources are ordered sequentially, so just add a base offset.
//It would be more flexible and less error prone to create a constant array that maps
//property id to resource id. This may need to be done if resource id collisions occur. [ORW]
static UINT GetPropertyNameResourceId( Prop::Id eProp )
{
	static const int PROPRESIDBASE = 210;
	return eProp + PROPRESIDBASE;
}


//wrap the map in a class in order to manage delayed initialization
//(this avoids the time consuming resource loading when names are not needed)
class PropNameMap
{
	typedef std::map< tstring, Prop::Id > T_MapNameToId;
	typedef std::map< Prop::Id, tstring > T_MapIdToName;
	T_MapNameToId mmapNameToId;
	T_MapIdToName mmapIdToName;
	bool mbInitialized;
public:
	PropNameMap() : mbInitialized( false ) {}
	PropNameMap( bool bInitializeNow ) : mbInitialized( false ) { if( bInitializeNow ) Initialize(); }
	~PropNameMap() {}

public:
	LPCTSTR GetName( Prop::Id eProp );
	Prop::Id GetId( LPCTSTR pszName );
	void GetSet( T_PropertyIdSet& IdSet );

protected:
	void Initialize();

	static tstring& toLString( tstring& sInput )
  {
		for( tstring::iterator iter = sInput.begin(); iter != sInput.end(); ++iter )
			*iter = std::tolower( *iter, std::locale() );
		return sInput;
  }
};


//delayed construction of the class
static PropNameMap& theMap()
{
	static PropNameMap _theMap;
	return _theMap;
}


//Exported interface
LPCTSTR GetPropertyName( Prop::Id eProp )
{
	return theMap().GetName( eProp );
}

Prop::Id GetPropertyId( LPCTSTR pszName )
{
	return theMap().GetId( pszName );
}

void GetPropertyIdSet( T_PropertyIdSet& IdSet )
{
	return theMap().GetSet( IdSet );
}

//Internal
LPCTSTR PropNameMap::GetName( Prop::Id eProp )
{
	Initialize();
	T_MapIdToName::const_iterator iter = mmapIdToName.find( eProp );
	if( iter != mmapIdToName.end() )
		return iter->second.c_str();
	return NULL;
}

Prop::Id PropNameMap::GetId( LPCTSTR pszName )
{
	Initialize();
	T_MapNameToId::const_iterator iter = mmapNameToId.find( toLString( tstring( pszName ) ) );
	if( iter != mmapNameToId.end() )
		return iter->second;
	return Prop::_Private;
}

void PropNameMap::GetSet( T_PropertyIdSet& IdSet )
{
	Initialize();
	IdSet.clear();
	for( T_MapIdToName::const_iterator iter = mmapIdToName.begin(); iter != mmapIdToName.end(); ++iter )
		IdSet.insert( iter->first );
}

void PropNameMap::Initialize()
{
	if( mbInitialized )
		return; //no-op
	//iterate over all defined property ids to load and save the associated name from resources
	for( int idxProp = Prop::_MinId; idxProp <= Prop::_MaxId; ++idxProp )
	{
		Prop::Id id = static_cast<Prop::Id>(idxProp);
		tstring sName( theWorkspace.LoadResourceString( GetPropertyNameResourceId( id ) ) );
		if( !sName.empty() )
		{
			mmapIdToName[id] = sName;
			mmapNameToId[toLString( sName )] = id;
		}
	}
	mbInitialized = true;
}
