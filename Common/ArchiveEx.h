#pragma once

struct ArcHeader
{
	CHAR			chSignature[3];		//	"akz"
	BYTE			uchFlag;			//	0-plane text; 1-compr; 2-crypt; 3-compr & crypt;
	DWORD			dwOriginalSize;		//	decompr size
	ULONG			ulCRC;				//	CRC
};

class CArchiveEx : public CArchive
{
	CString m_strKey;
	CFile* mpCompressedDataFile;

public:
	CArchiveEx(CFile* pFile, UINT nMode, void* lpBuf = NULL, CString sKey = _TEXT(""), BOOL bCompress = FALSE);
	CArchiveEx();
	virtual ~CArchiveEx();

	virtual void Close();
	virtual void Abort();

protected:
	virtual bool Crypto(BYTE* pBuffer, ULONG ulLen, CString strKey = _TEXT(""));
};
