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
    if ( (ListData = acedGetArgs() ) == NULL ) 
    {
        acedRetNil();
        return 0; 
    }

    // get the file name
    if ( ListData->restype == RTSTR )
        sFileName = ListData->resval.rstring;		
    else
        return 0; 		

    // iterate forward to the next required argument
    ListData = ListData->rbnext;

    if ( ListData == NULL )
        return 0; 		

    // get the read or write argument
    if ( ListData->restype == RTSTR )
    {
        sArg = ListData->resval.rstring;		
        sArg.MakeLower();
    }
    else
        return 0; 		
    //file name
    if( sArg == _T("r") )
        sFileName = theWorkspace.FindFile( sFileName );

    CLispFileObject *pFile = new CLispFileObject;	

    if ( sArg == "w" )
    {
        // open the file for write
        if ( !pFile->m_File.Open(sFileName, CFile::modeCreate | 
            CFile::shareExclusive | CFile::modeWrite, &Exception) )
        {
            acedRetNil();
            return 0; 
        }
        pFile->m_pArchive = new CArchive(&pFile->m_File, CArchive::store | 
            CArchive::bNoFlushOnDelete, 4096, NULL);
    }
    else if ( sArg == "r" ) 
    {
        if (!pFile->m_File.Open(sFileName, CFile::modeRead | 
            CFile::shareDenyWrite, &Exception))
        {
            acedRetNil();
            return 0; 
        }
        pFile->m_pArchive =  new CArchive(&pFile->m_File, CArchive::load | 
            CArchive::bNoFlushOnDelete , 4096, NULL);
    }
    // set the style flag
    pFile->m_Style = sArg;

    //set ISOpen
    pFile->m_bIsOpen = true;

    acedRetPointer( pFile, odcl::ptrBinFile );
    return 0;
}

