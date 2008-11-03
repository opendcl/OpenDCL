// Methods_BinFile.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_BinFile.h"
#include "ArgumentsRetrieval.h"
#include "ArxWorkspace.h"


class CBinFile : public CFile
{
	CArchive mArchive;
	bool mbError;
	IUnknown* mpUnknown;

public:
	static const short eEOL = -6001;
	static const short eEOF = -6002;

public:
	CBinFile( LPCTSTR lpszFileName, bool bForWrite = false )
		: CFile( lpszFileName, bForWrite?
														(CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive | CFile::osSequentialScan) :
														(CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan) )
		, mArchive( this, bForWrite? (CArchive::store) : (CArchive::load) )
		, mbError( false )
		, mpUnknown( new RefCountedPtrAsIUnknown< RefCountedPtr< const CBinFile > >( RefCountedPtr< const CBinFile >( this ) ) )
		{
		}
	virtual ~CBinFile()
		{
		}
	void setError( bool bError = true ) { mbError = bError; }
	bool isError() const { return mbError; }
	bool isWriting() const { return (mArchive.IsStoring() != FALSE); }
	CArchive& archive() { return mArchive; }
	operator IUnknown* () { return mpUnknown; }
	operator CArchive& () { return mArchive; }
};


ADSRESULT BinFile::Open()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CString sFilename;
	if( !GetStringArgument( pArgs, sFilename ) )
		return RSERR; //invalid input

	CString sOpenMode;
	if( !GetStringArgument( pArgs, sOpenMode ) )
		return RSERR; //invalid input

	bool bForWrite = false;
	if( sOpenMode.CompareNoCase( _T("r") ) == 0 )
		sFilename = theWorkspace.FindFile( sFilename );
	else if( sOpenMode.CompareNoCase( _T("w") ) == 0 )
		bForWrite = true;
	else
		return HandleArgValueError( pArgs );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CBinFile* pFile = NULL;
	try
	{
		pFile = new CBinFile( sFilename, bForWrite );
	}
	catch( CException* e )
	{
		e->Delete();
		return RSRSLT;
	}

	theArxWorkspace.AddUnknown( *pFile );
  theArxWorkspace.RetPointer( pFile, odcl::ptrBinFile );
  return RSRSLT;
}

ADSRESULT BinFile::Write()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CBinFile* pFile = NULL;
	if( !GetBinFileArgument( pArgs, pFile ) )
		return RSERR; //invalid input

	if( !pFile || !pFile->isWriting() || pFile->isError() )
		return HandleArgValueError( pArgs );

	CArchive& BinFile( *pFile );

	while( pArgs )
	{
		resbuf* pArgC = pArgs;
		pArgs = pArgs->rbnext;
		short type = pArgC->restype;
		BinFile << type;
		if( type >= 5000 && type <= 5024 )
		{
			switch( type )
			{
			case RTLE:	
			case RTLB:
			case RTNONE:
			case RTVOID:
			case RTDOTE:
			case RTNIL:
			case RTT:
			case RTRESBUF:
				break;
			case RTREAL:
			case RTANG:
			case RTORINT:
				BinFile << pArgC->resval.rreal;		
				break;
			case RTPOINT:
				BinFile << pArgC->resval.rpoint[X];
				BinFile << pArgC->resval.rpoint[Y];
				break;
			case RTSHORT:
				BinFile << pArgC->resval.rint;
				break;
			case RTSTR:
			case RTDXF0:
				BinFile << CStringW( pArgC->resval.rstring );
				break;
			case RTENAME:
			case RTPICKS:
				BinFile << pArgC->resval.rlname[0];		
				BinFile << pArgC->resval.rlname[1];	
				break;
			case RT3DPOINT:
				BinFile << pArgC->resval.rpoint[X];
				BinFile << pArgC->resval.rpoint[Y];
				BinFile << pArgC->resval.rpoint[Z];
				break;
			case RTLONG:
				BinFile << pArgC->resval.rlong;
				break;
			default:
				return HandleArgValueError( pArgs );
			}
			continue;
		}
		AcDb::DwgDataType datatype = acdbGroupCodeToType( type );
		switch( datatype )
		{
		case kDwgNotRecognized:
		case kDwgNull:
			break;
		//case kDwgBChunk:
		//	break;
		case kDwgReal:
			BinFile << pArgC->resval.rreal;		
			break;
		case kDwgInt16:
		case kDwgInt8:
			BinFile << pArgC->resval.rint;		
			break;
		case kDwgText:
		case kDwgHandle:
			BinFile << CStringW( pArgC->resval.rstring );
			break;
		case kDwgHardOwnershipId:
		case kDwgSoftOwnershipId:
		case kDwgHardPointerId:
		case kDwgSoftPointerId:
			BinFile << pArgC->resval.rlname[0];		
			BinFile << pArgC->resval.rlname[1];		
			break;
		case kDwg3Real:
			BinFile << pArgC->resval.rpoint[X];
			BinFile << pArgC->resval.rpoint[Y];
			BinFile << pArgC->resval.rpoint[Z];
			break;
		case kDwgInt32:
			BinFile << pArgC->resval.rlong;
			break;
		default:
			return HandleArgValueError( pArgs );
		}
	}
	pFile->archive() << CBinFile::eEOL;
	acedRetT();
	return RSRSLT;
}

