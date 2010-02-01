#include "StdAfx.h"

#include "ArxAxContainerCtrl.h"
#include "ArxWorkspace.h"
#include "AxEventDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "ControlPane.h"
#include "InvokeMethod.h"
#include "ArgumentsRetrieval.h"
#include "Project.h"
#include "VarUtils.h"
#include "Workspace.h"


static CString LongToS( LONG lValue )
{
	CString sVal;
	sVal.Format( _T("%d"), lValue );
	return sVal;
}


static resbuf* LongToRB( LONG lValue )
{
	if( lValue > SHRT_MAX || lValue < SHRT_MIN )
	{
		resbuf* prb = acutNewRb( RTLONG );
		prb->resval.rlong = (long)lValue;
		return prb;
	}
	resbuf* prb = acutNewRb( RTSHORT );
	prb->resval.rint = (short)lValue;
	return prb;
}


static CString ULongToS( ULONG lValue )
{
	CString sVal;
	sVal.Format( _T("%u"), lValue );
	return sVal;
}


static resbuf* ULongToRB( ULONG lValue )
{
	if( lValue > USHRT_MAX )
	{
		resbuf* prb = acutNewRb( RTLONG );
		prb->resval.rlong = (long)lValue;
		return prb;
	}
	resbuf* prb = acutNewRb( RTSHORT );
	prb->resval.rint = (short)lValue;
	return prb;
}


static CString LongLongToS( LONGLONG lValue )
{
	CString sVal;
	sVal.Format( _T("%I64d"), lValue );
	return sVal;
}


static resbuf* LongLongToRB( LONGLONG lValue )
{
	if( lValue > LONG_MAX || lValue < LONG_MIN )
	{
		resbuf* prb = acutNewRb( RTREAL );
		prb->resval.rreal = (ads_real)lValue;
		return prb;
	}
	if( lValue > SHRT_MAX || lValue < SHRT_MIN )
	{
		resbuf* prb = acutNewRb( RTLONG );
		prb->resval.rlong = (long)lValue;
		return prb;
	}
	resbuf* prb = acutNewRb( RTSHORT );
	prb->resval.rint = (short)lValue;
	return prb;
}


static CString DoubleToS( double dValue )
{
	static CString sDefault = _T("0.0");
	CString sReturn;
	if( RTNORM != acdbRToS( dValue, 2, 8, sReturn.GetBuffer( 256 ) ) )
		return sDefault;
	sReturn.ReleaseBuffer();
	return sReturn;
}


static resbuf* DoubleToRB( double dValue )
{
	resbuf* prb = acutNewRb( RTREAL );
	prb->resval.rreal = (ads_real)dValue;
	return prb;
}


static CString DateToS( DATE dValue )
{
	COleDateTime dt( dValue );
	CString sReturn;
	sReturn.Format( _T("'(%d %d %d)"), dt.GetYear(), dt.GetMonth(), dt.GetDay() );
	return sReturn;
}


static resbuf* DateToRB( DATE dValue )
{
	COleDateTime dt( dValue );
	resbuf* prb = acutNewRb( RT3DPOINT );
	prb->resval.rpoint[0] = (ads_real)dt.GetYear();
	prb->resval.rpoint[1] = (ads_real)dt.GetMonth();
	prb->resval.rpoint[2] = (ads_real)dt.GetDay();
	return prb;
}


static resbuf* IUnknownToRB( IUnknown* pValue )
{
	resbuf* prb = acutNewRb( RTENAME );
	prb->resval.rlname[0] = (LONG_PTR)pValue;
	prb->resval.rlname[1] = odcl::ptrIUnknown;
	theArxWorkspace.AddUnknown( pValue );
	return prb;
}


static resbuf* IDispatchToRB( IDispatch* pValue )
{
	IUnknown* pUnknown = NULL;
	HRESULT hr = pValue->QueryInterface( IID_IUnknown, (void**)&pUnknown );
	if( hr != S_OK )
		return NULL;
	return IUnknownToRB( pUnknown );
}


