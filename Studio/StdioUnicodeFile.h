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

	void WriteString(LPCTSTR lpsz) override;
	BOOL ReadString(CString& rString) override;

protected:
	BOOL ReadAnsiString( CStringA& rString );
	BOOL ReadUnicodeString( CStringW& rString );
	BOOL IsUnicode();
};
