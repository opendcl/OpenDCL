#include "StdAfx.H"
#include "VarUtils.H"
#include "DclControlObject.h"
//#include "ObjectBrowser.h"
#include "Project.h"


class INVOKEKIND_ENTRY
{
public:
   LPCTSTR pszName;
   INVOKEKIND invokekind;
};

static INVOKEKIND_ENTRY g_aInvokeKinds[] =
{
   { _T( "Method" ), INVOKE_FUNC },
   { _T( "PropGet" ), INVOKE_PROPERTYGET },
   { _T( "PropPut" ), INVOKE_PROPERTYPUT },
   { _T( "PropPutRef" ), INVOKE_PROPERTYPUTREF }
};

const int NUM_INVOKEKINDS = sizeof( g_aInvokeKinds )/sizeof(
   g_aInvokeKinds[0] );

LPCTSTR InvokeKindToString( INVOKEKIND invokekind )
{
   int iInvokeKind;

   for( iInvokeKind = 0; iInvokeKind < NUM_INVOKEKINDS; iInvokeKind++ )
   {
	  if( g_aInvokeKinds[iInvokeKind].invokekind == invokekind )
	  {
		 return( g_aInvokeKinds[iInvokeKind].pszName );
	  }
   }

   return( NULL );
}

class VARTYPE_ENTRY
{
public:
   LPCTSTR pszName;
   VARTYPE vt;
};

static VARTYPE_ENTRY g_aVarTypes[] =
{
   { _T( "VT_EMPTY" ), VT_EMPTY },
   { _T( "VT_BOOL" ), VT_BOOL },
   { _T( "VT_UI1" ), VT_UI1 },
   { _T( "VT_UI2" ), VT_UI2 },
   { _T( "VT_UI4" ), VT_UI4 },
   { _T( "VT_UI8" ), VT_UI8 },
   { _T( "VT_I1" ), VT_I1 },
   { _T( "VT_I2" ), VT_I2 },
   { _T( "VT_I4" ), VT_I4 },
   { _T( "VT_I8" ), VT_I8 },
   { _T( "VT_R4" ), VT_R4 },
   { _T( "VT_R8" ), VT_R8 },
   { _T( "VT_BSTR" ), VT_BSTR },
   { _T( "VT_CY" ), VT_CY },
   { _T( "VT_DATE" ), VT_DATE },
   { _T( "VT_ERROR" ), VT_ERROR },
   { _T( "VT_COLOR" ), VT_COLOR },
   { _T( "VT_FONT" ), VT_FONT },
   { _T( "VT_UNKNOWN" ), VT_UNKNOWN }
   
};

LPCTSTR VTIToString( int iType )
{
   return( g_aVarTypes[iType].pszName );
}

VARTYPE VTIToVT( int iType )
{
   return( g_aVarTypes[iType].vt );
}

LPCTSTR VTToString( VARTYPE vt )
{
   int iType;

   for( iType = 0; iType < NUM_VARTYPES; iType++ )
   {
	  if( g_aVarTypes[iType].vt == vt )
	  {
		 return( g_aVarTypes[iType].pszName );
	  }
   }

   return( NULL );
}

int VTToVTI( VARTYPE vt )
{
   int iType;

   for( iType = 0; iType < NUM_VARTYPES; iType++ )
   {
	  if( g_aVarTypes[iType].vt == vt )
	  {
		 return( iType );
	  }
   }

   return( -1 );
}


CString VARTYPEtoString(VARTYPE vt)
{
	switch(vt)
	{
	case VT_BOOL:
	case VT_I1:
	case VT_UI1:
		return _T("Bool");
		break;                         

	case VT_I2:
	case VT_UI2:
	case VT_INT:
	case VT_UINT:
		return _T("Integer");
		break;

	case VT_CY:
	case VT_I4:
	case VT_UI4:
		return _T("Long");
		break;

	case VT_R4:
	case VT_R8:
		return _T("Real");
		break;

	case VT_BSTR:
	case VT_LPSTR:
		return _T("String");
		break;

	case VT_UNKNOWN:
	case VT_DISPATCH:
		{
			// * Commented below and just return "OLE Object", otherwise it won't work in "Common" folder [ORW]
			//
			//if (pProperty == NULL && pMethod == NULL)
			//	return _T("OLE Object");
			//else if (pProperty != NULL)
			//{
			//	CDclControlObject *pOleObject = activeProject->GetAxOleObject(pProperty);
			//	if (pOleObject != NULL)
			//	{
			//		CString sType = pOleObject->m_Name;
			//		if (pObjectBrowser != NULL)
			//		{
			//			pObjectBrowser->LoadOleObjectIntoTree(pOleObject);
			//			return sType;
			//		}
			//	}
			//	return _T("OLE Object");
			//}
			//else if (pMethod != NULL)
			//{
			//	CDclControlObject *pOleObject = activeProject->GetAxOleObject(pMethod);
			//	if (pOleObject != NULL)
			//	{
			//		if (pObjectBrowser != NULL)
			//		{
			//			pObjectBrowser->LoadOleObjectIntoTree(pOleObject);
			//			return pOleObject->m_Name;
			//		}
			//	}			
			//	return _T("OLE Object");
			//}
			return _T("OLE Object");
			break;
		}
	case VT_VOID:
	case VT_EMPTY:
	case VT_NULL:
		break;

	case VT_PTR: // should be handled by BuildParam()
		return _T("Pointer");
		break;

	case VT_VARIANT:	
		break;

	default:
		break;
	}
		
	return _T("");
}

CString VariantToString( const VARIANT& var )
{
    
	USES_CONVERSION;
   CString strResult;
   COleVariant varString;
   BOOL tHandled;

   tHandled = FALSE;
   switch( var.vt )
   {
   case VT_BOOL:	   
	  if( var.boolVal == VARIANT_FALSE )
			strResult = _T("FALSE");
	  else
			strResult = _T("TRUE");
	  tHandled = TRUE;
	  break;
   }

   if( !tHandled )
   {
	  varString = var;
	  TRY
	  {
		 varString.ChangeType( VT_BSTR );
	  }
	  CATCH( COleException, e )
	  {
		 
	  }
	  END_CATCH

	  strResult = varString.bstrVal;
   }

   return( strResult );
}
