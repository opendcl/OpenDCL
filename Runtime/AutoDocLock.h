// AutoDocLock.h : header file
//

#pragma once


template< AcAp::DocLockMode _lockmode = AcAp::kWrite >
class CAutoDocLock
{
	AcApDocument* mpDoc;
public:
	CAutoDocLock( AcApDocument* pDoc = acDocManager->curDocument() ) : mpDoc( pDoc )
		{
			if( Acad::eOk != acDocManager->lockDocument( pDoc, _lockmode ) )
				mpDoc = NULL;
		}
	CAutoDocLock( AcDbDatabase* pDb ) : mpDoc( acDocManager->document( pDb ) )
		{
			if( mpDoc && Acad::eOk != acDocManager->lockDocument( mpDoc, _lockmode ) )
				mpDoc = NULL;
		}
	~CAutoDocLock(void)
		{
			if( mpDoc )
				acDocManager->unlockDocument( mpDoc );
		}
	operator bool() const { return (mpDoc != NULL); }
};

typedef CAutoDocLock< AcAp::kRead > CAutoDocReadLock;
typedef CAutoDocLock< AcAp::kWrite > CAutoDocWriteLock;
