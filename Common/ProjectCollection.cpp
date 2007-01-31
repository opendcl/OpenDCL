// ProjectCollection.cpp : implementation file
//

#include "stdafx.h"
#include "ProjectCollection.h"
#include "Project.h"
#include "FontCollection.h"
#include "DclFormObject.h"


/////////////////////////////////////////////////////////////////////////////
// CProjectCollection

IMPLEMENT_SERIAL(CProjectCollection, CObject, 1)

CProjectCollection::CProjectCollection()
{
}

CProjectCollection::~CProjectCollection()
{	
}

void CProjectCollection::ClearProjects()
{
	POSITION pos;
	int nCount = 0;

	while (nCount < mProjectCollection.GetCount())
	{
		// always get the first index when clearing a list
		pos = mProjectCollection.FindIndex(0);
		// get the object
		CProject *pProject = mProjectCollection.GetAt(pos);
		// remove the object from the list
		mProjectCollection.RemoveAt(pos);
		// clear everything in this object
		pProject->ClearProject();
		// delete the object from memory
		delete pProject;
		// increment the counter to include the next object
		nCount++;
	}
}
CProject* CProjectCollection::GetProject(CString ProjectName)
{
	POSITION pos;
	int nCount = 0;

	while (nCount < mProjectCollection.GetCount())
	{
		// always get the first index when clearing a list
		pos = mProjectCollection.FindIndex(nCount);
		// get the object
		CProject *pProject = mProjectCollection.GetAt(pos);
		
		// if this project's key matches the one queried for
		if (pProject->GetKeyName() == ProjectName)
			// return the project pointer
			return pProject;

		// increment the counter to include the next object
		nCount++;
	}
	return NULL;
}

void CProjectCollection::AddProject(CProject *pProject)
{
	mProjectCollection.AddTail(pProject);		
}

void CProjectCollection::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	// create a position variable to hold the counter increment
	POSITION pos;	
	short nCount;
	
	if (ar.IsStoring())
	{
		CFontCollection fontCol;

		// set counter , time it by -1 because we forgot to put in a version arg.
		nCount = mProjectCollection.GetCount() * -1;
		ar << nCount;		
		
		// set start position for navigating dcl forms
		pos = mProjectCollection.GetHeadPosition();
		
		// do loop to navigate projects
		while (pos != NULL)
		{
			// get current Dcl form
			CProject *pProject = mProjectCollection.GetNext(pos);		
		
			// set start position for navigating dcl forms
			POSITION dclPos = pProject->GetDclFormList().GetHeadPosition();
			
			// do loop to navigate dcl forms
			while (dclPos != NULL)
			{
				// get a pointer to the form
				CDclFormObject *pDcl = pProject->GetDclFormList().GetNext(dclPos);
				assert(pDcl != NULL);
				if(pDcl)
					pDcl->AddControlFonts(fontCol);
			}			
		}
		
		// set counter 
		int nFontCount = (WORD)fontCol.GetFontList().GetCount();
		ar << nFontCount;		
		
		// set start position for navigating dcl forms
		pos = fontCol.GetFontList().GetHeadPosition();
		
		// do loop to navigate dcl forms
		while (pos != NULL)
		{
			// get the font info
			LOGFONT stFont;
			fontCol.GetFontList().GetNext(pos)->GetLogFont(&stFont);		
			BOOL bSizeStyle = (stFont.lfHeight > 0);
			
			ar << CStringA(stFont.lfFaceName); //writing in MBCS format
			ar << (LONG)(bSizeStyle? stFont.lfHeight : -stFont.lfHeight); //need to "MulDiv" using device res [ORW]
			ar << (BOOL)(stFont.lfWeight == FW_BOLD);
			ar << (BOOL)stFont.lfItalic;
			ar << (BOOL)stFont.lfUnderline;
			ar << (BOOL)stFont.lfStrikeOut;
			ar << (BOOL)bSizeStyle;
		}
		
		// set start position for navigating dcl forms
		pos = mProjectCollection.GetHeadPosition();
		
		// do loop to navigate dcl forms
		while (pos != NULL)
		{
			// get current Dcl form
			CProject *pProject = mProjectCollection.GetNext(pos);		
			// put dcl form into archive
			pProject->Serialize(ar);			
		}
	}
	
}
