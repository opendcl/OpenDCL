// Methods_Properties.cpp : implementation file
//

#include "stdafx.h"
#include "ArgumentsRetrieval.h"
#include "Methods_Properties.h"
#include "PropertyNames.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "Resource.h"
#include "ArxDialogControl.h"


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
	case PropEnum:
	case PropPicture:
		acedRetInt( pProperty->GetLongValue() );
		break;
	case PropString:
	case PropEvent:
		acedRetStr( pProperty->GetStringValue() );
		break;
	case PropDouble:
		acedRetReal( pProperty->GetDoubleValue() );
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
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, GetPropertyApiName( id ) );

	if( !GetPropertyArgument( pArgs, pProperty ) )
		return false; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return false;

	CArxDialogControl::UpdateProperty( pControl, id );

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
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, GetPropertyApiName( id ) );

	ReturnPropertyValue( pProperty );
	return true;
}

ADSRESULT Control::SetProperty()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	CString sPropName;
	if( GetStringArgument( pArgs, sPropName, true ) )
	{
		TPropertyPtr pProperty = pControl->GetPropertyObject( GetPropertyId( sPropName ) );
		if( !pProperty )
			return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

		if( !GetPropertyArgument( pArgs, pProperty ) )
			return RSERR; //invalid input
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

			TPropertyPtr pProperty = pControl->GetPropertyObject( GetPropertyId( sPropName ) );
			if( !pProperty )
				return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

			if( !GetPropertyArgument( pArgs, pProperty ) )
				return RSERR; //invalid input

			if( !GetListEndArgument( pArgs ) )
				return RSERR; //invalid input
			
		} while( GetListBeginArgument( pArgs, true ) );
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxDialogControl::UpdateAllProperties( pControl );
	acedRetT();
	return RSRSLT;
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

	TPropertyPtr pProperty = pControl->GetPropertyObject( GetPropertyId( sPropName ) );
	if( !pProperty )
		return HandleArgValueError( pArgs, IDS_ERR_NOTAPROPERTY, (LPCTSTR)sPropName );

	ReturnPropertyValue( pProperty );
	return RSRSLT;
}

ADSRESULT Control::SetFocus()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CWnd* pCtrl = pControl->GetWindow();
	if( !pCtrl || !pCtrl->IsWindowVisible() )		
		return RSRSLT;

	pCtrl->SetFocus();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Control::ZOrder()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	int nDirection;
	if( !GetIntArgument( pArgs, nDirection ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CWnd* pCtrl = pControl->GetWindow();
	if( !pCtrl || !pCtrl->IsWindowVisible() )		
		return RSRSLT;

	if( pCtrl->SetWindowPos( (nDirection? &CWnd::wndTop : &CWnd::wndBottom),
													 0, 0, -1, -1, SWP_NOSIZE | SWP_NOMOVE ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Control::GetCurPos()
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

	CRect rcControl = pDlgControl->GetEffectiveWindowRect();
	long lWidth = rcControl.Width();
	long lHeight = rcControl.Height();

	if( !GetLongArgument( pArgs, rcControl.left ) )
		return RSERR; //invalid input

	if( !GetLongArgument( pArgs, rcControl.top ) )
		return RSERR; //invalid input

	GetLongArgument( pArgs, lWidth, true );

	GetLongArgument( pArgs, lHeight, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	TDclControlPtr pDclControl = pDlgControl->GetTemplate();
	TPropertyPtr pPropLeft = pDclControl->GetPropertyObject( Prop::Left );
	if( pPropLeft )
	{
		pPropLeft->SetLongValue( rcControl.left );
		pDclControl->SetLongProperty( Prop::Top, rcControl.top );
		pDclControl->SetLongProperty( Prop::Width, lWidth );
		pDclControl->SetLongProperty( Prop::Height, lHeight );
		pDlgControl->ApplyPosition();
	}
	else
	{
		rcControl.right = rcControl.left + lWidth;
		rcControl.bottom = rcControl.top + lHeight;
		pDlgControl->GetControlWnd()->MoveWindow( &rcControl );
	}
	pDlgControl->OnNeedRepaint();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Control::ForceUpdateNow()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( pControl->GetWindow()->RedrawWindow() )
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

	TDclControlPtr pControl;
	if( !GetControlArgument( pArgs, pControl ) )
		return RSERR; //invalid input

	int nX = -1;
	int nY = -1;
	if( GetIntArgument( pArgs, nX, true ) )
		GetIntArgument( pArgs, nY, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CWnd* pCtrlWnd = pControl->GetWindow();
	CPoint pt( nX, nY );
	if( pt.x == -1 && pt.y == -1 )
	{
		CRect rcCtrl;
		pCtrlWnd->GetWindowRect( &rcCtrl );
		pt = rcCtrl.CenterPoint();
	}

	PPTOOLTIP_INFO TI;
	if( pControl->GetControlInstance()->GetToolTipCtrl().GetToolInfo( TI, pCtrlWnd, DWORD(0) ) )
		pControl->GetControlInstance()->GetToolTipCtrl().ShowHelpTooltip(&pt, TI);

	acedRetT();
	return RSRSLT;
}

ADSRESULT ProgressBar::SetPos()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlProgress))
		return RSERR; //invalid input

	int nPos;
	if( !GetIntArgument( pArgs, nPos ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CProgressCtrl* pCtrl = (CProgressCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetPos( nPos );
	pCtrl->RedrawWindow();
	acedRetT();
	return RSRSLT;
}


