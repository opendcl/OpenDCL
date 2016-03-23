// DclControlTemplate.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlTemplate.h"
#include "DclFormTemplate.h"
#include "PropertyObject.h"
#include "DclImageList.h"
#include "AxContainerCtrl.h"
#include "AxInterfaceDescriptor.h"
#include "ImageListObject.h"
#include "Filing.h"
#include "Workspace.h"
#include "Project.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "DialogControl.h"
#include "FontSettings.h"
#include "PropertyNames.h"
#include "AcadColorTable.h"
#include "DclControlProp.h"


/////////////////////////////////////////////////////////////////////////////
// CDclControlTemplate

CDclControlTemplate::CDclControlTemplate()
: mpOwner( NULL )
, mType( _CtlForm )
, mpDlgControl( NULL )
, mnID( -1 )
{
}

CDclControlTemplate::CDclControlTemplate(CDclFormObject* pOwner)
: mpOwner( pOwner )
, mType( _CtlForm )
, mpDlgControl( NULL )
, mnID( -1 )
{
}

CDclControlTemplate::CDclControlTemplate(ControlType type, CDclFormObject* pOwner, LPCTSTR pszName /*= NULL*/)
: mpOwner( pOwner )
, mType( type )
, mpDlgControl( NULL )
, mnID( -1 )
{
	if( pszName )
		AddStringProperty(Prop::Name, PropString, pszName);
}

CDclControlTemplate::~CDclControlTemplate()
{
}

bool CDclControlTemplate::IsTabOrderAllowed() const
{
	if( mType == _CtlForm || mType == CtlFileExplorer )
		return false;
	return true;
}

CUndoManager* CDclControlTemplate::GetUndoManager() const
{
	if( !mpOwner )
		return NULL;
	return mpOwner->GetUndoManager();
}

CWnd* CDclControlTemplate::GetWindow() const
{
	if( !mpDlgControl )
		return NULL;
	return mpDlgControl->GetControlWnd();
}

TProjectPtr CDclControlTemplate::GetOwnerProject()
{
	if( !mpOwner )
		return NULL;
	return mpOwner->GetProject();
}

const TProjectPtr CDclControlTemplate::GetOwnerProject() const
{
	if( !mpOwner )
		return NULL;
	return mpOwner->GetProject();
}

TDclFormPtr CDclControlTemplate::GetOwnerForm()
{
	if( !mpOwner )
		return NULL;
	TProjectPtr pProject = mpOwner->GetProject();
	if( !pProject )
		return TDclFormLockedPtr( mpOwner );
	return pProject->GetRefCountedPtr( mpOwner );
}

const TDclFormPtr CDclControlTemplate::GetOwnerForm() const
{
	if( !mpOwner )
		return NULL;
	const TProjectPtr pProject = mpOwner->GetProject();
	if( !pProject )
		return TDclFormLockedPtr( mpOwner );
	return pProject->GetRefCountedPtr( mpOwner );
}

void CDclControlTemplate::SetControlInstance( CDialogControl* pDlgControl )
{
	//note: an assertion here indicates more than 1 instance of the control; perhaps
	//an earlier instance didn't get destroyed when it should have been?
	assert( mpDlgControl == NULL || pDlgControl == NULL );
	mpDlgControl = pDlgControl;
}


//IOStatus CDclControlTemplate::WriteToTextFile(FILE* pFile, const CString &fileName) const
//{
//  fprintf(pFile, "\nCArxControlObject");
//
//  int nCount;
//  BOOL bImageList;
//  POSITION pos;
//
//  int nThisVersion = 6;
//  writeInt(pFile, nThisVersion);
//  writeString(pFile, msAxTypeName);
//  writeLong(pFile, mType);
//  writeShort(pFile, m_ClientHeight);
//  writeInt(pFile, 400);
//  writeInt(pFile, mnID);
//
//  // serialize the image if it exists
//  if (mpImageList != NULL)
//  {
//    bImageList = TRUE;
//    writeBOOL(pFile, bImageList);
//    mpImageList->WriteToTextFile(pFile, fileName);
//  }
//  else
//  {
//    bImageList = FALSE;
//    writeBOOL(pFile, bImageList);
//  }
//
//  if (mType == CtlActiveX)
//  {
//    // save the activeX info.
//    writeString(pFile, m_sLicenseKey);
//    writeCLSID(pFile, m_clsid);
//    try
//    {
//      if (m_pStream) {
//				LARGE_INTEGER nZeroDisp = { 0, 0 };
//				ULARGE_INTEGER iSeekPtr;
//				HRESULT hr = m_pStream->Seek( nZeroDisp, STREAM_SEEK_END, &iSeekPtr );
//				ULONG cbStream = ULONG( iSeekPtr.QuadPart );
//				ULONG nBytesLeft = cbStream;
//
//				// store the bytes left
//				ar << nBytesLeft;
//
//				hr = m_pStream->Seek( nZeroDisp, STREAM_SEEK_SET, NULL );
//				ULONG nBytesRead;
//        BYTE* abData = new BYTE[nBytesLeft];
//        hr = m_pStream->Read( abData, nBytesLeft, &nBytesRead );
//        if ( nBytesRead > 0 ) {
//          writeBool(pFile, true);
//          writeBits(pFile, abData, nBytesRead);
//        } else {
//          writeBool(pFile, false);
//        }
//				delete[] abData;
//      } else {
//        writeBool(pFile, false);
//      }
//    }
//    catch( CFileException* pException )
//    {
//      pException->Delete();
//    }
//  }
//  // set counter for ArxControls
//  nCount = (int)mProperties.GetCount();
//
//  writeInt(pFile, nCount);
//
//  // set start position for navigating objects
//  pos = mProperties.GetHeadPosition();
//  // do loop to navigate objects
//  while (pos != NULL)
//  {
//    // get current object
//    TPropertyPtr pProp = mProperties.GetNext(pos);
//
//    // put object into archive
//    pProp->WriteToTextFile(pFile);
//
//    // increment counter
//    nCount--;
//  }
//	return statOK;
//}