static resbuf* StringToRB( const BSTR& sValue )
{
	resbuf* prb = acutNewRb( RTSTR );
	acutNewString( bstr_t( sValue ), prb->resval.rstring );
	return prb;
}

static bool ResbufToVariantArg( resbuf*& pArgs, VARIANTARG& varArg )
{
	variant_t _arg;
	switch( pArgs->restype )
	{	
		case RTREAL:
		case RTANG:
		case RTORINT:
			{
				_arg = pArgs->resval.rreal;
				break;
			}
		case RTSHORT:
			{
				_arg = pArgs->resval.rint;
				break;
			}
		case RTLONG:
			{
				_arg = pArgs->resval.rlong;
				break;
			}
		case RTENAME:
			{
				if ((varArg.vt & VT_TYPEMASK) == VT_UNKNOWN)
					_arg = (IUnknown*)pArgs->resval.rlname[0];
				else
					return false;
				break;
			}
		case RTSTR:
			{
				_arg = pArgs->resval.rstring;				
				break;
			}
		case RTT:
			{
				_arg.vt = VT_BOOL;
				_arg.boolVal = VARIANT_TRUE;				
				break;
			}
		case RTNIL:
			{
				if ((varArg.vt & VT_TYPEMASK) == VT_VARIANT)
				{
					_arg.vt = VT_VARIANT|VT_BYREF;
					_arg.pvarVal = NULL;
				}
				else
				{
					_arg.vt = VT_BOOL;
					_arg.boolVal = VARIANT_FALSE;				
				}
				break;
			}
		case RT3DPOINT:
		case RTPOINT:
			{
				switch (varArg.vt & VT_TYPEMASK)
				{
				case VT_DATE:
					{
						COleDateTime Date;
						Date.SetDate(
							pArgs->resval.rpoint[0],
							pArgs->resval.rpoint[1],
							pArgs->resval.rpoint[2]);
						_arg.vt = VT_DATE;
						_arg.date = Date;
						break;
					}
				case VT_UI4:
					{
						_arg.vt = VT_UI4;
						_arg.ulVal = RGB(pArgs->resval.rpoint[0], pArgs->resval.rpoint[1], pArgs->resval.rpoint[2]);
						break;
					}
				}
				break;
			}
		default:
			{
				return false; 
			}		
	}
	bool bSuccess = ((varArg.vt & VT_TYPEMASK) == VT_VARIANT ||
									 (varArg.vt & VT_TYPEMASK) == VT_EMPTY ||
									 S_OK == VariantChangeType( &_arg, &_arg, 0, (varArg.vt & VT_TYPEMASK) ));
	if( !bSuccess )
		return false;

	if( varArg.vt & VT_BYREF )
	{
		switch( varArg.vt & VT_TYPEMASK )
		{
			case VT_UI1: if( varArg.pbVal ) *varArg.pbVal = _arg.bVal; break;
			case VT_I2: case VT_UI2: if( varArg.piVal ) *varArg.piVal = _arg.iVal; break;
			case VT_I4: case VT_UI4: if( varArg.plVal ) *varArg.plVal = _arg.lVal; break;
			case VT_R4: if( varArg.pfltVal ) *varArg.pfltVal = _arg.fltVal; break;
			case VT_R8: if( varArg.pdblVal ) *varArg.pdblVal = _arg.dblVal; break;
			case VT_CY: if( varArg.pcyVal ) varArg.pcyVal->int64 = _arg.cyVal.int64; break;
			case VT_DATE: if( varArg.pdate ) *varArg.pdate = _arg.date; break;
			case VT_BSTR: if( varArg.pbstrVal ) *varArg.pbstrVal = _arg.bstrVal; _arg.Detach(); break;
		#if !defined(_UNICODE) && !defined(OLE2ANSI)
			case VT_BSTRA: if( varArg.pbstrVal ) *varArg.pbstrVal = _arg.bstrVal; _arg.Detach(); break;
		#endif
			case VT_BOOL: if( varArg.pboolVal ) *varArg.pboolVal = _arg.boolVal; break;
			case VT_ERROR: if( varArg.pscode ) *varArg.pscode = _arg.scode; break;
			case VT_UNKNOWN: if( varArg.ppunkVal ) *varArg.ppunkVal = _arg.punkVal; _arg.Detach(); break;
			case VT_DISPATCH: if( varArg.ppdispVal ) *varArg.ppdispVal = _arg.pdispVal; _arg.Detach(); break;
			case VT_VARIANT: if( varArg.pvarVal ) VariantCopy( varArg.pvarVal, &_arg ); break;
			default: return false;
		}
	}
	else
	{
		if( S_OK != VariantCopy( &varArg, &_arg ) )
			return false;
	}

	pArgs = pArgs->rbnext;
	return true; 
}


