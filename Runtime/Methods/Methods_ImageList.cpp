// Methods_ImageList.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ImageList.h"
#include "ArgumentsRetrieval.h"
#include "ArxWorkspace.h"
#include "Resource.h"


ADSRESULT ImageList::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TImageListPtr pImageList;
	if( !GetImageListArgument( pArgs, pImageList ) )
		return RSERR; //wrong argument type

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int ctImage = 0;
	if( pImageList )
		ctImage = pImageList->GetImageList().GetImageCount();

	acedRetInt( ctImage );

	return (RSRSLT) ;
}

ADSRESULT ImageList::GetSize()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TImageListPtr pImageList;
	if( !GetImageListArgument( pArgs, pImageList ) )
		return RSERR; //wrong argument type

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CSize sizeImage(-1, -1);
	if( pImageList )
		sizeImage = pImageList->GetSize();

	resbuf rbHeight = {NULL, RTSHORT};
	rbHeight.resval.rint = sizeImage.cy;
	resbuf rbWidth = {&rbHeight, RTSHORT};
	rbWidth.resval.rint = sizeImage.cx;
	acedRetList(&rbWidth);

	return (RSRSLT) ;
}

ADSRESULT ImageList::SetAt()
{
	struct resbuf *pArgs =acedGetArgs () ;

	TImageListPtr pImageList;
	if( !GetImageListArgument( pArgs, pImageList ) )
		return RSERR; //wrong argument type
	
	if( !pImageList )
		return RSERR; //invalid image list

	long idxPic = -1;
	if( !GetLongArgument( pArgs, idxPic ) )
		return RSERR; //invalid argument

	bool bDelete = GetNilArgument( pArgs, true );

	CString sFilename;
	if( !bDelete )
	{
		if( !GetStringArgument( pArgs, sFilename ) )
			return RSERR; //wrong argument type
	}

	//optional arguments
	int nWidth = -1;
	int nHeight = -1;
	if( GetIntArgument( pArgs, nWidth, true ) )
	{
		if( !GetIntArgument( pArgs, nHeight ) )
			return RSERR; //invalid input

		if( pImageList && pImageList->GetCount() > 0 )
		{
			const CSize& sizeImage = pImageList->GetSize();
			if( (nWidth != -1 && nWidth != sizeImage.cx) || (nHeight != -1 && nHeight != sizeImage.cy) )
				return RSRSLT; //size does not match existing image size
		}
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	if( bDelete )
	{
		if( pImageList->DeletePicture( idxPic ) )
			acedRetInt( idxPic );
	}
	else if( idxPic >= 0 && idxPic < pImageList->GetCount() )
	{
		CDclPicture pic( -1, sFilename );
		if( pImageList->ReplacePicture( idxPic, CDC::FromHandle( ::GetDC( NULL ) ), pic.GetPictureDisp() ) )
			acedRetInt( idxPic );
	}
	else if( idxPic == -1 || idxPic == pImageList->GetCount() )
	{
		if( nWidth >= 0 || nHeight >= 0 )
			pImageList->SetSize( CSize( nWidth, nHeight ) );
		CDclPicture pic( -1, sFilename );
		idxPic = pImageList->AddPicture( CDC::FromHandle( ::GetDC( NULL ) ), pic.GetPictureDisp() );
		if( idxPic >= 0 )
			acedRetInt( idxPic );
	}

	return (RSRSLT) ;
}
