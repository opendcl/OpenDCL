// SADirRead.cpp: implementation of the CSADirRead class.
//

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

/**********************************************************************

  Sample use:
 
#include "SADirRead.h"

...

	CSADirRead dr;

	dr.Recurse() = bScanSub;    // scan subdirs ?

	dr.ClearDirs();         // start clean
	dr.GetDirs("c:\\temp"); // get all folders under c:\temp

	// dump the directory list to the debug trace window:

	// get the dir array
	CSADirRead::SADirVector &dirs = dr.Dirs();

	// loop over it
	for (CSADirRead::SADirVector::const_iterator dit = dirs.begin(); dit!=dirs.end(); dit++)
	{
		TRACE("%s\n", (*dit).m_sName);
	}

	dr.ClearFiles();        // start clean

	dr.GetFiles("*.jpg");   // get all *.JPG files in c:\temp and below

	// get the file array
	CSADirRead::SAFileVector &files = dr.Files();   

	// dump it...
	for (CSADirRead::SAFileVector::const_iterator fit = files.begin(); fit!=files.end(); fit++)
	{
		TRACE("%s\n", (*fit).m_sName);
	}

	dr.ClearFiles();		// start clean, again

	dr.GetFiles("*.txt");	// get all *.txt files in c:\temp and below

	// dump them, too
	for (fit = files.begin(); fit!=files.end(); fit++)
	{
		TRACE("%s : %d bytes\n", (*fit).m_sName, (*fit).size);
	}

**********************************************************************/

#include "stdafx.h"
#include "SADirRead.h"
//#include <io.h>
//#include <errno.h>
//#include <sys/types.h>
//#include <sys/stat.h>

#include <algorithm>


// some helper functions we'll use here
bool asc_alpha_dir_sort(const CSADirRead::CSADirEntry &a, const CSADirRead::CSADirEntry &b);
bool asc_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool asc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool asc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool dsc_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool dsc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);
bool dsc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b);

const TCHAR *sBackSlash = _T("\\");
const TCHAR sForwardSlash = _T('/');
const TCHAR sDot[] = _T(".");
const TCHAR sDotDot[] = _T("..");
const TCHAR sFileFilter[] = _T("\\*");


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSADirRead::CSADirRead()
{
}

//////////////////////////////////////////////////////////////////////