resbuf* VariantArgToResbuf( const VARIANTARG& varArg, const GUID& guidType, resbuf*& prbTail )
{
	const VARIANT& var = (varArg.vt == (VT_VARIANT | VT_BYREF)? *varArg.pvarVal : varArg);
	switch( var.vt )
	{
	case VT_UI1: return (prbTail = acutNewRb( (var.bVal == 1)? RTT : RTNIL ));
	case (VT_UI1 | VT_BYREF): return (prbTail = acutNewRb( (*var.pbVal == 1)? RTT : RTNIL ));
	case VT_I2: return (prbTail = LongToRB( var.iVal ));
	case (VT_I2 | VT_BYREF): return (prbTail = LongToRB( *var.piVal ));
	case VT_UI4:
	case VT_I4:
		{
			if( guidType == GUID_COLOR )
			{
				COLORREF clr = var.lVal;
				int nRed = GetRValue(clr);
				int nGreen = GetGValue(clr);
				int nBlue = GetBValue(clr);
				struct resbuf* prbColor = acutBuildList(
					RTSHORT, nRed,
					RTSHORT, nGreen,
					RTSHORT, nBlue,
					RTNONE);
				if( prbColor )
					prbTail = prbColor->rbnext->rbnext;
				return prbColor;
			}
			else
				return (prbTail = LongToRB( var.lVal ));
		}
	case (VT_UI4 | VT_BYREF):
	case (VT_I4 | VT_BYREF):
		{
			if( guidType == GUID_COLOR )
			{
				COLORREF clr = *var.plVal;
				int nRed = GetRValue(clr);
				int nGreen = GetGValue(clr);
				int nBlue = GetBValue(clr);
				struct resbuf* prbColor = acutBuildList(
					RTSHORT, nRed,
					RTSHORT, nGreen,
					RTSHORT, nBlue,
					RTNONE);
				if( prbColor )
					prbTail = prbColor->rbnext->rbnext;
				return prbColor;
			}
			else
				return (prbTail = LongToRB( *var.plVal ));
		}
	case VT_R4: return (prbTail = DoubleToRB( var.fltVal ));
	case (VT_R4 | VT_BYREF): return (prbTail = DoubleToRB( *var.pfltVal ));
	case VT_R8: return (prbTail = LongToRB( var.dblVal ));
	case (VT_R8 | VT_BYREF): return (prbTail = LongToRB( *var.pdblVal ));
	case VT_CY: return (prbTail = LongLongToRB( var.cyVal.int64 ));
	case (VT_CY | VT_BYREF): return (prbTail = LongLongToRB( var.pcyVal->int64 ));
	case VT_DATE: return (prbTail = DateToRB( var.date ));
	case (VT_DATE | VT_BYREF): return (prbTail = DateToRB( *var.pdate ));
	case VT_BSTR: return (prbTail = StringToRB( var.bstrVal ));
	case (VT_BSTR | VT_BYREF): return (prbTail = StringToRB( *var.pbstrVal ));
#if !defined(_UNICODE) && !defined(OLE2ANSI)
	case VT_BSTRA: return (prbTail = StringToRB( var.bstrVal ));
	case (VT_BSTRA | VT_BYREF): return (prbTail = StringToRB( *var.pbstrVal ));
#endif
	case VT_BOOL: return (prbTail = acutNewRb( (var.boolVal == VARIANT_TRUE)? RTT : RTNIL ));
	case (VT_BOOL | VT_BYREF): return (prbTail = acutNewRb( (*var.pboolVal == VARIANT_TRUE)? RTT : RTNIL ));
	case VT_ERROR: return (prbTail = LongToRB( var.scode ));
	case (VT_ERROR | VT_BYREF): return (prbTail = LongToRB( *var.pscode ));
	case VT_UNKNOWN: return (prbTail = IUnknownToRB( var.punkVal ));
	case (VT_UNKNOWN | VT_BYREF): return (prbTail = IUnknownToRB( *var.ppunkVal ));
	case VT_DISPATCH: return (prbTail = IDispatchToRB( var.pdispVal ));
	case (VT_DISPATCH | VT_BYREF): return (prbTail = IDispatchToRB( *var.ppdispVal ));
	}	
	return (prbTail = acutNewRb( RTNIL ));
}


