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

    //ensure AutoLISP has passed Arguments	
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

    if( sArg == _T("r") )
        sFileName = theWorkspace.FindFile( sFileName );

    CLispFileObject *pFile = new CLispFileObject;	

    if ( sArg == "w" )
    {
        // open the file for write
        if ( !pFile->m_File.Open(sFileName, CFile::modeCreate | CFile::shareExclusive | CFile::modeWrite, &Exception) )
        {
            acedRetNil();
            return 0; 
        }
        pFile->m_pArchive = new CArchive(&pFile->m_File, CArchive::store | CArchive::bNoFlushOnDelete, 4096, NULL);
    }
    else if ( sArg == "r" ) 
    {
        if (!pFile->m_File.Open(sFileName, CFile::modeRead | CFile::shareDenyWrite, &Exception))
        {
            acedRetNil();
            return 0; 
        }
        pFile->m_pArchive =  new CArchive(&pFile->m_File, CArchive::load | CArchive::bNoFlushOnDelete , 4096, NULL);

    }
    // set the style flag
    pFile->m_Style = sArg;

    //set ISOpen
    pFile->m_bIsOpen = true;

    acedRetLong( (long)pFile );
    return 0;
}


int WriteBinFile()
{
    struct resbuf *ListData = NULL;
    CLispFileObject *pFile = NULL;

    try
    {
        //ensure AutoLISP has passed Arguments	
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
            //so we don't have to call acdbGroupCodeToType() on
            //normal lists
            if( 5000 < ListData->restype )
            {
                // get the file name
                switch ( ListData->restype )
                {
                case RTLE:			
                case RTLB:		
                case RTNONE:
                case RTVOID:
                case RTDOTE:
                case RTNIL:
                case RTT:
                case RTRESBUF:
                    *pFile->m_pArchive << ListData->restype;
                    break;
                case RTREAL:
                case RTANG:
                    *pFile->m_pArchive << ListData->restype;
                    *pFile->m_pArchive << ListData->resval.rreal;		
                    break;
                case RTPOINT:
                    {
                        *pFile->m_pArchive << ListData->restype;

                        double dVal;
                        dVal = ListData->resval.rpoint[0];		
                        *pFile->m_pArchive << dVal;
                        dVal = ListData->resval.rpoint[1];		
                        *pFile->m_pArchive << dVal;
                        break;
                    }
                case RTSHORT:
                    {
                        *pFile->m_pArchive << ListData->restype;

                        int nVal = ListData->resval.rint;
                        *pFile->m_pArchive << nVal;		
                        break;
                    }
                case RTSTR:
                case RTDXF0:
                    {
                        *pFile->m_pArchive << ListData->restype;

                        CString sText = ListData->resval.rstring;
                        *pFile->m_pArchive << sText;
                        break;
                    }
                case RTENAME:
                case RTPICKS:
                    {
                        *pFile->m_pArchive << ListData->restype;

                        *pFile->m_pArchive << ListData->resval.rlname[0];		
                        *pFile->m_pArchive << ListData->resval.rlname[1];		
                        break;
                    }
                case RTORINT:
                case RT3DPOINT:
                    {
                        *pFile->m_pArchive << ListData->restype;

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
                    *pFile->m_pArchive << ListData->restype;
                    *pFile->m_pArchive << ListData->resval.rlong;
                    break;
                }
            }
            else if( 5000 > ListData->restype && -16 < ListData->restype  )
            {
                //get the DXF type code
                short xDataType = acdbGroupCodeToType( ListData->restype );

                if (xDataType != RTERROR)
                {
                    switch (xDataType)
                    {

                    case kDwgNull:
                    case kDwgBChunk:
                    case kDwgNotRecognized:
                        *pFile->m_pArchive << ListData->restype;
                        break;
                    case kDwgReal:
                        *pFile->m_pArchive << ListData->restype;

                        *pFile->m_pArchive << ListData->resval.rreal;		
                        break;
                    case kDwgInt16:
                    case kDwgInt8:
                        {
                            *pFile->m_pArchive << ListData->restype;

                            int nVal = ListData->resval.rint;
                            *pFile->m_pArchive << nVal;		
                            break;
                        }
                    case kDwgText:
                    case kDwgHandle:
                        {
                            *pFile->m_pArchive << ListData->restype;

                            CString sText = ListData->resval.rstring;
                            *pFile->m_pArchive << sText;
                            break;
                        }
                    case kDwgHardOwnershipId:
                    case kDwgSoftOwnershipId:
                    case kDwgHardPointerId:
                    case kDwgSoftPointerId:
                        {
                            *pFile->m_pArchive << ListData->restype;

                            *pFile->m_pArchive << ListData->resval.rlname[0];		
                            *pFile->m_pArchive << ListData->resval.rlname[1];		
                            break;
                        }
                    case kDwg3Real:
                        {
                            *pFile->m_pArchive << ListData->restype;

                            double dVal;
                            dVal = ListData->resval.rpoint[0];		
                            *pFile->m_pArchive << dVal;
                            dVal = ListData->resval.rpoint[1];		
                            *pFile->m_pArchive << dVal;
                            dVal = ListData->resval.rpoint[2];		
                            *pFile->m_pArchive << dVal;

                            break;
                        }
                    case kDwgInt32:
                        *pFile->m_pArchive << ListData->restype;

                        *pFile->m_pArchive << ListData->resval.rlong;
                        break;
                    }
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
    catch(...)
    {
        acedRetNil();
        acutRelRb(ListData);
        return 0;
    }

}

int ReadBinFile()
{
    struct resbuf *pRB = NULL, *pRBFinal = NULL, *ListData = NULL ;
    pRBFinal = acutNewRb(RTNONE);
    short restype;
    CLispFileObject *pFile = NULL;

    try
    {

        //ensure AutoLISP has passed Arguments	
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
        else
        {
            acedRetVoid();
            return 0; 		
        }
        CArchive &ar = *pFile->m_pArchive;

        // get the resbuf variable type
        ar >> restype;

        // if the restype is RTFAIL this is the end of the file.
        while ( restype != RTFAIL  || pFile->m_bFoundEnd != true ) 
        {	
            if(5000 < restype)
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
            }
            else if ( 5000 > restype && -16 < restype )
            {
                short xDataType = acdbGroupCodeToType(restype); 
                if (xDataType != RTERROR)
                {
                    switch (xDataType)
                    {
                    case kDwgReal:
                        {
                            double dVal;
                            ar >> dVal;
                            pRB = acutNewRb(restype);	
                            pRB->resval.rreal = dVal;
                            break;
                        }
                    case kDwgInt16:
                    case kDwgInt8:
                        {
                            int nVal;
                            ar >> nVal;
                            pRB = acutNewRb(restype);	
                            pRB->resval.rint = nVal;

                            break;
                        }
                    case kDwgText:
                    case kDwgHandle:
                        {
                            CString sText;
                            ar >> sText;			
                            pRB = acutNewRb(restype);	
                            acutNewString(sText, pRB->resval.rstring);	
                            break;
                        }
                    case kDwgHardOwnershipId:
                    case kDwgSoftOwnershipId:
                    case kDwgHardPointerId:
                    case kDwgSoftPointerId:
                        {
                            pRB = acutNewRb(restype);	
                            ar >> pRB->resval.rlname[0];		
                            ar >> pRB->resval.rlname[1];		
                            break;
                        }
                    case kDwg3Real:
                        {
                            pRB = acutNewRb(restype);	
                            ar >> pRB->resval.rpoint[0];		
                            ar >> pRB->resval.rpoint[1];		
                            ar >> pRB->resval.rpoint[2];		
                            break;
                        }
                    case kDwgInt32:
                        long lVal;
                        ar >> lVal;
                        pRB = acutNewRb(restype);	
                        pRB->resval.rlong = lVal;
                        break;

                    case kDwgNull:
                    case kDwgBChunk:
                    case kDwgNotRecognized:
                        pRB = acutNewRb(restype);	
                        break;
                    }
                }
            }

            // get the resbuf variable type
            ar >> restype;

            // if the restype is RTFAIL this is the end of the file.
            if (restype == RTFAIL)
            {
                pFile->m_bFoundEnd = true;
            }

            if ( pRB != NULL)
            {
                AppendResBuf( pRBFinal , pRB);
            }
        }
        if ( pRBFinal->rbnext != NULL)
        {
            pRBFinal = pRBFinal->rbnext;
        }

        acedRetList(pRBFinal);
        return 0;
    }
    catch(...)
    {
        acedRetNil();
        acutRelRb(pRB);
        acutRelRb(pRBFinal);
        acutRelRb(ListData);
        return 0; 
    }
}

void AppendResBuf( resbuf *pRb, resbuf *pInsert )
{
    while ( pRb->rbnext != NULL )
        pRb = pRb->rbnext;
    pRb->rbnext = pInsert;
}

int CloseBinFile()
{
    struct resbuf *ListData = NULL;
    CFileException Exception;
    CLispFileObject *pFile = NULL;

    //ensure AutoLISP has passed Arguments	
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
            *pFile->m_pArchive << int(RTFAIL);

            pFile->m_pArchive->Close();
            delete pFile->m_pArchive;

            pFile->m_File.Close();
            delete pFile;

            pFile->m_bIsOpen = false;

            acedRetT();
            return 0;
        }
        else 
        {
            pFile->m_File.Close();
            delete pFile;

            pFile->m_bIsOpen = false;

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