ADSRESULT BinFile::Read()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CBinFile* pFile = NULL;
	if( !GetBinFileArgument( pArgs, pFile ) )
		return RSERR; //invalid input

	if( !pFile || pFile->isWriting() || pFile->isError() )
		return HandleArgValueError( pArgs );

	bool bOneLine = false;
	if( !GetBoolArgument( pArgs, bOneLine ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArchive& BinFile( *pFile );
	short nStopAt = (bOneLine? CBinFile::eEOL : CBinFile::eEOF);

	resbuf* prbHead = NULL;
	resbuf* prbTail = NULL;
	try
	{
		while( true )
		{
			short type;
			BinFile >> type;
			if( type == nStopAt )
				break;
			if( type == CBinFile::eEOL )
				continue;
			resbuf* prbNew = NULL;
			if( type >= 5000 && type <= 5024 )
			{
        prbNew = acutNewRb( type );
				switch( type )
				{
        case RTLE:
        case RTLB:
        case RTNONE:
        case RTVOID:
        case RTDOTE:			
        case RTNIL:
        case RTT:
        case RTRESBUF:		
          break;
        case RTREAL:
        case RTANG:
        case RTORINT:
          BinFile >> prbNew->resval.rreal;
					break;
        case RTPOINT:
					BinFile >> prbNew->resval.rpoint[X];		
					BinFile >> prbNew->resval.rpoint[Y];
					break;
        case RTSHORT:
					BinFile >> prbNew->resval.rint;
					break;
        case RTDXF0:
        case RTSTR:
					{
						CStringW sText;
						BinFile >> sText;
						acutNewString( CString( sText ), prbNew->resval.rstring );
					}
					break;
        case RTENAME:
        case RTPICKS:
					BinFile >> prbNew->resval.rlname[0];		
					BinFile >> prbNew->resval.rlname[1];
					break;
        case RT3DPOINT:
					BinFile >> prbNew->resval.rpoint[X];		
					BinFile >> prbNew->resval.rpoint[Y];		
					BinFile >> prbNew->resval.rpoint[Z];
					break;
        case RTLONG:
					BinFile >> prbNew->resval.rlong;
					break;
				default:
					acutRelRb( prbNew );
					acutRelRb( prbHead );
					pFile->setError();
					return RSRSLT;
				}
			}
			else
			{
				AcDb::DwgDataType datatype = acdbGroupCodeToType( type );
				switch( datatype )
				{
				case kDwgNotRecognized:
				case kDwgNull:
					break;
				//case kDwgBChunk:
				//	break;
				case kDwgReal:
					BinFile >> prbNew->resval.rreal;		
					break;
				case kDwgInt16:
				case kDwgInt8:
					BinFile >> prbNew->resval.rint;		
					break;
				case kDwgText:
				case kDwgHandle:
					{
						CStringW sText;
						BinFile >> sText;
						acutNewString( CString( sText ), prbNew->resval.rstring );
					}
					break;
				case kDwgHardOwnershipId:
				case kDwgSoftOwnershipId:
				case kDwgHardPointerId:
				case kDwgSoftPointerId:
					BinFile >> prbNew->resval.rlname[0];		
					BinFile >> prbNew->resval.rlname[1];		
					break;
				case kDwg3Real:
					BinFile >> prbNew->resval.rpoint[X];
					BinFile >> prbNew->resval.rpoint[Y];
					BinFile >> prbNew->resval.rpoint[Z];
					break;
				case kDwgInt32:
					BinFile >> prbNew->resval.rlong;
					break;
				default:
					acutRelRb( prbNew );
					acutRelRb( prbHead );
					pFile->setError();
					return RSRSLT;
				}
			}
			if( prbTail )
				prbTail->rbnext = prbNew;
			else
				prbHead = prbNew;
			prbTail = prbNew;
		}
	}
	catch( CException* e )
	{
		e->Delete();
		return RSRSLT;
	}
	acedRetList( prbHead );
	acutRelRb( prbHead );
	return RSRSLT;
}

ADSRESULT BinFile::Close()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CBinFile* pFile = NULL;
	if( !GetBinFileArgument( pArgs, pFile ) )
		return RSERR; //invalid input

	if( !pFile )
		return HandleArgValueError( pArgs );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	try
	{
		if( pFile->isWriting() && !pFile->isError() )
			pFile->archive() << CBinFile::eEOF;
		theArxWorkspace.RemoveUnknown( *pFile ); //should delete it
	}
	catch( CException* e )
	{
		e->Delete();
		return RSRSLT;
	}
	acedRetT();
	return RSRSLT;
}

ADSRESULT BinFile::Check()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CBinFile* pFile = NULL;
	if( !GetBinFileArgument( pArgs, pFile ) )
		return RSERR; //invalid input

	if( !pFile )
		return HandleArgValueError( pArgs );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( pFile->isError() )
		acedRetInt( -1 );
	else if( !pFile->isWriting() &&
					 pFile->archive().IsBufferEmpty() &&
					 pFile->GetPosition() >= pFile->GetLength() )
		acedRetInt( 1 );
	else
		acedRetInt( 0 );
	return RSRSLT;
}
