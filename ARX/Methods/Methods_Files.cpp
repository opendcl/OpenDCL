// Methods_Files.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Files.h"
#include "MethodLexicon.h"
#include "ArgumentsRetrieval.h"


int FilesDir()
{
	int nArg = 0;
	CString sDir;
	CString sExtension;
	CFileFind finder;
	CString sFileName;
	BOOL bResult;



	if (!GetStringArgument(nArg, &sDir, sFilesDir))
	{
		// return -1
		acedRetInt(-1);
		return 0;
	}
	nArg++;
	
	if (!FindOptionalStringArgument(nArg, &sExtension, sFilesDir))
	{
		sExtension = _T("*.*");
	}

	// get the list of Files
	if (sDir.Right(1) == "\\")
		bResult = finder.FindFile(sDir + sExtension);
	else
		bResult = finder.FindFile(sDir + "\\" + sExtension);
	
	// Convert the array to a list that can be returned
	struct resbuf* rbpRetList = acutNewRb(RTSTR);
	struct resbuf* rbpTail = rbpRetList;

	while (bResult)
	{	
		bResult = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
		// recur infinitely!
		if (finder.IsDots())
			continue;
		
		sFileName = finder.GetFileName();
		
		acutNewString(sFileName, rbpTail->resval.rstring);
			
		if (bResult)
		{
			rbpTail->rbnext = acutNewRb(RTSTR);
			rbpTail = rbpTail->rbnext;
		}
	}
	
	acedRetList(rbpRetList);
	acutRelRb(rbpRetList);

	finder.Close();
	
	return 0;
}

