// Folder.h: interface for the CFolder class.
//

#pragma once


class CFolder  
{
public:
	CFolder();
	CFolder(CString pathDescription, CString path, int imageIndex=0, int imageIndexExpanded=1);
	virtual ~CFolder();

public:
	int m_imageIndex;		
	int m_imageIndexExpanded;		
	CString m_pathDescription;	
	CString m_path;	
};
