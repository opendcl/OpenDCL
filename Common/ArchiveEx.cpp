#include "stdafx.h"
#include "ArchiveEx.h"
#include "StgFile.h"
#include "zlib.h"

LPCSTR gszaSignature = "akz";

CArchiveEx::CArchiveEx(CFile* pFile, UINT nMode, void* lpBuf, CString sKey, BOOL bCompress)
: CArchive(pFile, nMode, 4096, lpBuf)
, m_strKey( sKey )
, mpCompressedDataFile( NULL )
{
	if(bCompress)
	{
		mpCompressedDataFile = m_pFile;
		m_pFile = new CMemFile;
		if(IsLoading())
		{
			mpCompressedDataFile->SeekToBegin();
			ArcHeader ahead;
			mpCompressedDataFile->Read(&ahead, sizeof(ahead));
			CHAR sign[4];
			sign[0] = ahead.chSignature[0];
			sign[1] = ahead.chSignature[1];
			sign[2] = ahead.chSignature[2];
			sign[3] = '\0';
			static const CStringA sSignature( gszaSignature );
			if(sSignature == sign)
			{
				DWORD dwFileSize = DWORD(pFile->GetLength());
				BYTE* pCompBuf = new BYTE[dwFileSize];
				dwFileSize = mpCompressedDataFile->Read(pCompBuf, dwFileSize);
				if(ahead.uchFlag == 3)
				{
					ULONG crc = adler32(0L, Z_NULL, 0);
					if( !m_strKey.IsEmpty() )
						Crypto(pCompBuf, dwFileSize, m_strKey);
					crc = adler32(crc, pCompBuf, dwFileSize);
					if(crc != ahead.ulCRC)
					{
						AfxMessageBox(_T("Password incorrect!!!"));
						Abort();
						AfxThrowUserException();
					}
				}

				DWORD dwOrigSize = ahead.dwOriginalSize;
				BYTE* pUncomp = new BYTE[dwOrigSize];
				int iComprErr = uncompress(pUncomp, &dwOrigSize, pCompBuf, dwFileSize);
				ASSERT(iComprErr == Z_OK);
				delete[] pCompBuf;
				m_pFile->Write(pUncomp, dwOrigSize);
				delete[] pUncomp;
			}
			m_pFile->SeekToBegin();
		}
	}
}

CArchiveEx::~CArchiveEx()
{
	if(mpCompressedDataFile)
		Close();
}

void CArchiveEx::Abort()
{
	if(mpCompressedDataFile)
	{
		delete m_pFile;
		m_pFile = mpCompressedDataFile;
		mpCompressedDataFile = NULL;
	}
	CArchive::Abort();
}

void CArchiveEx::Close()
{
	ASSERT_VALID(mpCompressedDataFile);
	Flush();

	if(mpCompressedDataFile)
	{
		if(IsStoring())
		{
			DWORD cbUncomp = DWORD(m_pFile->GetLength());
			BYTE* pBufSrc = new BYTE[cbUncomp];
			m_pFile->SeekToBegin();
			m_pFile->Read(pBufSrc, cbUncomp);
			DWORD dwComprSize = (DWORD)(cbUncomp * 1.1) + 12;
			BYTE* pBufComp = new BYTE[dwComprSize];
			int iComprErr = compress(pBufComp, &dwComprSize, pBufSrc, cbUncomp);
			ASSERT(iComprErr == Z_OK);
			delete[] pBufSrc;
			ULONG crc = adler32(0L, Z_NULL, 0);
			crc = adler32(crc, pBufComp, dwComprSize);
			Crypto(pBufComp, dwComprSize, m_strKey);

			ArcHeader ahead;
			ahead.chSignature[0] = 'a';
			ahead.chSignature[1] = 'k';
			ahead.chSignature[2] = 'z';
			ahead.dwOriginalSize = cbUncomp;
			ahead.uchFlag = m_strKey.IsEmpty()?1:3;
			ahead.ulCRC = crc;

			mpCompressedDataFile->SeekToBegin();
			mpCompressedDataFile->Write(&ahead, sizeof(ahead));
			mpCompressedDataFile->Write(pBufComp, dwComprSize);
			delete[] pBufComp;
		}
		delete m_pFile;
		m_pFile = mpCompressedDataFile;
		mpCompressedDataFile = NULL;
	}
	CArchive::Close();
}

bool CArchiveEx::Crypto(BYTE* pBuffer, ULONG ulLen, CString strKey)
{
	if(strKey.IsEmpty())
		return FALSE;
	CStringA strKeyA(strKey);
	int nLenKey = strKeyA.GetLength();
	for(ULONG ii=0; ii<ulLen; ii++)
	{
		for(int jj=0; jj<nLenKey; jj++)
			pBuffer[ii] ^= strKeyA[jj];
	}
	return TRUE;
}
