#pragma once

// simple derived class that reads Unicode files via ReadString

class CStdioUnicodeFile :
	public CStdioFile
{
	enum { Unknown, Ansi, Unicode } mfFileType;
public:
	CStdioUnicodeFile(void);
	CStdioUnicodeFile(FILE* pOpenStream);
	CStdioUnicodeFile(LPCTSTR lpszFileName, UINT nOpenFlags);
	~CStdioUnicodeFile(void);

	virtual void WriteString(LPCTSTR lpsz);
	virtual BOOL ReadString(CString& rString);

protected:
	BOOL ReadAnsiString( CStringA& rString );
	BOOL ReadUnicodeString( CStringW& rString );
	BOOL IsUnicode();
};
