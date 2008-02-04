#include "stdafx.h"
#include "FontSettings.h"
#include "DclControlObject.h"
#include "AxContainerCtrl.h"
#include "AxInterfaceDescriptor.h"
#include "OLEFont.h"


FontSettings::FontSettings()
: mlSize( 0 )
, mbBold( false )
, mbUnderlined( false )
, mbItalic( false )
{}

FontSettings::FontSettings( LPCTSTR pszName, long lSize, bool bBold, bool bUnderlined, bool bItalic )
: mlSize( 0 )
, mbBold( false )
, mbUnderlined( false )
, mbItalic( false )
{
	set( pszName, lSize, bBold, bUnderlined, bItalic );
}

FontSettings::FontSettings( LPCTSTR pszName, bool bScaled, UINT nSize, bool bBold, bool bUnderlined, bool bItalic )
: mlSize( 0 )
, mbBold( false )
, mbUnderlined( false )
, mbItalic( false )
{
	set( pszName, bScaled, nSize, bBold, bUnderlined, bItalic );
}

FontSettings::FontSettings( TDclControlPtr pDclControl )
: mlSize( 0 )
, mbBold( false )
, mbUnderlined( false )
, mbItalic( false )
{
	setFrom( pDclControl );
}

FontSettings::FontSettings( CAxContainerCtrl* pAxControl, DISPID dispid )
: mlSize( 0 )
, mbBold( false )
, mbUnderlined( false )
, mbItalic( false )
{
	setFrom( pAxControl, dispid );
}

void FontSettings::set( LPCTSTR pszName, long lSize, bool bBold,
												bool bUnderlined, bool bItalic )
{
	msName = pszName;
	mlSize = lSize;
	mbBold = bBold;
	mbUnderlined = bUnderlined;
	mbItalic = bItalic;
}

void FontSettings::set( LPCTSTR pszName, bool bScaled, UINT nSize, bool bBold,
												bool bUnderlined, bool bItalic )
{
	msName = pszName;
	mlSize = (bScaled? (long)nSize : -(long)nSize);
	mbBold = bBold;
	mbUnderlined = bUnderlined;
	mbItalic = bItalic;
}

void FontSettings::setFrom( TDclControlPtr pDclControl )
{
	if( !pDclControl )
	{
		clear();
		return;
	}
	msName = pDclControl->GetStringProperty( Prop::FontName );
	mlSize = pDclControl->GetLongProperty( Prop::FontSize );
	mbBold = pDclControl->GetBooleanProperty( Prop::FontBold );
	mbUnderlined = pDclControl->GetBooleanProperty( Prop::FontUnderline );
	mbItalic = pDclControl->GetBooleanProperty( Prop::FontItalic );
	if( pDclControl->GetType() == CtlActiveX )
	{
		TPropertyPtr pFontProp = NULL;
		const TPropertyList& Props = pDclControl->GetPropertyList();
		for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
		{
			if ((*iter)->GetType() == PropActiveXProp)
			{
				if ((*iter)->GetConstAxInterfaceDescriptorPtr()->GetGuid() == IID_IFontDisp ||
						(*iter)->GetConstAxInterfaceDescriptorPtr()->GetGuid() == IID_IFont)
				{
					pFontProp = (*iter);
					break;
				}
			}
		}
		if( pFontProp ) 
		{
			CDialogControl* pDlgControl = pDclControl->GetControlInstance();
			if( pDlgControl )
				setFrom( pDlgControl->GetActiveXCtrl(),
								 pFontProp->GetConstAxInterfaceDescriptorPtr()->GetGetDispId() );
		}
	}
}

void FontSettings::setFrom( CAxContainerCtrl* pAxControl, DISPID dispid )
{
	if( !pAxControl )
	{
		clear();
		return;
	}
	COleFont font = pAxControl->GetFont( dispid );
	msName = font.GetName();
	CY cyFontSize = font.GetSize();
	mlSize = cyFontSize.Lo / -10000;		
	mbBold = (font.GetBold() == TRUE);
	mbUnderlined = (font.GetUnderline() == TRUE);
	mbItalic = (font.GetItalic() == TRUE);
}

CString FontSettings::absoluteSize() const
{
	CString sSize;
	sSize.Format( _T("%d"), (mlSize < 0? -mlSize : mlSize) );
	return sSize;
}
