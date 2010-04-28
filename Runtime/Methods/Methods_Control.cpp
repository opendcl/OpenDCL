// Methods_Control.cpp : implementation file
//

#include "stdafx.h"
#include "ArgumentsRetrieval.h"
#include "Methods_Control.h"
#include "PropertyNames.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlTypes.h"
#include "ArxWorkspace.h"
#include "Resource.h"
#include "ArxDialogControl.h"
#include "ImageListObject.h"


static void ReturnPropertyValue( TPropertyPtr pProperty )
{
	switch( pProperty->GetType() )
	{
	case PropBool:
		if( pProperty->GetBooleanValue() )
			acedRetT();
		else
			acedRetNil();
		break;
	case PropLong:
		theArxWorkspace.RetLong( pProperty->GetLongValue() );
		break;
	case PropEnum:
	case PropPicture:
		acedRetInt( pProperty->GetLongValue() );
		break;
	case PropString:
	case PropEvent:
	case PropActiveXEvent:
		acedRetStr( pProperty->GetStringValue() );
		break;
	case PropDouble:
		acedRetReal( pProperty->GetDoubleValue() );
		break;
	case PropImageList:
		{
			TImageListPtr pImageList;
			TDclControlPtr pDclControl = pProperty->GetOwnerControl();
			if( pDclControl )
				pImageList = pDclControl->GetImageList();
			if( pImageList )
			{
				IUnknown* pUnknown = new TImageListPtrIUnknown( pImageList );
				theArxWorkspace.AddUnknown( pUnknown );
				theArxWorkspace.RetPointer( pUnknown, odcl::ptrImageList );
			}
			else
				acedRetNil();
		}
		break;
	case PropOLEColor:
		theArxWorkspace.RetLong( (LONG)pProperty->GetOLEColorValue() );
		break;
	case PropIntArray:
		{
			const PropVal::TIntArray* prInt = pProperty->GetConstIntArrayPtr();
			if( prInt && !prInt->empty() )
			{
				resbuf* prbHead = NULL;
				resbuf* prbTail = NULL;
				for( PropVal::TIntArray::const_iterator iter = prInt->begin();
						 iter != prInt->end();
						 ++iter )
				{
					resbuf* prbNew = acutNewRb( RTSHORT );
					prbNew->rbnext = NULL;
					prbNew->resval.rint = *iter;
					if( !prbHead )
						prbHead = prbNew;
					else
						prbTail->rbnext = prbNew;
					prbTail = prbNew;
				}
				acedRetList( prbHead );
				acutRelRb( prbHead );
			}
			else
				acedRetNil();
		}
		break;
	case PropStringArray:
		{
			const PropVal::TCStringArray* prStr = pProperty->GetConstStringArrayPtr();
			if( prStr && !prStr->empty() )
			{
				resbuf* prbHead = NULL;
				resbuf* prbTail = NULL;
				for( PropVal::TCStringArray::const_iterator iter = prStr->begin();
						 iter != prStr->end();
						 ++iter )
				{
					resbuf* prbNew = acutNewRb( RTSTR );
					prbNew->rbnext = NULL;
					acutNewString( *iter, prbNew->resval.rstring );
					if( !prbHead )
						prbHead = prbNew;
					else
						prbTail->rbnext = prbNew;
					prbTail = prbNew;
				}
				acedRetList( prbHead );
				acutRelRb( prbHead );
			}
			else
				acedRetNil();
		}
		break;
	case PropStringArrayList:
		{
			const PropVal::TCStringArrayList* prrStr = pProperty->GetConstStringArrayListPtr();
			if( prrStr && !prrStr->empty() )
			{
				resbuf* prbHead = NULL;
				resbuf* prbTail = NULL;
				for( PropVal::TCStringArrayList::const_iterator iter = prrStr->begin();
						 iter != prrStr->end();
						 ++iter )
				{
					resbuf* prbNew = acutNewRb( RTLB );
					prbNew->rbnext = NULL;
					if( !prbHead )
						prbHead = prbNew;
					else
						prbTail->rbnext = prbNew;
					prbTail = prbNew;
					const PropVal::TCStringArray& rStr = *iter;
					for( PropVal::TCStringArray::const_iterator iter = rStr.begin();
							 iter != rStr.end();
							 ++iter )
					{
						prbTail->rbnext = acutNewRb( RTSTR );
						prbTail = prbNew;
						prbTail->rbnext = NULL;
						acutNewString( *iter, prbTail->resval.rstring );
					}
					prbTail->rbnext = acutNewRb( RTLE );
					prbTail = prbTail->rbnext;
					prbTail->rbnext = NULL;
				}
				acedRetList( prbHead );
				acutRelRb( prbHead );
			}
			else
				acedRetNil();
		}
		break;
	case PropIntArrayList:
		{
			const PropVal::TIntArrayList* prrInt = pProperty->GetConstIntArrayListPtr();
			if( prrInt && !prrInt->empty() )
			{
				resbuf* prbHead = NULL;
				resbuf* prbTail = NULL;
				for( PropVal::TIntArrayList::const_iterator iter = prrInt->begin();
						 iter != prrInt->end();
						 ++iter )
				{
					resbuf* prbNew = acutNewRb( RTLB );
					prbNew->rbnext = NULL;
					if( !prbHead )
						prbHead = prbNew;
					else
						prbTail->rbnext = prbNew;
					prbTail = prbNew;
					const PropVal::TIntArray& rInt = *iter;
					for( PropVal::TIntArray::const_iterator iter = rInt.begin();
							 iter != rInt.end();
							 ++iter )
					{
						prbTail->rbnext = acutNewRb( RTSHORT );
						prbTail = prbNew;
						prbTail->rbnext = NULL;
						prbTail->resval.rint = *iter;
					}
					prbTail->rbnext = acutNewRb( RTLE );
					prbTail = prbTail->rbnext;
					prbTail->rbnext = NULL;
				}
				acedRetList( prbHead );
				acutRelRb( prbHead );
			}
			else
				acedRetNil();
		}
		break;
	}	
}