static CString VariantArgToString( const VARIANTARG& varArg )
{
	const VARIANT& var = (varArg.vt == (VT_VARIANT | VT_BYREF)? *varArg.pvarVal : varArg);
	switch( var.vt )
	{
	case VT_UI1: return ((var.bVal == 1)? _T("T") : _T("NIL"));
	case (VT_UI1 | VT_BYREF): return ((*var.pbVal == 1)? _T("T") : _T("NIL"));
	case VT_I2: return LongToS( var.iVal );
	case (VT_I2 | VT_BYREF): return LongToS( *var.piVal );
	case VT_I4: return LongToS( var.lVal );
	case (VT_I4 | VT_BYREF): return LongToS( *var.plVal );
	case VT_R4: return DoubleToS( var.fltVal );
	case (VT_R4 | VT_BYREF): return DoubleToS( *var.pfltVal );
	case VT_R8: return LongToS( var.dblVal );
	case (VT_R8 | VT_BYREF): return LongToS( *var.pdblVal );
	case VT_CY: return LongLongToS( var.cyVal.int64 );
	case (VT_CY | VT_BYREF): return LongLongToS( var.pcyVal->int64 );
	case VT_DATE: return DateToS( var.date );
	case (VT_DATE | VT_BYREF): return DateToS( *var.pdate );
	case VT_BSTR: return var.bstrVal;
	case (VT_BSTR | VT_BYREF): return *var.pbstrVal;
#if !defined(_UNICODE) && !defined(OLE2ANSI)
	case VT_BSTRA: return var.bstrVal;
	case (VT_BSTRA | VT_BYREF): return *var.pbstrVal;
#endif
	case VT_BOOL: return ((var.boolVal == VARIANT_TRUE)? _T("T") : _T("NIL"));
	case (VT_BOOL | VT_BYREF): return ((*var.pboolVal == VARIANT_TRUE)? _T("T") : _T("NIL"));
	case VT_ERROR: return LongToS( var.scode );
	case (VT_ERROR | VT_BYREF): return LongToS( *var.pscode );
	}	
	return _T("NIL");
}