void CDclControlTemplate::Serialize(CArchive& ar)
{
	BYTE nThisVersion = GetCurrentSaveVersion();
	if (ar.IsStoring())
	{
		ar << nThisVersion;
		ar << long(mType);
		ar << mnID;

		if( mType == _CtlOldFileExplorer )
			mType = CtlFileExplorer;

		// serialize the image if it exists
		ar << bool(mpImageList != NULL);
		if( mpImageList )
			mpImageList->Serialize(ar);

		// serialize ActiveX info if it exists
		ar << bool(mpAxCtrlInitInfo != NULL);
		if( mpAxCtrlInitInfo )
			mpAxCtrlInitInfo->Serialize( ar );

		// write property list
		ar << unsigned short(mProperties.size());
		for( TPropertyList::iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
		{
			(*iter)->Serialize(ar);
			#ifdef _DIAGNOSTIC
				if( (GetAsyncKeyState( VK_CONTROL ) & KF_UP) == KF_UP )
					TraceFmt( _T("> %s\r\n"), (*iter)->toString() );
			#endif //_DIAGNOSTIC
		}
	}
	else
	{
		ar >> nThisVersion;
		if( nThisVersion <= 10 )
		{
			BYTE bSkip;
			ar >> bSkip;
			assert( bSkip == 0 );
			ar >> bSkip;
			assert( bSkip == 0 );
			ar >> bSkip;
			assert( bSkip == 0 );
		}
		if (nThisVersion > GetCurrentSaveVersion())
			AfxThrowArchiveException(CArchiveException::badSchema, ar.m_strFileName );

		CString sAxTypeName;
		if( nThisVersion < 12 )
			ar >> sAxTypeName;
		long lType;
		ar >> lType;
		mType = (ControlType)lType;
		if( mType < 0 )
			mType = _CtlForm; //correct control type for controls from older ODC files
		else if( mType == _CtlOldFileExplorer )
			mType = CtlFileExplorer; //changed value, so need to correct type saved in older files
		if (nThisVersion < 9)
		{
			short nClientHeight;
			ar >> nClientHeight; //discard
		}
		if (nThisVersion < 8)
		{
			int nPurchaseState;
			ar >> nPurchaseState; //discard
		}

		if (nThisVersion >= 6)
			ar >> mnID;
		else if( mpOwner )
			mnID = mpOwner->GetUniqueControlId();
		if( mType == CtlSplitter )
		{
			assert( mnID > 2 ); //illegal splitter ID
			if( mnID <= 2 )
				mnID += 100;
		}
		mpImageList = NULL;
		if( nThisVersion >= 5 )
		{
			bool bImageList;
			if( nThisVersion <= 10 )
			{
				BOOL bImageListTemp;
				ar >> bImageListTemp;
				bImageList = (bImageListTemp != FALSE);
			}
			else
				ar >> bImageList;
			if( bImageList )
			{
				if( nThisVersion >= 12 )
				{
					mpImageList = new CDclImageList();
					mpImageList->Serialize(ar);
					if( mpImageList->IsNull() )
						mpImageList = NULL;
				}
				else
				{
					CImageListObject temp;
					temp.Serialize(ar);
					if( !temp.IsDeleted() )
						mpImageList = new CDclImageList( &temp.GetImageList() );
				}
			}
		}

		mpAxCtrlInitInfo = NULL;
		if (nThisVersion >= 3 )
		{
			if( nThisVersion >= 12)
			{
				bool bAxCtrlInitInfo;
				ar >> bAxCtrlInitInfo;
				if( bAxCtrlInitInfo )
				{
					mpAxCtrlInitInfo = new CDclAxCtrlInitInfo;
					mpAxCtrlInitInfo->Serialize( ar );
					if( mpAxCtrlInitInfo->IsNull() )
						mpAxCtrlInitInfo = NULL;
				}
			}
			else
			{
				if (mType == CtlActiveX)
				{
					CLSID clsid = CLSID_NULL;
					CString sLicenseKey;
					// get the activeX info.
					ar >> sLicenseKey;
					if (nThisVersion >= 4 && nThisVersion <= 9)
					{
						CString sBaseCode;
						ar >> sBaseCode;
						BOOL bLicenseChecked;
						ar >> bLicenseChecked;
					}

					SerializeCLSID(ar, clsid);

					bool bHasStream;
					if( nThisVersion <= 10 )
					{
						BOOL bHasStreamTemp;
						ar >> bHasStreamTemp;
						bHasStream = (bHasStreamTemp != FALSE);
					}
					else
						ar >> bHasStream;

					CComPtr< IStream > pStream = NULL;
					if( bHasStream )
					{
						HRESULT hResult = CreateStreamOnHGlobal( NULL, TRUE, &pStream );
						if( FAILED( hResult ) )
							return;

						ULONG cbStream;
						ar >> cbStream;
						while( cbStream > 0 )
						{
							BYTE abData[512];
							ULONG nBytesToRead = std::min<ULONG>( cbStream, sizeof( abData ) );
							ar.Read( abData, nBytesToRead );
							pStream->Write( abData, nBytesToRead, NULL );
							cbStream -= nBytesToRead;
						}

						LARGE_INTEGER nZeroDisp = { 0, 0 };
						pStream->Seek( nZeroDisp, STREAM_SEEK_SET, NULL );
					}
					mpAxCtrlInitInfo = new CDclAxCtrlInitInfo( clsid, sLicenseKey, sAxTypeName );
					if( mpAxCtrlInitInfo->IsNull() )
						mpAxCtrlInitInfo = NULL;
					else if( pStream )
						mpAxCtrlInitInfo->GetIStream() = pStream;
				}
			}
		}

		mProperties.clear();
		if (nThisVersion >= 2)
		{
			unsigned short nCount;
			ar >> nCount;
			while (nCount-- > 0)
			{
				TPropertyPtr pProp = new CPropertyObject( TDclControlLockedPtr( this ), PropInvalid );
				pProp->Serialize(ar);
				InsertNamedProperty( pProp );
			#ifdef _DIAGNOSTIC
				if( (GetAsyncKeyState( VK_CONTROL ) & KF_UP) == KF_UP )
					TraceFmt( _T("< %s\r\n"), pProp->toString() );
			#endif //_DIAGNOSTIC
			}
		}
		else
		{
			CList< CPropertyObject* > listProps;
			listProps.Serialize(ar);
			POSITION pos = listProps.GetHeadPosition();
			while( pos )
			{
				CPropertyObject* pProp = listProps.GetNext( pos );
				mProperties.push_back( pProp );
			}
		}
		// here were are going to add the font of this object to the font collection
		// before any pictures are loaded or any dialogs are displayed.
		theWorkspace.GetFontCollection().GetFont( TDclControlLockedPtr( this ), NULL );
	}

	if (!ar.IsStoring())
	{
		// remove any properties that shouldn't have been persisted or that were added erroneously in the past
		FormType eFormType = _FrmInvalid;
		if( mpOwner )
		{
			CDclFormObject* pOwnerForm = mpOwner;
			while( pOwnerForm->GetParentForm() )
				pOwnerForm = pOwnerForm->GetParentForm(); //get top level parent
			eFormType = pOwnerForm->GetType();
		}
		TPropertyList::iterator iter = mProperties.begin();
		while( iter != mProperties.end() )
		{
			TPropertyList::iterator iterAt = iter++;
			Prop::Id nID = (*iterAt)->GetID();
			switch( mType )
			{
			case CtlComboBox:
				switch( nID )
				{
				case Prop::EventUpdate:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlFrame:
				switch( nID )
				{
				case Prop::BackgroundColor:
				case Prop::ForegroundColor:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlGraphicButton:
				switch( nID )
				{
				case Prop::EventDblClicked:
				case Prop::ImageList:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlGrid:
				switch( nID )
				{
				case Prop::DragnDropToAutoCAD:
				case Prop::DragnDropFromControl:
				case Prop::DragnDropFromOther:
				case Prop::DragnDropBegin:
				case Prop::DragnDropAllowBegin:
				case Prop::DragnDropAllowDrop:
				case Prop::EventReturnPressed:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlImageComboBox:
				switch( nID )
				{
				case Prop::EventUpdate:
					mProperties.erase( iterAt );
					continue;
				case Prop::Sorted:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlFileExplorer:
				switch( nID )
				{
				case Prop::Left:
				case Prop::Top:
					mProperties.erase( iterAt );
					continue;
				case Prop::Width:
				case Prop::Height:
					if( !(*iterAt)->IsHidden() )
						(*iterAt)->SetHidden();
					break;
				case Prop::EventHelp:
					if( !(*iterAt)->GetStringValue().IsEmpty() && mpOwner )
					{ //transfer it to the parent form if possible
						CString sFormHelp = mpOwner->GetControlProperties()->GetStringProperty( Prop::EventHelp );
						if( sFormHelp.IsEmpty() )
							mpOwner->GetControlProperties()->SetStringProperty( Prop::EventHelp, (*iterAt)->GetStringValue() );
					}
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlListBox:
				switch( nID )
				{
				case Prop::HScrollBar:
				case Prop::UseTabStops:
				case Prop::EventClicked:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlOptionList:
				switch( nID )
				{
				case Prop::NoIntegralHeight:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlPictureBox:
				switch( nID )
				{
				case Prop::ImageList:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlProgressBar:
				switch( nID )
				{
				case Prop::DisplaySeconds:
					{
						bool bSet = (*iterAt)->GetBooleanValue();
						mProperties.erase( iterAt );
						if( bSet && !GetPropertyObject( Prop::ProgressLegend ) )
							AddLongProperty( Prop::ProgressLegend, PropEnum, 1 );
						continue;
					}
				case Prop::DisplayPercentage:
					{
						bool bSet = (*iterAt)->GetBooleanValue();
						mProperties.erase( iterAt );
						if( bSet && !GetPropertyObject( Prop::ProgressLegend ) )
							AddLongProperty( Prop::ProgressLegend, PropEnum, 2 );
						continue;
					}
				case Prop::CaptionSecond:
					{
						CString sVal = (*iterAt)->GetStringValue();
						if( sVal == _T("second") || sVal == _T("second remaining") )
							(*iterAt)->SetStringValue( _T("") ); //revert to default from resources
						break;
					}
				case Prop::CaptionSeconds:
					{
						CString sVal = (*iterAt)->GetStringValue();
						if( sVal == _T("seconds") || sVal == _T("seconds remaining") )
							(*iterAt)->SetStringValue( _T("") ); //revert to default from resources
						break;
					}
				case Prop::CaptionMinute:
					{
						if( (*iterAt)->GetStringValue() == _T("minute") )
							(*iterAt)->SetStringValue( _T("") ); //revert to default from resources
						break;
					}
				case Prop::CaptionMinutes:
					{
						if( (*iterAt)->GetStringValue() == _T("minutes") )
							(*iterAt)->SetStringValue( _T("") ); //revert to default from resources
						break;
					}
				}
				break;
			case CtlTextButton:
				switch( nID )
				{
				case Prop::EventDblClicked:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlCheckBox:
				switch( nID )
				{
				case Prop::Value:
					if( (*iterAt)->GetType() == PropBool )
					{
						bool bChecked = (*iterAt)->GetBooleanValue();
						(*iterAt)->SetType( PropLong );
						(*iterAt)->SetLongValue( bChecked? 1 : 0 );
					}
					break;
				}
				break;
			case CtlOptionButton:
				switch( nID )
				{
				case Prop::Value:
					if( (*iterAt)->GetType() == PropBool )
					{
						bool bChecked = (*iterAt)->GetBooleanValue();
						(*iterAt)->SetType( PropLong );
						(*iterAt)->SetLongValue( bChecked? 1 : 0 );
					}
					break;
				}
				break;
			case CtlBlockList:
				switch( nID )
				{
				case Prop::BlockName:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case CtlDwgList:
				switch( nID )
				{
				case Prop::EventClicked:
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case _CtlForm:
				if( mpOwner && mpOwner->GetParentForm() )
				{
					switch( nID )
					{
					case Prop::Name:
					case Prop::ControlBrowser:
					case Prop::VarName:
					case Prop::Custom:
					case Prop::UseBottomFromBottom:
					case Prop::UseTopFromBottom:
						mProperties.erase( iterAt ); //tab pages should not have these properties
						continue;
					}
				}
				switch( nID )
				{
				case Prop::ImageList:
					mProperties.erase( iterAt );
					continue;
				case Prop::FormEventShow:
					if( eFormType != FrmOptionsTab )
					{
						mProperties.erase( iterAt );
						continue;
					}
					break;
				}
				switch( eFormType )
				{
				case FrmOptionsTab:
					switch( nID )
					{
					case Prop::Custom:
					case Prop::DockableSides:
					case Prop::EventHelp:
						mProperties.erase( iterAt );
						continue;
					}
					break;
				case FrmModalDlg:
					break;
				case FrmModelessDlg:
					switch( nID )
					{
					case Prop::TitleBarIcon:
						mProperties.erase( iterAt );
						continue;
					}
					break;
				case FrmControlBar:
					//switch( nID )
					//{
					//case Prop::MinDialogWidth:
					//case Prop::MinDialogHeight:
					//case Prop::MaxDialogWidth:
					//case Prop::MaxDialogHeight:
					//	mProperties.erase( iterAt );
					//	continue;
					//}
					break;
				}
				break;
			}
			switch( nID )
			{
			case Prop::UseLeftFromRight:
			case Prop::UseRightFromRight:
			case Prop::UseTopFromBottom:
			case Prop::UseBottomFromBottom:
				if( (*iterAt)->GetType() == PropBool )
				{
					long lVal = (*iterAt)->GetLongValue();
					(*iterAt)->SetType( PropLong );
					(*iterAt)->SetLongValue( lVal );
				}
				break;
			case Prop::IconYSpacing: //erroneously added as an event in OpenDCL 4.1 Alpha 4
				if( (*iterAt)->GetType() == PropEvent )
				{
					if( GetPropertyObject( Prop::DragnDropToAutoCAD ) )
						mProperties.erase( iterAt );
					else
						(*iterAt)->SetID( Prop::DragnDropToAutoCAD );
				}
				else if( (*iterAt)->IsHidden() )
				{
					long lVal = (*iterAt)->GetLongValue();
					if( lVal == 0 )
						lVal = 32;
					mProperties.erase( iterAt );
					AddLongProperty( Prop::IconYSpacing, PropLong, lVal ); //re-insert it where it belongs
				}
				continue;
			case Prop::TabOrder:
			case Prop::FontColor:
			case Prop::LabelAlignment:
			case Prop::TabSelected:
			case Prop::ZOrder:
				mProperties.erase( iterAt );
				continue;
			case Prop::EventReturn:
				{
					CString sEvent = (*iterAt)->GetStringValue();
					mProperties.erase( iterAt );
					TPropertyPtr pNewEvent = AddStringProperty( Prop::EventReturnPressed, PropEvent, sEvent );
					pNewEvent->SetHidden();
				}
				continue;
			case Prop::URLLinkType:
				{
					CString sURL = GetStringProperty( Prop::Hyperlink );
					CString sPrefix;
					switch( (*iter)->GetEnumValue() )
					{
					case 1:
						sPrefix = _T("mailto:");
						break;
					default:
						sPrefix = _T("http://");
						break;
					}
					if( sURL.Left( sPrefix.GetLength() ) != sPrefix )
					{
						sURL = sPrefix + sURL;
						SetStringProperty( Prop::Hyperlink, sURL );
					}
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case Prop::InterfaceMode:
				if( (*iterAt)->GetType() == PropBool )
				{
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case Prop::FontSizeStyle:
				{
					long lFontSize = GetLongProperty( Prop::FontSize );
					if( (*iterAt)->GetBooleanValue() )
					{
						if( lFontSize < 0 )
							SetLongProperty( Prop::FontSize, -lFontSize );
					}
					else
					{
						if( lFontSize > 0 )
							SetLongProperty( Prop::FontSize, -lFontSize );
					}
					mProperties.erase( iterAt );
					continue;
				}
				break;
			case Prop::TabsCaption:
			case Prop::TabsImageList:
			case Prop::TabsTTT:
				if( (*iterAt)->IsHidden() )
					(*iterAt)->SetHidden( false );
				break;
			case Prop::_Private:
				if( (*iterAt)->GetType() == PropActiveXProp )
				{
					//ActiveX props with extra arguments were being erroneously hidden
					if( (*iterAt)->IsHidden() )
						(*iterAt)->SetHidden( false );
				}
				break;
			case Prop::EventInvoke:
				switch( eFormType )
				{
				case FrmControlBar:
				case FrmModelessDlg:
				case FrmPaletteDlg:
					if( (*iterAt)->IsHidden() )
						(*iterAt)->SetHidden( false );
					break;
				default:
					if( !(*iterAt)->IsHidden() )
						(*iterAt)->SetHidden();
					break;
				}
				break;
			}
		}
	}
}

size_t CDclControlTemplate::CountPropertyListItems( Prop::Id nID )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		return pProp->size();
	return 0;
}

CString CDclControlTemplate::GetPropertyListItem( Prop::Id nID, size_t nIndex )
{
	CString sValue;
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp && nIndex < pProp->size() )
	{
		if( pProp->GetType() == PropStringArray )
			sValue = pProp->GetConstStringArrayPtr()->at( nIndex );
		else if( pProp->GetType() == PropIntArray )
			sValue.Format( _T("%d"), pProp->GetConstIntArrayPtr()->at( nIndex ) );
	}
	return sValue;
}

TPropertyPtr CDclControlTemplate::GetRefCountedPtr( const CPropertyObject* pProperty ) const
{
	if( !pProperty )
		return NULL;
	return GetPropertyObject( pProperty->GetID() );
}

const TPropertyPtr CDclControlTemplate::GetPropertyObject( Prop::Id nID ) const
{
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetID() == nID )
			return (*iter);
	}
	return NULL;
}

TPropertyPtr CDclControlTemplate::GetPropertyObject( Prop::Id nID )
{
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetID() == nID )
			return (*iter);
	}
	return NULL;
}

TPropertyPtr CDclControlTemplate::FindPropertyObject( LPCTSTR pszApiName ) const
{
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetApiName() == pszApiName )
			return (*iter);
	}
	return NULL;
}

CRect CDclControlTemplate::GetWndRect() const
{
	CPoint ptThis( GetLongProperty( Prop::Left ), GetLongProperty( Prop::Top ) );
	CSize sizeThis( GetLongProperty( Prop::Width ), GetLongProperty( Prop::Height ) );
	return CRect( ptThis, sizeThis );
}

void CDclControlTemplate::SetFontProperties( const FontSettings& FS, UINT flags /*= fontAll*/ )
{
	if( flags & fontName )
		SetStringProperty( Prop::FontName, FS.name() );
	if( flags & fontSize )
		SetLongProperty( Prop::FontSize, FS.size() );
	else if( flags & fontScaled )
	{
		long lSize = GetLongProperty( Prop::FontSize );
		if( (lSize > 0) ^ FS.isScaled() )
			SetLongProperty( Prop::FontSize, -lSize );
	}
	if( flags & fontBold )
		SetBooleanProperty( Prop::FontBold, FS.isBold() );
	if( flags & fontItalic )
		SetBooleanProperty( Prop::FontItalic, FS.isItalic() );
	if( flags & fontUnderlined )
		SetBooleanProperty( Prop::FontUnderline, FS.isUnderlined() );
}

void CDclControlTemplate::SetGlobalVariableName( LPCTSTR pszParentName /*= NULL*/ )
{
	CString sParentName = pszParentName;
	if( sParentName.IsEmpty() )
		sParentName = mpOwner->GetKeyPath();
	CString sControlName = sParentName + _T('/') + GetKeyName();
	AddStringProperty( Prop::VarName, PropString, sControlName, true );
}

void CDclControlTemplate::ClearGlobalVariableName()
{
	SetStringProperty( Prop::VarName, NULL );
}

void CDclControlTemplate::ResetEventNames()
{
	for( TPropertyList::iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		TPropertyPtr pProp = *iter;
		switch( pProp->GetType() )
		{
		case PropEvent:
		case PropActiveXEvent:
			{
				if( !pProp->GetStringValue().IsEmpty() )
				{
					CString sEventName;
					sEventName.Format( _T("c:%s#On%s"), GetVarName(), pProp->GetName() );
					pProp->SetStringValue( sEventName );
				}
			}
			break;
		}
	}
}

bool CDclControlTemplate::SetStringProperty( Prop::Id nID, LPCTSTR pszValue )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetStringValue( pszValue );
}

TPropertyPtr CDclControlTemplate::AddStringProperty( Prop::Id nID,
																									 PropertyType type /*= PropString*/,
																									 LPCTSTR pszValue /*= NULL*/,
																									 bool bResetExisting /*= false*/ )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( TDclControlLockedPtr( this ), type, 0, nID );
		InsertNamedProperty( pProp );
		pProp->SetStringValue( pszValue );
	}
	else if( bResetExisting )
	{
		pProp->SetType( type );
		pProp->SetStringValue( pszValue );
	}
	return pProp;
}

bool CDclControlTemplate::SetBooleanProperty( Prop::Id nID, bool bValue /*= true*/ )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetBooleanValue( bValue );
}

TPropertyPtr CDclControlTemplate::AddBooleanProperty( Prop::Id nID,
																										PropertyType type /*= PropBool*/,
																										bool bValue /*= true*/,
																										bool bResetExisting /*= false*/ )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( TDclControlLockedPtr( this ), type, 0, nID );
		InsertNamedProperty( pProp );
		pProp->SetBooleanValue( bValue );
	}
	else if( bResetExisting )
	{
		pProp->SetType( type );
		pProp->SetBooleanValue( bValue );
	}
	return pProp;
}

bool CDclControlTemplate::SetLongProperty( Prop::Id nID, long lValue /*= -1*/ )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
		return false;
	return pProp->SetLongValue( lValue );
}

