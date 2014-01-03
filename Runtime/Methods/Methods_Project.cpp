// Methods_Project.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Project.h"
#include "ArgumentsRetrieval.h"
#include "ArxWorkspace.h"
#include "Resource.h"
#include "Base64.h"


ADSRESULT Project::Load()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CString sPath;
	if( !GetStringArgument( pArgs, sPath ) )
		return RSERR; //invalid argument

	//optional arguments
	bool bReload = false;
	CString sKeyName;
	if( GetBoolArgument( pArgs, bReload, true ) )
	{
		if( !GetStringArgument( pArgs, sKeyName, true ) )
			GetNilArgument( pArgs, true );
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( sPath.IsEmpty() )
		return RSERR; //invalid argument

	TArxProjectPtr pProject = theArxWorkspace.LoadProjectFile( sPath, sKeyName, bReload );
	if( !pProject )
	{
		CString sFmt = theWorkspace.LoadResourceString( IDS_PROJECTLOADFAILURE );
		if( !sFmt.IsEmpty() )
		{
			CString sAlertMsg;
			sAlertMsg.Format( sFmt, (LPCTSTR)sPath );
			theWorkspace.DisplayAlert( sAlertMsg );
		}
		return RSRSLT; 
	}
	acedRetStr( pProject->GetKeyName() );

	return (RSRSLT) ;
}

ADSRESULT Project::Unload()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TArxProjectPtr pProject;
	if( !GetProjectArgument( pArgs, pProject ) )
		return RSERR; //wrong argument type

	//optional arguments
	bool bForce = false;
	GetBoolArgument( pArgs, bForce, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pProject )
		return RSERR; //too many arguments

	if( theArxWorkspace.UnloadProject( pProject, bForce ) )
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Project::SaveAs()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TArxProjectPtr pProject;
	if( !GetProjectArgument( pArgs, pProject ) )
		return RSERR; //wrong argument type

	CString sFilename;
	if( !GetStringArgument( pArgs, sFilename ) )
		return RSERR; //wrong argument type

	//optional arguments
	CString sPassword;
	GetStringArgument( pArgs, sPassword, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pProject )
		return RSERR; //too many arguments

	if( pProject->GetPassword() != sPassword )
		return RSRSLT; //wrong password

	if (pProject->WriteToFile( sFilename ) == statOK)
		acedRetT();

	return (RSRSLT) ;
}

ADSRESULT Project::Import()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CString sRawDataIn;
	PropVal::TCStringArray rsRawData;
	if( pArgs->restype == RTLB )
	{ //we expect a list of strings
		if( !GetStringArrayArgument( pArgs, rsRawData ) )
			return RSERR; //wrong argument type
		for( size_t idx = 0; idx < rsRawData.size(); ++idx )
			sRawDataIn += rsRawData[idx];
	}
	else
	{ //otherwise it must be a single string
		if( !GetStringArgument( pArgs, sRawDataIn ) )
			return RSERR; //wrong argument type
	}

	//optional arguments
	CString sPassword;
	CString sProjectKey;
	if( GetStringArgument( pArgs, sPassword, true ) )
		GetStringArgument( pArgs, sProjectKey, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	TArxProjectPtr pProject;
	std::string sRawData;
#ifdef _UNICODE
	CStringA sRawDataA( sRawDataIn );
	sRawData = (LPCSTR)sRawDataA;
#else
	sRawData = sRawDataIn;
#endif
	try
	{
		std::string sDecodedData = base64_decode( sRawData );
		CMemFile Data( (BYTE*)sDecodedData.c_str(), sDecodedData.length() );
		pProject = theArxWorkspace.ImportProject( Data, sProjectKey );
		if (pProject)
		{
			if( !sPassword.IsEmpty() )
				pProject->SetPassword( sPassword );
			acedRetStr( pProject->GetKeyName() );
		}
	}
	catch( ... )
	{
		return RSERR; 
	}

	return (RSRSLT) ;
}

ADSRESULT Project::Export()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TArxProjectPtr pProject;
	if( !GetProjectArgument( pArgs, pProject ) )
		return RSERR; //wrong argument type

	//optional arguments
	CString sPassword;
	GetStringArgument( pArgs, sPassword, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pProject )
		return RSERR; //too many arguments

	if( pProject->GetPassword() != sPassword )
		return RSRSLT; //wrong password

	CMemFile bufData( 0x10000 );
	if (theArxWorkspace.ExportProject(pProject, bufData))
	{
		UINT cbData = (UINT)bufData.GetLength();
		BYTE* pbData = bufData.Detach();
		CString sRawData = base64_encode( pbData, cbData ).c_str();
		bufData.Attach( pbData, cbData );
		acedRetStr(sRawData);
	}

	return (RSRSLT) ;
}

ADSRESULT Project::GetPictureSize()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TArxProjectPtr pProject;
	if( !GetProjectArgument( pArgs, pProject ) )
		return RSERR; //invalid argument

	long id = -1;
	if( !GetLongArgument( pArgs, id ) )
		return RSERR; //invalid argument

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if (!pProject)
		return RSRSLT; //project not found

	CSize sizePicture;
	if (!pProject->GetPictureSize (id, sizePicture))
		return RSRSLT; //no picture with that id

	resbuf rbHeight = {NULL, RTSHORT};
	rbHeight.resval.rint = sizePicture.cy;
	resbuf rbWidth = {&rbHeight, RTSHORT};
	rbWidth.resval.rint = sizePicture.cx;
	acedRetList(&rbWidth);

	return (RSRSLT) ;
}

ADSRESULT Project::GetForms()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TArxProjectPtr pProject;
	if( !GetProjectArgument( pArgs, pProject ) )
		return RSERR; //wrong argument type

	//optional arguments
	CString sPassword;
	GetStringArgument( pArgs, sPassword, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( !pProject )
		return RSERR; //too many arguments

	if( pProject->GetPassword() != sPassword )
		return RSRSLT; //wrong password

	resbuf* prbForms = NULL;
	resbuf* prbTail = NULL;
	const TDclFormList& Forms = pProject->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		resbuf* prbForm = acutNewRb( RTENAME );
		prbForm->resval.rlname[0] = (LONG_PTR)(const CDclControlObject*)(*iter)->GetControlProperties();
		prbForm->resval.rlname[1] = odcl::ptrDclControl;
		if( prbTail )
			prbTail->rbnext = prbForm;
		else
			prbForms = prbForm;
		prbTail = prbForm;
	}
	acedRetList( prbForms );
	acutRelRb( prbForms );

	return (RSRSLT) ;
}
