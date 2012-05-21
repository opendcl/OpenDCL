// ArxHatchCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxHatchCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CArxHatchCtrl

CArxHatchCtrl::CArxHatchCtrl( TDclControlPtr pTemplate,
															CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CArxGsViewCtrl( pTemplate, pPane, nID, false )
, mpHatchDb( NULL )
, mridLoop( 4 )
{
	mridLoop.setLogicalLength( 4 );
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxHatchCtrl::~CArxHatchCtrl()
{
	delete mpHatchDb;
}

bool CArxHatchCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	switch( pProp->GetID() )
	{
	case Prop::HatchScale:
		DisplayHatchPattern(msHatchPattern);
		break;
	}
	return true;
}

void CArxHatchCtrl::Clear()
{
	msHatchPattern.Empty();
	if( mpHatchDb )
	{
		delete mpHatchDb;
		mpHatchDb = NULL;
	}
	midHatch.setNull();
	clearAll();
}

bool CArxHatchCtrl::DisplayHatchPattern( LPCTSTR pszPattern )
{
	msHatchPattern = pszPattern;

	AcDbDatabase* pWorkingDb = new AcDbDatabase( true, true );
	if( !pWorkingDb )
		return false;
	
	AcDbBlockTable* pBlockTable = NULL;
	Acad::ErrorStatus es = pWorkingDb->getSymbolTable( pBlockTable, AcDb::kForRead );
	if( es != Acad::eOk )
	{
		delete pWorkingDb;
		return false;
	}

	AcDbBlockTableRecord* pModelSpace = NULL;
	es = pBlockTable->getAt( ACDB_MODEL_SPACE, pModelSpace, AcDb::kForWrite );
	pBlockTable->close();
	if( es != Acad::eOk )
	{
		delete pWorkingDb;
		return false;
	}

	// Construct database AcDbLines
	//
	AcGePoint3d vertexPts[4];
	
	int nTheWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int nTheHeight =::GetSystemMetrics(SM_CYSCREEN);
	CRect rc(0,0,nTheWidth, nTheHeight);
		
	rc.left -= nTheWidth*0.5;
	rc.right -= nTheWidth*0.5;
	rc.top -= nTheHeight*0.5;
	rc.bottom -= nTheHeight*0.5;

	vertexPts[0].set(rc.left, rc.top, 0.0);
	vertexPts[1].set(rc.right, rc.top, 0.0);
	vertexPts[2].set(rc.right, rc.bottom, 0.0);
	vertexPts[3].set(rc.left, rc.bottom, 0.0);

	COLORREF crFore = mColorService.GetForegroundColor();
	AcCmColor clr;
	clr.setColorMethod( AcCmEntityColor::kByColor );
	clr.setRGB( GetRValue(crFore),
							GetGValue(crFore),
							GetBValue(crFore) );

  for( int i = 0; i < 4; i++ ) 
	{
		AcDbLine* pLine = new AcDbLine();
		pLine->setStartPoint( vertexPts[i] );
		pLine->setEndPoint( vertexPts[(i == 3) ? 0 : i + 1] );
		//pLine->setVisibility(AcDb::kInvisible);
		pLine->setColor( clr );
		AcDbObjectId idLine;
		es = pModelSpace->appendAcDbEntity( idLine, pLine );
		assert( es == Acad::eOk );
		if( es != Acad::eOk )
			delete pLine;
		else
			pLine->close();
		mridLoop.setAt( i, idLine );
	}

	double dblPatternScaleFactor = 1.0;
	AcDbDatabase* pCurDwg = acdbCurDwg();
	if( pCurDwg && pCurDwg->measurement() == AcDb::kEnglish )
		dblPatternScaleFactor = 25.4;

	AcDbHatch* pHatch = new AcDbHatch;
	pHatch->setNormal( AcGeVector3d( 0.0, 0.0, 1.0 ) );
	pHatch->setElevation( 0.0 );
	pHatch->setPatternScale( dblPatternScaleFactor * mpTemplate->GetDoubleProperty( Prop::HatchScale ) );
	pHatch->setPatternAngle( 0.0 );
	//pHatch->setAssociative(Adesk::kTrue);
	//pHatch->setHatchStyle(AcDbHatch::kNormal);
	pHatch->appendLoop( AcDbHatch::kExternal, mridLoop );
	pHatch->setPattern( AcDbHatch::kPreDefined, pszPattern );
	pHatch->evaluateHatch();
	pHatch->setColor( clr );
	
	es = pModelSpace->appendAcDbEntity( midHatch, pHatch );
	assert( es == Acad::eOk );
	if( es != Acad::eOk )
		delete pHatch;
	else
		pHatch->close();

	AcGePoint3d extMax = pWorkingDb->extmax();

	DisplayBTR( pModelSpace, 1.0, false, 1, AcGeVector3d::kZAxis );

	pModelSpace->close();

	return true;
}