TPropertyPtr CDclControlTemplate::AddLongProperty( Prop::Id nID,
																								 PropertyType type /*= PropLong*/,
																								 long lValue /*= -1*/,
																								 bool bResetExisting /*= false*/ )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( !pProp )
	{
		pProp = new CPropertyObject( TDclControlLockedPtr( this ), type, 0, nID );
		InsertNamedProperty( pProp );
		pProp->SetLongValue( lValue );
	}
	else if( bResetExisting )
	{
		pProp->SetType( type );
		pProp->SetLongValue( lValue );
	}
	return pProp;
}

CString CDclControlTemplate::GetStringProperty(Prop::Id nID) const
{
	CString sValue;
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		sValue = pProp->GetStringValue();
	return sValue;
}

long CDclControlTemplate::GetLongProperty( Prop::Id nID ) const
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
	{
		//some boolean properties are read and written as longs,
		//so that case gets special handling here [ORW]
		if (pProp->GetType() == PropBool)
		{
			TraceFmt( _T("* 'PropLong' value requested from 'PropBool' property (ID: %d)! [%s]\r\n"), nID, _T(__FUNCTION__));
			return (long)pProp->GetBooleanValue();
		}
		return pProp->GetLongValue();
	}
	return -1;
}

void CDclControlTemplate::SetColorProperty( Prop::Id nID, COLORREF color )
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		pProp->SetOLEColorValue(color);
}

