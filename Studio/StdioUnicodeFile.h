#pragma once

// simple derived class that reads Unicode files via ReadString

class CStdioUnicodeFile :
	public CStdioFile
{
	enum { Unknown, Ansi, Unicode } mfReadType;
public:
	CStdioUnicodeFile(void);
	CStdioUnicodeFile(FILE* pOpenStream);
	CStdioUnicodeFile(LPCTSTR lpszFileName, UINT nOpenFlags);
	~CStdioUnicodeFile(void);

	virtual BOOL ReadString(CString& rString);

protected:
	BOOL ReadAnsiString( CStringA& rString );
	BOOL ReadUnicodeString( CStringW& rString );
	BOOL IsUnicode();
};