template<> class arg_t< DISPPARAMS* > : arg_b
{
	const DISPPARAMS* _arg;
public:
	typedef DISPPARAMS* type;
	arg_t( DISPPARAMS* arg ) : _arg( arg ) {}
	virtual resbuf* asResbuf() const
		{
			if( !_arg || _arg->cArgs == 0 )
				return NULL;
			resbuf* prbHead = NULL;
			resbuf* prbTail = NULL;
			UINT idx = _arg->cArgs;
			while( idx > 0 )
			{
				resbuf* prbArgTail = NULL;
				resbuf* prbArg = VariantArgToResbuf( _arg->rgvarg[--idx], GUID_NULL, prbArgTail );
				if( !prbHead )
					prbHead = prbArg;
				else
					prbTail->rbnext = prbArg;
				prbTail = prbArgTail;
			}
			return prbHead;
		}
	virtual CString asString() const
		{
			if( !_arg || _arg->cArgs == 0 )
				return _T("");
			CString sArg;
			UINT idx = _arg->cArgs;
			while( idx > 0 )
			{
				sArg += _T(' ');
				sArg += VariantArgToString( _arg->rgvarg[--idx] );
			}
			return sArg;
		}
};
typedef args_1< DISPPARAMS* > args_X;


CArxAxContainerCtrl::CArxAxContainerCtrl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate) 
: CAxContainerCtrl(pTemplate, pPane, nID, false)
, mArxServices( this )
{
	TraceFmt( _T("> CArxAxContainerCtrl::CArxAxContainerCtrl(%s [%p], [%p], %s [HWND: %p]) [this: %p]\r\n"),
		(LPCTSTR)pTemplate->GetKeyPath(), &*pTemplate, pPane, (LPCTSTR)CString(mpTemplate->GetRuntimeClass()->m_lpszClassName),
		mpTemplate->GetWindow(), this );

	if( bCreate ) 
		Create( pPane->GetHostDialog(), nID );
}

CArxAxContainerCtrl::~CArxAxContainerCtrl()
{
	TraceFmt( _T("< CArxDialogControl::~CArxDialogControl() [this: %p]\r\n"), this );
}


bool CArxAxContainerCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, nID );

	return bSuccess;
}


BOOL CArxAxContainerCtrl::OnCmdMsg( UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo ) 
{
	if( nCode == CN_EVENT && nID == GetTemplate()->GetControlInstance()->GetControlId() )
		HandleAxEvent( (AFX_EVENT*)pExtra );

	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CArxAxContainerCtrl::HandleAxEvent( AFX_EVENT* pEvent )
{
	if( !pEvent || pEvent->m_eventKind != AFX_EVENT::event )
		return;
	const TPropertyList& Props = GetTemplate()->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		const CPropertyObject* pProp = (*iter);
		if( !pProp )
			continue;
		const AxInterfaceDescriptor* pID = pProp->GetConstAxInterfaceDescriptorPtr();
		if( !pID )
			continue;
		const AxEventDescriptor* pED = pID->GetEvent();
		if( !pED )
			continue;
		if( pED->GetDispId() == pEvent->m_dispid && !pProp->GetStringValue().IsEmpty() )
		{
			bool bByrefArgs = false;
			DISPPARAMS*& pDispParams = pEvent->m_pDispParams;
			VARIANTARG* rVarArgs = pDispParams->rgvarg;
			UINT idx = pDispParams->cArgs;
			while( idx > 0 )
			{
				if( rVarArgs[--idx].vt & VT_BYREF )
				{
					bByrefArgs = true;
					break;
				}
			}
			if( bByrefArgs )
			{ //utilize returned list to modify byref args
				resbuf* prbResult = NULL;
				GetArxServices()->HandleEvent( pProp->GetStringValue(), prbResult, args_X( pDispParams ) );
				if( prbResult )
				{
					resbuf* pArgs = prbResult;
					UINT idx = pDispParams->cArgs;
					while( pArgs && idx > 0 )
					{
						VARIANTARG& varArg = rVarArgs[--idx];
						if( varArg.vt & VT_BYREF )
							ResbufToVariantArg( pArgs, varArg );
					}
					acutRelRb( prbResult );
				}
			}
			else
				GetArxServices()->HandleEvent( pProp->GetStringValue(), args_X( pDispParams ) );
		}
	}
}