COLORREF CDclControlTemplate::GetColorProperty( Prop::Id nID ) const
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		return GetRGBColor( pProp->GetLongValue() );
	return RGB(0,0,0);
}

double CDclControlTemplate::GetDoubleProperty( Prop::Id nID ) const
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
		return pProp->GetDoubleValue();
	return 0.0;
}

bool CDclControlTemplate::GetBooleanProperty( Prop::Id nID ) const
{
	TPropertyPtr pProp = GetPropertyObject( nID );
	if( pProp )
	{
		//some enum properties are read and written as booleans,
		//so that case gets special handling here [ORW]
		if( pProp->GetType() == PropEnum )
		{
			TraceFmt( _T("* 'PropBool' value requested from 'PropEnum' property (ID: %d)! [%s]\r\n"), nID, _T(__FUNCTION__));
			return (pProp->GetEnumValue() != 0);
		}
		return pProp->GetBooleanValue();
	}
	return false;
}

TPropertyPtr CDclControlTemplate::GetMethods() const
{
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetType() == PropActiveXMethods )
			return (*iter);
	}
	return NULL;
}

TPropertyList::iterator CDclControlTemplate::FindPropertyInsertPos( Prop::Id id, bool bHidden )
{
	if( id == Prop::ControlBrowser )
		return mProperties.begin();
	return FindPropertyInsertPos( GetPropertyApiName( id ), bHidden );
}

