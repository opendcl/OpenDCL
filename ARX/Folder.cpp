// Folder.cpp: implementation of the CFolder class.
//

#include "stdafx.h"
#include "Folder.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFolder::CFolder()
{

}

CFolder::CFolder(CString pathDescription, CString path, int imageIndex, int imageIndexExpanded)
{	m_pathDescription = pathDescription;
	m_path = path;
	m_imageIndex = imageIndex;
	m_imageIndexExpanded = imageIndexExpanded;
}

CFolder::~CFolder()
{

}