int WriteBinFile()
{
    struct resbuf *ListData = NULL;
    CLispFileObject *pFile = NULL;

    //
    int nVal;
    double dVal;
    CString sText;

    try
    {
        //ensure AutoLisp has passed Arguments	
        if ( (ListData = acedGetArgs() ) == NULL ) 
        {
            acedRetNil();
            return 0; 
        }

        // get the file name
        if ( ListData->restype == RTLONG )
        {
            if ( ListData->resval.rlong > 0 )
                pFile = (CLispFileObject*)ListData->resval.rlong;			
            else
            {
                acedRetNil();
                return 0; 	
            }
        }
        else if ( ListData->restype == RTENAME )
        {
            if ( ListData->resval.rlname[0] > 0 )
                pFile = (CLispFileObject*)ListData->resval.rlname[0];			
            else
            {
                acedRetNil();
                return 0; 	
            }
        }
        else
        {
            acedRetNil();
            return 0; 	
        }

        // iterate forward to the next required argument
        ListData = ListData->rbnext;

        if ( ListData == NULL )
            return 0; 		

        while( ListData != NULL )
        {
            if( 4999 < ListData->restype && 5024 > ListData->restype )
            {
                switch ( ListData->restype )
                {
                case RTLE:	
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTLB:
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTNONE:
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTVOID:
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTDOTE:
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTNIL:
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTT:
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTRESBUF:
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTREAL:
                    *pFile->m_pArchive << ListData->restype;
                    *pFile->m_pArchive << ListData->resval.rreal;
                    break;
                case RTANG:
                    *pFile->m_pArchive << ListData->restype;
                    *pFile->m_pArchive << ListData->resval.rreal;		
                    break;
                case RTPOINT:
                    *pFile->m_pArchive << ListData->restype;
                    dVal = ListData->resval.rpoint[0];		
                    *pFile->m_pArchive << dVal;
                    dVal = ListData->resval.rpoint[1];		
                    *pFile->m_pArchive << dVal;
                    break;
                case RTSHORT:
                    *pFile->m_pArchive << ListData->restype;
                    nVal = ListData->resval.rint;
                    *pFile->m_pArchive << nVal;		
                    break;
                case RTSTR:
                    *pFile->m_pArchive << ListData->restype;
                    sText = ListData->resval.rstring;
                    *pFile->m_pArchive << sText;
                    break;
                case RTDXF0:
                    *pFile->m_pArchive << ListData->restype;
                    sText = ListData->resval.rstring;
                    *pFile->m_pArchive << sText;
                    break;
                case RTENAME:
                    *pFile->m_pArchive << ListData->restype;
                    *pFile->m_pArchive << ListData->resval.rlname[0];		
                    *pFile->m_pArchive << ListData->resval.rlname[1];	
                case RTPICKS:
                    *pFile->m_pArchive << ListData->restype;
                    *pFile->m_pArchive << ListData->resval.rlname[0];		
                    *pFile->m_pArchive << ListData->resval.rlname[1];		
                    break;
                case RTORINT:
                    *pFile->m_pArchive << ListData->restype;
                    dVal = ListData->resval.rpoint[0];		
                    *pFile->m_pArchive << dVal;
                    dVal = ListData->resval.rpoint[1];		
                    *pFile->m_pArchive << dVal;
                    dVal = ListData->resval.rpoint[2];		
                    *pFile->m_pArchive << dVal;
                    break;
                case RT3DPOINT:
                    *pFile->m_pArchive << ListData->restype;
                    dVal = ListData->resval.rpoint[0];		
                    *pFile->m_pArchive << dVal;
                    dVal = ListData->resval.rpoint[1];		
                    *pFile->m_pArchive << dVal;
                    dVal = ListData->resval.rpoint[2];		
                    *pFile->m_pArchive << dVal;
                    break;
                case RTLONG:
                    *pFile->m_pArchive << ListData->restype;
                    *pFile->m_pArchive << ListData->resval.rlong;
                    break;
                }
            }
            else if( 1072 > ListData->restype && -6 < ListData->restype /* && ListData->restype != 0 */)
            {
                short xDataType = acdbGroupCodeToType( ListData->restype );

                if (xDataType != RTERROR)
                {
                    switch (xDataType)
                    {
                    case kDwgNull:
                        *pFile->m_pArchive << ListData->restype;
                        break;
                    case kDwgBChunk:
                        *pFile->m_pArchive << ListData->restype;
                        break;
                    case kDwgNotRecognized:
                        *pFile->m_pArchive << ListData->restype;
                        break;
                    case kDwgReal:
                        *pFile->m_pArchive << ListData->restype;
                        *pFile->m_pArchive << ListData->resval.rreal;		
                        break;
                    case kDwgInt16:
                        *pFile->m_pArchive << ListData->restype;
                        nVal = ListData->resval.rint;
                        *pFile->m_pArchive << nVal;		
                        break;
                    case kDwgInt8:
                        *pFile->m_pArchive << ListData->restype;
                        nVal = ListData->resval.rint;
                        *pFile->m_pArchive << nVal;		
                        break;
                    case kDwgText:
                        *pFile->m_pArchive << ListData->restype;
                        sText = ListData->resval.rstring;
                        *pFile->m_pArchive << sText;
                        break;
                    case kDwgHandle:
                        *pFile->m_pArchive << ListData->restype;
                        sText = ListData->resval.rstring;
                        *pFile->m_pArchive << sText;
                        break;
                    case kDwgHardOwnershipId:
                        *pFile->m_pArchive << ListData->restype;
                        *pFile->m_pArchive << ListData->resval.rlname[0];		
                        *pFile->m_pArchive << ListData->resval.rlname[1];		
                        break;
                    case kDwgSoftOwnershipId:
                        *pFile->m_pArchive << ListData->restype;
                        *pFile->m_pArchive << ListData->resval.rlname[0];		
                        *pFile->m_pArchive << ListData->resval.rlname[1];		
                        break;
                    case kDwgHardPointerId:
                        *pFile->m_pArchive << ListData->restype;
                        *pFile->m_pArchive << ListData->resval.rlname[0];		
                        *pFile->m_pArchive << ListData->resval.rlname[1];		
                        break;
                    case kDwgSoftPointerId:
                        *pFile->m_pArchive << ListData->restype;
                        *pFile->m_pArchive << ListData->resval.rlname[0];		
                        *pFile->m_pArchive << ListData->resval.rlname[1];		
                        break;
                    case kDwg3Real:
                        *pFile->m_pArchive << ListData->restype;
                        dVal = ListData->resval.rpoint[0];		
                        *pFile->m_pArchive << dVal;
                        dVal = ListData->resval.rpoint[1];		
                        *pFile->m_pArchive << dVal;
                        dVal = ListData->resval.rpoint[2];		
                        *pFile->m_pArchive << dVal;
                        break;
                    case kDwgInt32:
                        *pFile->m_pArchive << ListData->restype;
                        *pFile->m_pArchive << ListData->resval.rlong;
                        break;
                    }
                }
            }

            // iterate forward to the next required argument
            ListData = ListData->rbnext;

            //Test if we are at the end
            if (ListData == NULL)
            {
                //write our EOL marker
                *pFile->m_pArchive << ArchiveEOL;

                acedRetT();
                return 0; 		
            }	
        }

        acedRetT();
        return 0;
    }
    catch(...)//you never know
    {
        acedRetNil();
        acutRelRb(ListData);
        return 0;
    }
}