bool SetCtrlProperty( Prop::Id id )
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return false; //invalid input

	TPropertyPtr pProperty = pControl->GetPropertyObject( id );
	if( !pProperty )
	{
		HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, GetPropertyApiName( id ) );
		return false;
	}

	if( !GetPropertyArgument( pArgs, pProperty ) )
		return false; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return false;

	CDialogControl* pDlgControl = pControl->GetControlInstance();
	if( pDlgControl )
		pDlgControl->ApplyProperty( pProperty );

	acedRetT();
	return true;
}

bool GetCtrlProperty( Prop::Id id )
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return false; //invalid input

	TPropertyPtr pProperty = pControl->GetPropertyObject( id );
	if( !pProperty )
	{
		HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, GetPropertyApiName( id ) );
		return false;
	}

	ReturnPropertyValue( pProperty );
	return true;
}

ADSRESULT Control::SetProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	CDialogControl* pDlgControl = pControl->GetControlInstance();

	TPropertyPtr pProperty;
	CString sPropName;
	if( GetStringArgument( pArgs, sPropName, true ) )
	{
		pProperty = pControl->GetPropertyObject( GetPropertyId( sPropName ) );
		if( !pProperty )
			return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

		if( !GetPropertyArgument( pArgs, pProperty ) )
			return RSERR; //invalid input

		if( pDlgControl )
			pDlgControl->ApplyProperty( pProperty );
	}
	else if( GetNilArgument( pArgs, true ) )
	{
		acedRetT();
		return RSRSLT;
	}
	else
	{
		if( !GetListBeginArgument( pArgs ) )
			return RSERR; //invalid input

		if( GetListEndArgument( pArgs, true ) )
			return RSRSLT;

		do
		{
			CString sPropName;
			if( !GetStringArgument( pArgs, sPropName ) )
			return RSERR; //invalid input

			pProperty = pControl->FindPropertyObject( sPropName );
			if( !pProperty )
				return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

			if( !GetPropertyArgument( pArgs, pProperty ) )
				return RSERR; //invalid input

			if( pDlgControl )
				pDlgControl->ApplyProperty( pProperty );

			if( !GetListEndArgument( pArgs ) )
				return RSERR; //invalid input
			
		} while( GetListBeginArgument( pArgs, true ) );
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT Control::GetProperties()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pDclControl = NULL;
	if( !GetControlArgument( pArgs, pDclControl ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pDclControl )
		return RSRSLT; //form not found

	resbuf* prbProperties = NULL;
	resbuf* prbTail = NULL;
	const TPropertyList& Props = pDclControl->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		resbuf* prbProp = acutNewRb( RTSTR );
		acutNewString( (*iter)->GetApiName(), prbProp->resval.rstring );
		if( prbTail )
			prbTail->rbnext = prbProp;
		else
			prbProperties = prbProp;
		prbTail = prbProp;
	}
	acedRetList( prbProperties );
	acutRelRb( prbProperties );

	return (RSRSLT) ;
}

ADSRESULT Control::GetProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	CString sPropName;
	if( !GetStringArgument( pArgs, sPropName ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	TPropertyPtr pProperty = pControl->FindPropertyObject( sPropName );
	if( !pProperty )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

	ReturnPropertyValue( pProperty );
	return RSRSLT;
}

ADSRESULT Control::GetHWND()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl;
	if( !GetDlgControlArgument( pArgs, pDlgControl, _CtlInvalid ) )
		return RSERR; //invalid argument

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	theArxWorkspace.RetHandle( (DWORD_PTR)pDlgControl->GetHWnd() );

	return (RSRSLT) ;
}