TPropertyList::iterator CDclControlTemplate::FindPropertyInsertPos( LPCTSTR pszName, bool bHidden )
{
	if( !pszName || !*pszName )
		return mProperties.begin();
	if( mProperties.empty() )
		return mProperties.begin();

	TPropertyList::iterator iter = mProperties.end();
	do
	{
		--iter;
		assert( (*iter)->GetApiName() != pszName ); //trying to insert a property with a duplicate name?
		if( (*iter)->IsHidden() != bHidden )
			continue;
		if( (*iter)->GetApiName() < pszName )
			break;
	}
	while( iter != mProperties.begin() );
	return ++iter;
}

bool CDclControlTemplate::InsertNamedProperty( TPropertyPtr pProp )
{
	assert( pProp != NULL );
	if( !pProp )
		return false;

	if( pProp->GetID() == Prop::_Private )
	{
		mProperties.push_back( pProp );
		return true;
	}
	assert( *pProp->GetApiName() != _T('\0') );

	TPropertyList::iterator iter = FindPropertyInsertPos( pProp->GetApiName(), pProp->IsHidden() );
	if( iter != mProperties.end() && pProp->GetApiName() == (*iter)->GetApiName() )
	{
		TraceFmt( _T("* Attempt to insert duplicate property (%s) failed!\r\n"), (LPCTSTR)pProp->GetApiName() );
		return false;
	}
	mProperties.insert( iter, pProp );
	return true;
}