int ReadBinFile()
{

    struct resbuf *ListData = NULL;
    struct resbuf *pRbHead = acutNewRb( RTT );
    struct resbuf *pRbTail; 
    pRbTail = pRbHead;  //I don't want to loose my head, add to the tail

    //
    short restype;
    short EOREAD = ArchiveEOF;
    CLispFileObject *pFile = NULL;

    //
    int nVal;
    long lVal;
    double dVal;
    CString sText;

    try
    {
        //ensure AutoLisp has passed Arguments	
        if ( (ListData = acedGetArgs() ) == NULL ) 
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
        else if ( ListData->restype == RTENAME )
        {
            if ( ListData->resval.rlname[0] > 0 )
                pFile = (CLispFileObject*)ListData->resval.rlname[0];			
            else
            {
                acedRetNil();
                return 0; 	
            }
        }
        else
        {
            acedRetVoid();
            return 0; 		
        }

        if ((ListData = ListData->rbnext) != NULL)
        {
            if(ListData->restype == RTT)
            {
                EOREAD  = ArchiveEOL;
            }
            else
            {
                EOREAD  = ArchiveEOF;
            }
        }

        CArchive &ar = *pFile->m_pArchive;

        // get the resbuf variable type
        ar >> restype;

        while ( restype != EOREAD || pFile->m_bFoundEnd != true ) 
        {
            if( restype != ArchiveEOF )
            {
                if(4999 < restype && 5024 > restype)
                {
                    switch (restype)
                    {
                    case RTLE:
                    case RTLB:
                    case RTNONE:
                    case RTVOID:
                    case RTDOTE:			
                    case RTNIL:
                    case RTT:
                    case RTRESBUF:		
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);
                        break;
                    case RTREAL:
                        ar >> dVal;
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);
                        pRbTail->resval.rreal = dVal;
                        break;
                    case RTANG:
                        ar >> dVal;			
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);
                        pRbTail->resval.rreal = dVal;
                        break;
                    case RTPOINT:
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);				
                        ar >> pRbTail->resval.rpoint[0];		
                        ar >> pRbTail->resval.rpoint[1];
                        break;
                    case RTSHORT:
                        ar >> nVal;
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);	
                        pRbTail->resval.rint = nVal;
                        break;
                    case RTDXF0:
                    case RTSTR:
                        ar >> sText;			
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);
                        acutNewString(sText, pRbTail->resval.rstring);
                        break;
                    case RTENAME:
                    case RTPICKS:
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);	
                        ar >> pRbTail->resval.rlname[0];		
                        ar >> pRbTail->resval.rlname[1];
                        break;
                    case RTORINT:
                    case RT3DPOINT:
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);
                        ar >> pRbTail->resval.rpoint[0];		
                        ar >> pRbTail->resval.rpoint[1];		
                        ar >> pRbTail->resval.rpoint[2];
                        break;
                    case RTLONG:
                        ar >> lVal;
                        pRbTail = pRbTail->rbnext = acutNewRb(restype);	
                        pRbTail->resval.rlong = lVal;
                        break;
                    }
                }
                else if ( 1072 > restype && -6 < restype )
                {
                    short xDataType = acdbGroupCodeToType(restype);
                    if (xDataType != RTERROR)
                    {
                        switch (xDataType)
                        {
                        case kDwgReal:
                            ar >> dVal;
                            pRbTail = pRbTail->rbnext = acutNewRb(restype);	
                            pRbTail->resval.rreal = dVal;
                            break;
                        case kDwgInt16:
                        case kDwgInt8:
                            ar >> nVal;
                            pRbTail = pRbTail->rbnext = acutNewRb(restype);	
                            pRbTail->resval.rint = nVal;
                            break;
                        case kDwgText:
                        case kDwgHandle:
                            ar >> sText;			
                            pRbTail = pRbTail->rbnext = acutNewRb(restype);	
                            acutNewString(sText, pRbTail->resval.rstring);
                            break;
                        case kDwgHardOwnershipId:
                        case kDwgSoftOwnershipId:
                        case kDwgHardPointerId:
                        case kDwgSoftPointerId:
                            pRbTail = pRbTail->rbnext = acutNewRb(restype);	
                            ar >> pRbTail->resval.rlname[0];		
                            ar >> pRbTail->resval.rlname[1];
                            break;
                        case kDwg3Real:
                            pRbTail = pRbTail->rbnext = acutNewRb(restype);
                            ar >> pRbTail->resval.rpoint[0];		
                            ar >> pRbTail->resval.rpoint[1];		
                            ar >> pRbTail->resval.rpoint[2];
                            break;
                        case kDwgInt32:
                            ar >> lVal;
                            pRbTail = pRbTail->rbnext = acutNewRb(restype);	
                            pRbTail->resval.rlong = lVal;
                            break;
                        case kDwgNull:
                            pRbTail = pRbTail->rbnext = acutNewRb(restype);
                            break;
                        case kDwgBChunk:
                        case kDwgNotRecognized:
                            pRbTail = pRbTail->rbnext = acutNewRb(restype);
                            break;
                        }
                    }
                }

                ar >> restype ;

                if (restype == EOREAD)
                    pFile->m_bFoundEnd = true;
            }
            else
            {
                acedRetNil();
                return 0; 
            }
        }
        if ( pRbHead->rbnext != NULL)
        {
            acedRetList(pRbHead->rbnext);
            acutRelRb(pRbHead);//
            return 0;
        }
        else
        {
            acedRetNil();
            return 0; 
        }
    }
    catch(...)
    {
        acedRetNil();
        acutRelRb(pRbHead);
        acutRelRb(ListData);
        return 0; 
    }
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
            pFile = (CLispFileObject*)ListData->resval.rlong;		
        else
        {
            acedRetNil();
            return 0; 		
        }
    }
    else if ( ListData->restype == RTENAME )
    {
        if ( ListData->resval.rlname[0] > 0 )
            pFile = (CLispFileObject*)ListData->resval.rlname[0];			
        else
        {
            acedRetNil();
            return 0; 	
        }
    }
    else if (ListData->restype == RTSHORT)
    {
        if (ListData->resval.rlong > 0)
            pFile = (CLispFileObject*)ListData->resval.rint;
        else
        {
            acedRetNil();
            return 0; 		
        }
    }
    else
    {
        acedRetNil();
        return 0; 		
    }

    //test if the file is opened
    if (pFile->m_bIsOpen == true)
    {
        if (pFile->m_Style == "w") 
        {
            //write our EOF marker
            //three strikes your out
            *pFile->m_pArchive << ArchiveEOF;
            *pFile->m_pArchive << ArchiveEOF;
            *pFile->m_pArchive << ArchiveEOF;

            pFile->m_pArchive->Close();
            delete pFile->m_pArchive;

            pFile->m_File.Close();
            delete pFile;

            //Set flag 
            pFile->m_bIsOpen = false;

            //Success 
            acedRetT();
            return 0;
        }
        else // file was opened "r"
        {
            pFile->m_pArchive->Close();
            delete pFile->m_pArchive;

            pFile->m_File.Close();
            delete pFile;

            //Set flag 
            pFile->m_bIsOpen = false;


            //Success
            acedRetT();
            return 0;
        }
    }
    else 
    {
        acedRetNil();
        return 0;
    }
}

int CheckBinFile()
{
    struct resbuf *ListData = NULL;
    CLispFileObject *pFile = NULL;
    if ((ListData = acedGetArgs()) == NULL) 
    {
        acedRetNil();
        return 0; 
    }

    // get the file handle
    if (ListData->restype == RTLONG)
    {
        if (ListData->resval.rlong > 0)
            pFile = (CLispFileObject*)ListData->resval.rlong;			
        else
        {
            acedRetNil();
            return 0; 		
        }
    }
    else if ( ListData->restype == RTENAME )
    {
        if ( ListData->resval.rlname[0] > 0 )
            pFile = (CLispFileObject*)ListData->resval.rlname[0];			
        else
        {
            acedRetNil();
            return 0; 	
        }
    }
    else
    {
        acedRetNil();
        return 0; 		
    }
    // if there is an error state
    if (pFile->m_bFoundError == true)
    {
        acedRetInt( -1 );
        return 0;
    }
    // if this is the end of the file.
    if (pFile->m_bFoundEnd == true)
    {
        acedRetInt( 1 );
        return 0;
    }
    acedRetInt( 0 );
    return 0;
}
