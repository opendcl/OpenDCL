/*********************************************************************

   SADirRead

   Copyright (C) 2002 Smaller Animals Software, Inc.

   This software is provided 'as-is', without any express or implied
   warranty.  In no event will the authors be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

   3. This notice may not be removed or altered from any source distribution.

   http://www.smalleranimals.com
   smallest@smalleranimals.com

**********************************************************************/

// SADirRead.h: interface for the CSADirRead class.
//
//////////////////////////////////////////////////////////////////////

/*
   Basic use:

   #include "SADirRead.h"

	CSADirRead dr;

   // look in pFolderName , and all subfolders
	dr.GetDirs(pFolderName, true);

   // get all JPG and BMP files
   dr.GetFiles("*.jpg");
   dr.GetFiles("*.bmp");

	CSADirRead::SAFileVector &fileVector = dr.Files();
   // now, fileVector contains a list of all .JPG and .BMP files in pFolderName (and all subfolders)

   // TRACE the names of the files:
   for (CSADirRead::SAFileVector::const_iterator fit = fileVector.begin(); fit!=fileVector.end(); fit++)
	{
		TRACE("%s: %u64 bytes\n", (*fit).m_sName, (*fit).size);
	}
*/

/**********************************************************************
   USE_WIN32_FINDFILE

   if you comment this next #define, the class will use the C 
   library "findfirst", "findnext", "findclose" functions. Otherwise,
   it will use the Win32 functions "FindFirstFile", "FindNextFile" and "FindClose".
**********************************************************************/

#pragma once


#define USE_WIN32_FINDFILE


#include <vector>
using namespace std;
#pragma warning(disable:4786)


//////////////////////////////////////////////////////////////////////

class CSADirRead  
{
public:
	CSADirRead();
	virtual ~CSADirRead();

   /* 
      GetDirs

      first, build the list of folders we will scan. we do this as a separate step
      to allow the class to scan multiple folders when GetFiles is called.

      if bRecurse is true, this adds the input folder and all subfolders to 
      the list of folders to scan. if Recurse is False, it adds pDirPath to the 
      list of folders to scan.
   */
	bool	GetDirs(const TCHAR * pDirPath, bool bRecurse);


   /* 
      GetFiles

      if bIncludeFoldersInFileList is true, folder names will be 
      included along with file names.

      if bIncludeFilesInFileList is false, and set bIncludeFoldersInFileList to 
      true, if you only want to get a list of sub-folders.

      then get the files in the folders you've specified above. all folders specified
      in your GetDirs calls are scanned. 
   */
	bool	GetFiles(const TCHAR *pFilemask, bool bIncludeFilesInFileList = true, bool bIncludeFoldersInFileList = false);

   // remove all entries
	bool	ClearDirs();
	bool	ClearFiles();

	// if you want to sort the data
	enum {eSortNone, eSortAlpha, eSortWriteDate, eSortSize};
	bool	SortFiles(int iSortStyle, bool bReverse);

	//////////////////////////////////////////////////////////////////////

	// directories
	struct CSADirEntry
	{
		CSADirEntry()	{}
		CSADirEntry(const CString &s)
		{
			m_sName = s;
		}
		CString m_sName;
	};

	typedef vector<CSADirEntry> SADirVector;

	SADirVector &Dirs()	{return m_dirs;}

	//////////////////////////////////////////////////////////////////////

	// files
#ifdef USE_WIN32_FINDFILE
	struct CSAFileEntry
	{
		CString	m_sName;
      bool     bIsFolder;
		unsigned int attrib;
		unsigned __int64 time_create;
		unsigned __int64 time_access;
		unsigned __int64 time_write;
		unsigned __int64 size;
	};
#else
	struct CSAFileEntry
	{
		CString	m_sName;
      bool     bIsFolder;
		unsigned int attrib;
		time_t time_create;
		time_t time_access;
		time_t time_write;
		_fsize_t size;

	};
#endif

	typedef vector<CSAFileEntry> SAFileVector;

	SAFileVector &Files()	{return m_files;}

	//////////////////////////////////////////////////////////////////////
public:
	void	FormatPath(CString &path);

protected:

	bool	GetSubDirs(SADirVector &dir_array, const CString &path);
	UINT	FindFiles(const CString & dir, const CString & filter, bool bIncludeFilesInFileList, bool bIncludeFoldersInFileList);
	
	CString	m_sSourceDir;

	SADirVector m_dirs;
	SAFileVector m_files;
};