ADSRESULT Control::SetFocus()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl;
	if( !GetDlgControlArgument( pArgs, pDlgControl, _CtlInvalid ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CWnd* pCtrl = pDlgControl->GetControlWnd();
	if( !pCtrl || !pCtrl->IsWindowVisible() )		
		return RSRSLT;

	pCtrl->SetFocus();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Control::ZOrder()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl;
	if( !GetDlgControlArgument( pArgs, pDlgControl, _CtlInvalid ) )
		return RSERR; //invalid input

	int nDirection;
	if( !GetIntArgument( pArgs, nDirection ) )
		return RSERR; //invalid input

	if( nDirection < 0 || nDirection > 1 )
		return HandleArgValueError( pArgs );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CWnd* pCtrl = pDlgControl->GetControlWnd();
	if( !pCtrl )		
		return RSRSLT;

	if( pCtrl->SetWindowPos( (nDirection? &CWnd::wndTop : &CWnd::wndBottom),
													 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Control::GetPos()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl;
	if( !GetDlgControlArgument( pArgs, pDlgControl, _CtlInvalid ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CRect rcControl = pDlgControl->GetEffectiveWindowRect();

	resbuf rbHeight = { NULL, RTSHORT };
	rbHeight.resval.rint = rcControl.Height();
	resbuf rbWidth = { &rbHeight, RTSHORT };
	rbWidth.resval.rint = rcControl.Width();
	resbuf rbTop = { &rbWidth, RTSHORT };
	rbTop.resval.rint = rcControl.top;
	resbuf rbLeft = { &rbTop, RTSHORT };
	rbLeft.resval.rint = rcControl.left;
	acedRetList( &rbLeft );
	return RSRSLT;
}

ADSRESULT Control::SetPos()
{	
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl;
	if( !GetDlgControlArgument( pArgs, pDlgControl, _CtlInvalid ) )
		return RSERR; //invalid input

	long lLeft = -1;
	long lTop = -1;
	long lWidth = -1;
	long lHeight = -1;

	if( !GetLongArgument( pArgs, lLeft ) )
		return RSERR; //invalid input

	if( !GetLongArgument( pArgs, lTop ) )
		return RSERR; //invalid input

	GetLongArgument( pArgs, lWidth, true );

	GetLongArgument( pArgs, lHeight, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( pDlgControl->GetControlType() == _CtlForm )
	{
		CDialogObject* pDlg = (CDialogObject*)pDlgControl;
		if( !pDlg->MoveDialog( lLeft, lTop ) )
			return RSRSLT;
		if( lWidth >= 0 || lHeight >= 0 )
		{
			CRect rcDlg = pDlg->GetEffectiveWindowRect();
			if( lWidth < 0 )
				lWidth = rcDlg.Width();
			if( lHeight < 0 )
				lHeight = rcDlg.Height();
			pDlg->GetTopLevelWnd()->SetWindowPos( NULL, 0, 0, lWidth, lHeight,
																						SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
		}
	}
	else
	{
		TDclControlPtr pDclControl = pDlgControl->GetTemplate();
		pDclControl->SetLongProperty( Prop::Left, lLeft );
		pDclControl->SetLongProperty( Prop::Top, lTop );
		if( lWidth >= 0 )
			pDclControl->SetLongProperty( Prop::Width, lWidth );
		if( lHeight >= 0 )
			pDclControl->SetLongProperty( Prop::Height, lHeight );
	}
	pDlgControl->ApplyPosition();
	pDlgControl->OnNeedRepaint();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Control::Redraw()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl;
	if( !GetDlgControlArgument( pArgs, pDlgControl, _CtlInvalid ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	pDlgControl->OnNeedRepaint( true, true );
	acedRetT();
	return RSRSLT;
}

ADSRESULT Control::GetName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetStr( pControl->GetStringProperty( Prop::Name ) );
	return RSRSLT;
}

ADSRESULT Control::ShowToolTip()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl;
	if( !GetDlgControlArgument( pArgs, pDlgControl, _CtlInvalid ) )
		return RSERR; //invalid input

	int nX = -1;
	int nY = -1;
	if( GetIntArgument( pArgs, nX, true ) )
		GetIntArgument( pArgs, nY, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CWnd* pCtrlWnd = pDlgControl->GetControlWnd();
	CPoint pt( nX, nY );
	if( pt.x == -1 && pt.y == -1 )
	{
		CRect rcCtrl;
		pCtrlWnd->GetWindowRect( &rcCtrl );
		pt = rcCtrl.CenterPoint();
	}

	PPTOOLTIP_INFO TI;
	if( pDlgControl->GetToolTipCtrl().GetToolInfo( TI, pCtrlWnd, DWORD(0) ) )
		pDlgControl->GetToolTipCtrl().ShowHelpTooltip( &pt, TI );

	acedRetT();
	return RSRSLT;
}