CSADirRead::~CSADirRead()
{

}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::GetDirs(const TCHAR * pDirPath, bool bRecurse)
{
	m_sSourceDir = pDirPath;

	FormatPath(m_sSourceDir);

	if (m_sSourceDir.GetLength() == 0)
		return false;

	// find tree starting from pDirPath
	if (bRecurse)
	{
		GetSubDirs(m_dirs, m_sSourceDir);
	}

	// add the main directory. 
	m_dirs.push_back(CSADirEntry(m_sSourceDir));

	return true;	
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::GetSubDirs(vector<CSADirRead::CSADirEntry> &dir_array, const CString &path)
{
	CString newPath;

	CString searchString;
	searchString = path;
	searchString+= sFileFilter;

   try 
   {
#ifndef USE_WIN32_FINDFILE
      struct _finddata_t  c_file;
      long fhandle;

      if ((fhandle=_findfirst( searchString, &c_file ))!=-1) 
      {
         // we only care about subdirs
         if ((c_file.attrib & _A_SUBDIR)==_A_SUBDIR) 
         {
            // add c_file.name to the string array
            
            // we'll handle parents on our own
            if ((_tcscmp(c_file.name, sDot)!=0) && (_tcscmp(c_file.name, sDotDot)!=0)) 
            {
               newPath = path;
               newPath+= sBackSlash;
               newPath+= c_file.name;
               //GetSubDirs(dir_array, newPath);
               
               dir_array.push_back(newPath);
            }
         }
         
         // find the rest of them	
         while(_findnext( fhandle, &c_file ) == 0 ) 
         {
            
            if ((c_file.attrib & _A_SUBDIR)==_A_SUBDIR) 
            {
               // we'll handle parents on our own
               if ((_tcscmp(c_file.name, sDot)!=0) && (_tcscmp(c_file.name, sDotDot)!=0)) 
               {
                  newPath = path;
                  newPath+= sBackSlash;
                  newPath+= c_file.name;
                  //GetSubDirs(dir_array, newPath);
                  dir_array.push_back(newPath);
               }
            }
         }
         _findclose(fhandle);
      }
#else
      WIN32_FIND_DATA FindFileData;
      HANDLE hFind;
      if ((hFind = FindFirstFile(searchString, &FindFileData))!=INVALID_HANDLE_VALUE)
      {
         // we only care about files, not subdirs
         if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY) 
         {
            // we'll handle parents on our own
            if ((_tcscmp(FindFileData.cFileName, sDot)!=0) && (_tcscmp(FindFileData.cFileName, sDotDot)!=0)) 
            {
               newPath = path;
               newPath+= sBackSlash;
               newPath+=FindFileData.cFileName;
               //GetSubDirs(dir_array, newPath);
               
               dir_array.push_back(newPath);
            }
         }
         
         // find the rest of them	
         while(FindNextFile( hFind, &FindFileData )) 
         {
            // we only care about files, not subdirs
            if ((FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY) 
            {
               // we'll handle parents on our own
               if ((_tcscmp(FindFileData.cFileName, sDot)!=0) && (_tcscmp(FindFileData.cFileName, sDotDot)!=0)) 
               {
                  newPath = path;
                  newPath+= sBackSlash;
                  newPath+=FindFileData.cFileName;
                 // GetSubDirs(dir_array, newPath);
                  
                  dir_array.push_back(newPath);
               }
               
            }
         }
      }
      FindClose(hFind);
#endif
   } 
   catch (...) 
   {
      return false;
   }
   
   return true;
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::ClearFiles()
{
   m_files.clear();
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::ClearDirs()
{
	m_dirs.clear();
	return true;
}

//////////////////////////////////////////////////////////////////////

bool CSADirRead::GetFiles(const TCHAR *pFilemask, bool bIncludeFilesInFileList, bool bIncludeFoldersInFileList)
{
	/*
	// get the files in each of our directories
	for (vector<CSADirEntry>::iterator it = m_dirs.begin();it!=m_dirs.end(); it++) 
	{
		CString curDir = (*it).m_sName;

      // sanity check
		if (curDir.IsEmpty())
		{
			continue;
		}
		
		if (!FindFiles(curDir, pFilemask, bIncludeFilesInFileList, bIncludeFoldersInFileList))
		{
			return false;
		}
	}
	*/
	if (!FindFiles(m_sSourceDir, pFilemask, bIncludeFilesInFileList, bIncludeFoldersInFileList))
	{
		return false;
	}
	return true;
}	

//////////////////////////////////////////////////////////////////////

bool CSADirRead::SortFiles(int iSortStyle, bool bReverse)
{
	switch (iSortStyle)
	{
	default:
		break;
	case eSortAlpha:
		std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_alpha_file_sort : asc_alpha_file_sort));
		break;
	case eSortWriteDate:
		std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_date_file_sort : asc_date_file_sort));
		break;
	case eSortSize:
		std::sort(m_files.begin(), m_files.end(), (bReverse ? dsc_size_file_sort : asc_size_file_sort));
		break;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////

// chop off trailing 
void CSADirRead::FormatPath(CString &path)
{
	CString inPath = path;
	inPath.TrimRight();
	CString tmp;
	
	int iLastSlashPos = inPath.ReverseFind(*sBackSlash);
	if (iLastSlashPos == -1)
	{
		iLastSlashPos = inPath.ReverseFind(sForwardSlash);
	}
	
	if (iLastSlashPos!=-1) 
	{
		if (iLastSlashPos==inPath.GetLength()-1) 
		{
			path = inPath.Left(iLastSlashPos);

			FormatPath(path); // in case the incoming path is "C:\temp\\\..."
		}
		/*
		else
		{
			path = inPath.Mid(iLastSlashPos+1);
		}
		*/
	} 
}

//////////////////////////////////////////////////////////////////////

UINT CSADirRead::FindFiles(const CString & dir, const CString & filter, bool bIncludeFilesInFileList, bool bIncludeFoldersInFileList)
{
	// make sure the path ends in a single "\"
	CString baseName = dir;
	FormatPath(baseName);
	baseName+=sBackSlash;

	CString fullPath = baseName;
	fullPath += filter;

	CString fileName;

	// find first file in current directory
#ifndef USE_WIN32_FINDFILE
	struct _finddata_t  c_file;
	long fhandle;

	try 
	{
		if ((fhandle=_findfirst( fullPath, &c_file ))!=-1) 
		{
         bool bIsFolder = (c_file.attrib & _A_SUBDIR)==_A_SUBDIR;

         bool bAddThisOne = (bIsFolder && bIncludeFoldersInFileList) || (!bIsFolder && bIncludeFilesInFileList);

         // skip . and ..
         if (bIsFolder && (_tcscmp(c_file.name, sDot)==0) || (_tcscmp(c_file.name, sDotDot)==0))
         {
            bAddThisOne = false;
         }

         if (bAddThisOne) 
			{
				fileName = baseName;
				fileName += c_file.name;

				CSAFileEntry t;
            t.bIsFolder = bIsFolder;
            t.attrib = c_file.attrib;
				t.m_sName = fileName;
				t.time_write = c_file.time_write;
				t.time_create = c_file.time_create;
            t.size = c_file.size;
				m_files.push_back(t);
			}

			// find the rest of them	
			while(_findnext( fhandle, &c_file ) == 0 ) 
			{
            bool bIsFolder = (c_file.attrib & _A_SUBDIR)==_A_SUBDIR;
            bool bAddThisOne = (bIsFolder && bIncludeFoldersInFileList) || (!bIsFolder && bIncludeFilesInFileList);

            // skip . and ..
            if (bIsFolder && (_tcscmp(c_file.name, sDot)==0) || (_tcscmp(c_file.name, sDotDot)==0))
            {
               bAddThisOne = false;
            }

			   if (bAddThisOne) 
				{
					fileName=baseName;
					fileName += c_file.name;

					CSAFileEntry t;
               t.bIsFolder = bIsFolder;
               t.attrib = c_file.attrib;
					t.m_sName = fileName;
					t.time_write = c_file.time_write;
					t.time_create = c_file.time_create;
               t.size = c_file.size;
					m_files.push_back(t);
				}
			}
			_findclose(fhandle);
		}
	} 
	catch (...) 
	{
		return false;
	}
#else
   WIN32_FIND_DATA FindFileData;
   HANDLE hFind;
	try 
	{
		if ((hFind = FindFirstFile(fullPath, &FindFileData))!=INVALID_HANDLE_VALUE)
		{
         bool bIsFolder = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY;

         bool bAddThisOne = (bIsFolder && bIncludeFoldersInFileList) || ((!bIsFolder) && bIncludeFilesInFileList);

         // skip . and ..
         if (bIsFolder && (_tcscmp(FindFileData.cFileName, sDot)==0) || (_tcscmp(FindFileData.cFileName, sDotDot)==0))
         {
            bAddThisOne = false;
         }

			if (bAddThisOne) 
         {
				fileName = baseName;
				fileName += FindFileData.cFileName;

				CSAFileEntry t;
				t.m_sName = fileName;

            t.bIsFolder = bIsFolder;
            t.attrib = FindFileData.dwFileAttributes;

            ASSERT(sizeof(FindFileData.ftLastWriteTime)== sizeof(_FILETIME));
				memcpy(&t.time_write, &FindFileData.ftLastWriteTime, sizeof(_FILETIME));
				
            ASSERT(sizeof(FindFileData.ftLastWriteTime)== sizeof(_FILETIME));
				memcpy(&t.time_create, &FindFileData.ftCreationTime, sizeof(_FILETIME));

            t.size = ((unsigned __int64)FindFileData.nFileSizeHigh * ((unsigned __int64)MAXDWORD+1)) + (unsigned __int64)FindFileData.nFileSizeLow;
				m_files.push_back(t);
			}

			// find the rest of them	
			while (FindNextFile(hFind, &FindFileData))
			{
            bool bIsFolder = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY;

            bool bAddThisOne = (bIsFolder && bIncludeFoldersInFileList) || ((!bIsFolder) && bIncludeFilesInFileList);

            // skip . and ..
            if (bIsFolder && (_tcscmp(FindFileData.cFileName, sDot)==0) || (_tcscmp(FindFileData.cFileName, sDotDot)==0))
            {
               bAddThisOne = false;
            }

			   if (bAddThisOne) 
			   {
				   fileName = baseName;
				   fileName += FindFileData.cFileName;

				   CSAFileEntry t;
				   t.m_sName = fileName;
               
               t.bIsFolder = bIsFolder;
               t.attrib = FindFileData.dwFileAttributes;

               ASSERT(sizeof(FindFileData.ftLastWriteTime)== sizeof(_FILETIME));
				   memcpy(&t.time_write, &FindFileData.ftLastWriteTime, sizeof(_FILETIME));
				   
               ASSERT(sizeof(FindFileData.ftLastWriteTime)== sizeof(_FILETIME));
				   memcpy(&t.time_create, &FindFileData.ftCreationTime, sizeof(_FILETIME));

               t.size = ((unsigned __int64)FindFileData.nFileSizeHigh * ((unsigned __int64)MAXDWORD+1)) + (unsigned __int64)FindFileData.nFileSizeLow;
				   m_files.push_back(t);
			   }
			}
			FindClose(hFind);
		}
	} 
	catch (...) 
	{
		return false;
	}
#endif

	return true;
}

//////////////////////////////////////////////////////////////////////

bool asc_alpha_dir_sort(const CSADirRead::CSADirEntry &a, const CSADirRead::CSADirEntry &b)
{
	return (_tcsicmp(a.m_sName, b.m_sName) < 0);
}

//////////////////////////////////////////////////////////////////////

bool asc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (_tcsicmp(a.m_sName, b.m_sName) < 0);
}

//////////////////////////////////////////////////////////////////////

bool asc_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
#ifndef USE_WIN32_FINDFILE
	return (difftime(a.time_write, b.time_write) < 0);
#else
   return (a.time_write < b.time_write);
#endif
}

//////////////////////////////////////////////////////////////////////

bool asc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (a.size < b.size);
}

//////////////////////////////////////////////////////////////////////

bool dsc_alpha_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (!asc_alpha_file_sort(a,b));
}

//////////////////////////////////////////////////////////////////////

bool dsc_date_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (!asc_date_file_sort(a,b));
}

//////////////////////////////////////////////////////////////////////

bool dsc_size_file_sort(const CSADirRead::CSAFileEntry &a, const CSADirRead::CSAFileEntry &b)
{
	return (!asc_size_file_sort(a,b));
}
