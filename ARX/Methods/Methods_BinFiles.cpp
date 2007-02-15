// Methods_BinFiles.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_BinFiles.h"
#include "Methods_ActiveX.h"
#include "ArgumentsRetrieval.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// Methods_BinFiles

int OpenBinFile()
{
	struct resbuf *ListData = NULL;
	CString sFileName;
	CString sArg;
	CFileException Exception;
	acedRetNil();
	
 	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		acedRetNil();
        return 0; 
	}

	// get the file name
	if (ListData->restype == RTSTR)
		sFileName = ListData->resval.rstring;		
	else
        return 0; 		

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	if (ListData == NULL)
        return 0; 		
	
	// get the read or write argument
	if (ListData->restype == RTSTR)
	{
		sArg = ListData->resval.rstring;		
		sArg.MakeLower();
	}
	else
        return 0; 		

	if( sArg == _T("r") )
		sFileName = theWorkspace.FindFile( sFileName );
	
	CLispFileObject *pFile = new CLispFileObject;	

	if (sArg == "w")
	{
		// open the file for write
		if ( !pFile->m_File.Open(sFileName, CFile::modeCreate | CFile::shareExclusive | CFile::modeWrite, &Exception) )
		{
			acedRetNil();
			return 0; 
		}
		pFile->m_pArchive =
			new CArchive(&pFile->m_File, CArchive::store | CArchive::bNoFlushOnDelete, 4096, NULL);
	}
	else if (sArg == "r")
	{
		if (!pFile->m_File.Open(sFileName, CFile::modeRead | CFile::shareDenyWrite, &Exception))
		{
			acedRetNil();
			return 0; 
		}
		pFile->m_pArchive = 
			new CArchive(&pFile->m_File, CArchive::load | CArchive::bNoFlushOnDelete , 4096, NULL);
		
	}
	// set the style flag
	pFile->m_Style = sArg;

	acedRetLong((long)pFile);
	return 0;
}


int WriteBinFile()
{
	struct resbuf *ListData = NULL;
	CLispFileObject *pFile = NULL;
	int nLbCount=0;
	acedRetNil();

 	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
		return 0; 

	// get the file name
	if (ListData->restype == RTLONG)
	{
		if (ListData->resval.rlong > 0)
			pFile = (CLispFileObject*)ListData->resval.rlong;			
		else
			return 0; 		
	}
	else
		return 0; 		

	// iterate forward to the next required argument
	ListData = ListData->rbnext;

	if (ListData == NULL)
		return 0; 		

	while(ListData != NULL)
	{
		*pFile->m_pArchive << ListData->restype;
		// get the file name
		switch (ListData->restype)
		{
		case RTLE:
			nLbCount--;
			break;			
		case RTLB:
			nLbCount++;
			if (nLbCount == 3)
			{
				theWorkspace.DisplayAlert(_T("Lists nested more than two levels deep are not allowed."));
				acedRetNil();
				return 0; 		
			}
			break;			
		case RTNONE:
		case RTVOID:
		case RTDOTE:
		case RTNIL:
		case RTT:
		case RTRESBUF:
			break;
		case RTREAL:
		case RTANG:
			*pFile->m_pArchive << ListData->resval.rreal;		
			break;
		case RTPOINT:
			{
			double dVal;
			dVal = ListData->resval.rpoint[0];		
			*pFile->m_pArchive << dVal;
			dVal = ListData->resval.rpoint[1];		
			*pFile->m_pArchive << dVal;
			break;
			}
		case RTSHORT:
			{
			int nVal = ListData->resval.rint;
			*pFile->m_pArchive << nVal;		
			break;
			}
		case RTSTR:
		case RTDXF0:
			{
			CString sText = ListData->resval.rstring;
			*pFile->m_pArchive << sText;
			break;
			}
		case RTENAME:
		case RTPICKS:
			{
			*pFile->m_pArchive << ListData->resval.rlname[0];		
			*pFile->m_pArchive << ListData->resval.rlname[1];		
			break;
			}
		case RTORINT:
		case RT3DPOINT:
			{
			double dVal;
			dVal = ListData->resval.rpoint[0];		
			*pFile->m_pArchive << dVal;
			dVal = ListData->resval.rpoint[1];		
			*pFile->m_pArchive << dVal;
			dVal = ListData->resval.rpoint[2];		
			*pFile->m_pArchive << dVal;
			
			break;
			}
		case RTLONG:
			*pFile->m_pArchive << ListData->resval.rlong;
			break;
		default:
			{
			return 0; 		
			break;
			}
		}
		// iterate forward to the next required argument
		ListData = ListData->rbnext;
	
		if (ListData == NULL)
		{
			acedRetT();
			return 0; 		
		}	
	}
	
	acedRetT();
    return 0; 		
	
}