void CDclControlTemplate::ResetProperty( Prop::Id nId )
{
	TPropertyPtr pProp = GetPropertyObject( nId );
	if( pProp )
		pProp->clear();
}

void CDclControlTemplate::RemoveProperty( Prop::Id nId )
{
	for( TPropertyList::iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		if( (*iter)->GetID() == nId )
		{
			mProperties.erase( iter );
			break;
		}
	}
}

IOStatus CDclControlTemplate::ReadFromTextFile(std::ifstream &sFile, const CString &fileName)
{
	CStringA sClassname = readLine(sFile);
	if ( sClassname != "CDclControlTemplate" && sClassname != "CArxControlObject") return statInvalidFormat;
	int iVersion;
	if (!readInt(sFile, iVersion)) return statInvalidFormat;

	switch (iVersion) {
		case 6 :
			return ReadFromTextFile6(sFile, fileName);
			break;
	}

	return statInvalidFormat;
}

IOStatus CDclControlTemplate::ReadFromTextFile6(std::ifstream &sFile, const CString &fileName)
{
	CStringA sTypeName; //discard
	if (!readString(sFile, sTypeName)) return statInvalidFormat;
	long lType;
	if (!readLong(sFile, lType)) return statInvalidFormat;
	mType = (ControlType)lType;
	if( mType == -1 )
		mType = _CtlForm; //correct control type for controls from older ODC files
	short nClientHeight; //discard it
	if (!readShort(sFile, nClientHeight)) return statInvalidFormat;
	int nPurchaseState;
	if (!readInt(sFile, nPurchaseState)) return statInvalidFormat; //discard
	if (!readInt(sFile, mnID)) return statInvalidFormat;

	mpImageList = NULL;
	bool bImageList;
	if (!readBool(sFile, bImageList)) return statInvalidFormat;
	if (bImageList) {
		CImageListObject discard;
		IOStatus stat = discard.ReadFromTextFile(sFile, fileName);
		if (stat != statOK) return stat;
		mpImageList = new CDclImageList(&discard.GetImageList());
	}

	mpAxCtrlInitInfo = NULL;
	if (mType == CtlActiveX)
	{
		// get the activeX info.
		CStringA sKey;
		if (!readString(sFile, sKey)) return statInvalidFormat;
		CString sLicenseKey = CString( sKey );
		CStringA sBaseCode; //ignored
		if (!readString(sFile, sBaseCode)) return statInvalidFormat;
		BOOL bLicenseChecked; //ignored
		if (!readBOOL(sFile, bLicenseChecked)) return statInvalidFormat;

		CLSID clsid = CLSID_NULL;
		if (!readCLSID(sFile, clsid)) return statInvalidFormat;

		LARGE_INTEGER nDisplacement;
		CComPtr< IStream >pStream;
		try
		{
			BOOL bHasStream;

			if (!readBOOL(sFile, bHasStream)) return statInvalidFormat;

			if (bHasStream)
			{
				IStream *pStreamTemp = NULL;
				HRESULT hResult = CreateStreamOnHGlobal( NULL, TRUE, &pStreamTemp);

				if( FAILED( hResult ) ) return statInvalidFormat;

				std::string abData;
				if (!readBits(sFile, abData)) return statInvalidFormat;
				pStreamTemp->Write( abData.c_str(), abData.length(), NULL );

				nDisplacement.QuadPart = 0;
				pStreamTemp->Seek( nDisplacement, STREAM_SEEK_SET, NULL );
				pStream = pStreamTemp;
			}
		}
		catch( CFileException* pException )
		{
			pException->Delete();
			return statInvalidFormat;
		}
		mpAxCtrlInitInfo = new CDclAxCtrlInitInfo( clsid, sLicenseKey );
		if( mpAxCtrlInitInfo->IsNull() )
			mpAxCtrlInitInfo = NULL;
		else if( pStream )
			mpAxCtrlInitInfo->GetIStream() = pStream;
	}

	// get counter for objects
	int nCount;
	if (!readInt(sFile, nCount)) return statInvalidFormat;

	mProperties.clear();
	while (nCount-- > 0)
	{
		TPropertyPtr pProp = new CPropertyObject(TDclControlLockedPtr( this ), PropInvalid);
		IOStatus stat = pProp->ReadFromTextFile(sFile);
		if (stat != statOK) return stat;

		if( pProp->GetName().IsEmpty() )
		{
			switch( pProp->GetType() )
			{
			case PropActiveXProp:
			case PropActiveXEnum:
			case PropActiveXEvent:
			case PropActiveXRunTime:
			case PropActiveXMethods:
				pProp->SetStringValue( pProp->GetConstAxInterfaceDescriptorPtr()->GetName() );
			}
		}
		mProperties.push_back(pProp);
	}

	// here were are going to add the font of this object to the font collection
	// before any pictures are loaded or any dialogs are displayed.
	CFontCollection *pFontCol = &theWorkspace.GetFontCollection();
	pFontCol->GetFont(TDclControlLockedPtr( this ), NULL);

	return statOK;
}

CString CDclControlTemplate::GetKeyName() const
{
	return GetStringProperty( Prop::Name );
}

CString CDclControlTemplate::GetKeyPath() const
{
	CString sPath;
	if( mpOwner )
	{
		sPath = mpOwner->GetKeyPath();
		if( mType != _CtlForm )
			sPath += _T('/') + GetKeyName();
	}
	else
		sPath = GetKeyName();
	return sPath;
}

CString CDclControlTemplate::GetVarName() const
{
	CString sName = GetStringProperty( Prop::VarName );
	if( sName.IsEmpty() )
		sName = GetKeyPath();
	return sName;
}


#ifdef _DIAGNOSTIC
LPCTSTR CDclControlTemplate::toString() const
{
	CString sInstance;
	if( mpDlgControl )
		sInstance.Format( _T(" (DlgControl: %s)"), asString( mpDlgControl ) );
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("CDclControlobject [%ls: %ls%ls]"), asString( mType ), (LPCTSTR)GetKeyPath(), (LPCTSTR)sInstance );
	return buf;
}

void CDclControlTemplate::dump( bool bDeep /*= true*/ ) const
{
	CString sOut;
	sOut.Format( _T("%s\r\n"), toString() );
	theWorkspace.DisplayStatus( sOut );
	if( !bDeep )
		return;
	size_t idx = 0;
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
	{
		CString sProp;
		sProp.Format( _T("  #%4s: %s\r\n"), asString( idx++ ), (*iter)->toString() );
		theWorkspace.DisplayStatus( sProp );
	}
}
#endif


#ifdef _DEBUG
void CDclControlTemplate::dumpDebugger( bool bDeep /*= true*/ ) const
{
	TraceFmt( _T("%s\r\n"), toString() );
	if( !bDeep )
		return;
	size_t idx = 0;
	for( TPropertyList::const_iterator iter = mProperties.begin(); iter != mProperties.end(); ++iter )
		TraceFmt( _T("  #%4s: %s\r\n"), asString( idx++ ), (*iter)->toString() );
}
#endif