int ReadBinFile()
{
	struct resbuf *pReturnRB = NULL;
	struct resbuf *ListData = NULL;
	short restype;
	int nLBCount = 0;

	CLispFileObject *pFile = NULL;

 	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		acedRetNil();
        return 0; 
	}

	// get the file name
	if (ListData->restype == RTLONG)
	{
		if (ListData->resval.rlong > 0)
		{
			pFile = (CLispFileObject*)ListData->resval.rlong;			
		}
		else
		{
			acedRetVoid();
			return 0; 		
		}
	}
	else
	{
		acedRetVoid();
        return 0; 		
	}
	CArchive &ar = *pFile->m_pArchive;

	
	// get the resbuf variable type
	ar >> restype;

	// if the restype is -1 this is the end of the file.
	if (restype == -1 || pFile->m_bFoundEnd == true)
	{
		pFile->m_bFoundEnd = true;
		acedRetVoid();
        return 0; 		
	}


	// if this resbuf is not a list...
	if (restype != RTLB)
	{
		// get the file name
		switch (restype)
		{
		case RTNONE:
			{
			struct resbuf RetVal;
			RetVal.restype = RTNONE;
			acedRetVal(&RetVal);
			break;
			}
		case RTVOID:
			{
			struct resbuf RetVal;
			RetVal.restype = RTVOID;
			acedRetVal(&RetVal);
			break;
			}
		case RTDOTE:			
			{
			struct resbuf RetVal;
			RetVal.restype = RTDOTE;
			acedRetVal(&RetVal);
			break;
			}
		case RTNIL:
			{
			acedRetNil();
			break;
			}
		case RTT:
			{
			acedRetT();
			break;
			}
		case RTRESBUF:			
			{
			struct resbuf RetVal;
			RetVal.restype = RTRESBUF;
			acedRetVal(&RetVal);
			break;
			}
		case RTREAL:
			{
			double dVal;
			ar >> dVal;
			acedRetReal(dVal);
			break;
			}
		case RTANG:
			{
			struct resbuf RetVal;
			double dVal;
			ar >> dVal;			
			RetVal.restype = RTANG;
			RetVal.resval.rreal = dVal;
			acedRetVal(&RetVal);
			break;
			}
		case RTPOINT:
			{
			struct resbuf RetVal;
			ar >> RetVal.resval.rpoint[0];		
			ar >> RetVal.resval.rpoint[1];		
			RetVal.restype = RTPOINT;
			acedRetVal(&RetVal);
			break;
			}
		case RTSHORT:
			{
			int nVal;
			ar >> nVal;
			acedRetInt(nVal);
			break;
			}
		case RTDXF0:
			{
			struct resbuf RetVal;
			CString sText;
			ar >> sText;			
			RetVal.restype = RTDXF0;
			RetVal.resval.rstring = sText.GetBuffer(sText.GetLength());
			//strcpy(RetVal.resval.rstring, sText);
			acedRetVal(&RetVal);
			break;
			}
		case RTSTR:
			{
			CString sText;
			ar >> sText;
			acedRetStr(sText);
			break;
			}
		case RTENAME:
			{
			struct resbuf RetVal;
			ar >> RetVal.resval.rlname[0];		
			ar >> RetVal.resval.rlname[1];		
			RetVal.restype = RTENAME;
			acedRetVal(&RetVal);
			break;
			}
		case RTPICKS:
			{
			struct resbuf RetVal;
			ar >> RetVal.resval.rlname[0];		
			ar >> RetVal.resval.rlname[1];		
			RetVal.restype = RTPICKS;
			acedRetVal(&RetVal);
			break;
			}
		case RTORINT:
			{
			struct resbuf RetVal;
			ar >> RetVal.resval.rpoint[0];		
			ar >> RetVal.resval.rpoint[1];		
			ar >> RetVal.resval.rpoint[2];		
			RetVal.restype = RTORINT;
			acedRetVal(&RetVal);
			break;
			}
		case RT3DPOINT:
			{
			ads_point point;
			double dVal;
			ar >> dVal; point[0] = dVal;		
			ar >> dVal; point[1] = dVal;		
			ar >> dVal; point[2] = dVal;		
			acedRetPoint(point);
			break;
			}
		case RTLONG:
			long lVal;
			ar >> lVal;
			acedRetLong(lVal);
			break;
		}
		return 0;
	}
	else // if it is a list
	{
		struct resbuf *pRBList = NULL, *pRB = NULL, *pRBFinal = NULL;
		nLBCount++;
		bool bFirstCounted = false;
		pRBFinal = acutNewRb(restype);	
		pRBList = pRBFinal;
		while (nLBCount > 0)
		{
			// get the file name
			switch (restype)
			{
			case RTLB:
				{
				if (bFirstCounted == true)
					nLBCount++;
				if (nLBCount > 1)
				{					
					pRB = acutNewRb(restype);	
				}
				bFirstCounted = true;
				break;
				}
			case RTLE:
				{
				nLBCount--;
				pRB = acutNewRb(restype);	
				break;
				}
			case RTNONE:
			case RTVOID:
			case RTDOTE:			
			case RTNIL:
			case RTT:
			case RTRESBUF:			
				{
				pRB = acutNewRb(restype);	
				break;
				}
			case RTREAL:
				{
				double dVal;
				ar >> dVal;
				pRB = acutNewRb(restype);	
				pRB->resval.rreal = dVal;
				break;
				}
			case RTANG:
				{
				double dVal;
				ar >> dVal;			
				pRB = acutNewRb(restype);	
				pRB->resval.rreal = dVal;				
				break;
				}
			case RTPOINT:
				{
				pRB = acutNewRb(restype);					
				ar >> pRB->resval.rpoint[0];		
				ar >> pRB->resval.rpoint[1];		
				break;
				}
			case RTSHORT:
				{
				int nVal;
				ar >> nVal;
				pRB = acutNewRb(restype);	
				pRB->resval.rint = nVal;
				
				break;
				}
			case RTDXF0:
			case RTSTR:
				{
				CString sText;
				ar >> sText;			
				pRB = acutNewRb(restype);	
				acutNewString(sText, pRB->resval.rstring);	
				break;
				}
			case RTENAME:
			case RTPICKS:
				{
				pRB = acutNewRb(restype);	
				ar >> pRB->resval.rlname[0];		
				ar >> pRB->resval.rlname[1];		
				break;
				}
			case RTORINT:
			case RT3DPOINT:
				{
				pRB = acutNewRb(restype);	
				ar >> pRB->resval.rpoint[0];		
				ar >> pRB->resval.rpoint[1];		
				ar >> pRB->resval.rpoint[2];		
				break;
				}
			case RTLONG:
				long lVal;
				ar >> lVal;
				pRB = acutNewRb(restype);	
				pRB->resval.rlong = lVal;
				break;
			}			
			if (pRB != NULL) 				
			{
				if (restype == RTLB && nLBCount == 1)
				{
					// nothing
				}
				else
				{
					// Build up the linked list
					// Remember that pRBList still points
					// to the beginning of the liked list.
					pRBList->rbnext = pRB;
					pRBList = pRB;
				}
			}
			
			// get the resbuf variable type
			ar >> restype;
		}
		acedRetList(pRBFinal);
		return 0;
	}

	acedRetNil();	
    return -1; 		
	
}

int CloseBinFile()
{
	struct resbuf *ListData = NULL;
	CFileException Exception;
	CLispFileObject *pFile = NULL;

 	//ensure AutoLisp has passed Arguments	
	if ((ListData = acedGetArgs()) == NULL) 
	{
		acedRetNil();
        return 0; 
	}

	// get the file name
	if (ListData->restype == RTLONG)
	{
		if (ListData->resval.rlong > 0)
		{
			pFile = (CLispFileObject*)ListData->resval.rlong;		
		}
		else
		{
			acedRetVoid();
			return 0; 		
		}
	}
	else if (ListData->restype == RTSHORT)
	{
		if (ListData->resval.rlong > 0)
		{
			pFile = (CLispFileObject*)ListData->resval.rint;
		}
		else
		{
			acedRetVoid();
			return 0; 		
		}
	}
	else
	{
		acedRetVoid();
        return 0; 		
	}

	if (pFile->m_Style == "w")
	{
		int nEndOfFile = -1;
		*pFile->m_pArchive << nEndOfFile;
	}
	pFile->m_pArchive->Close();
	delete pFile->m_pArchive;
	pFile->m_File.Close();
	delete pFile;

	acedRetVoid();
	return 0;
}