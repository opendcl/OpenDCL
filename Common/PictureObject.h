#pragma once

class CStgFile;
enum IOStatus;


class CPictureObject : public CObject
{
protected:
	UINT mnID;
	CSize msizePic;
	CPictureHolder m_hPicture;

private:
	CPictureObject& operator= (const CPictureObject&);
protected:
	CPictureObject(void);
public:
	CPictureObject( const CPictureObject& );
	CPictureObject( UINT nID );
	CPictureObject( UINT nID, LPCTSTR szFile, bool bApplyMask = false );
	virtual ~CPictureObject(void);

protected:
	//2007-02-12 [ORW]: save version set to 6 (was originally set to 3, but some code in Serialize() expects 4 or 5)
	//2007-02-15 [ORW]: save version set to 7 (removed duplicated pic type)
	//2007-02-28 [ORW]: save version set to 8 (removed cached width and height from archive)
	ULONG GetCurrentSaveVersion() const { return 8; }

public:
	UINT GetID() const { return mnID; }
	int GetWidth() const { return msizePic.cx; }
	int GetHeight() const { return msizePic.cy; }
	short GetPicType() const;	
	bool IsValid() const { return (msizePic.cx > 0 && msizePic.cy > 0); }
	//const CPictureHolder& GetPicture() const { return m_hPicture; }
	LPPICTUREDISP GetPictureDisp() const;
	const HBITMAP GetBitmap() const;
	const HICON GetIcon() const;
	HBITMAP CloneBitmap() const;
	HICON CloneIcon() const;
	void LoadResourceIcon( UINT nIconResId, HMODULE hResMod = NULL );

	//Copied from non-member utility function in Editor
	void Update( LPPICTUREDISP NewPicture );
	void LoadFile( LPCTSTR szFile, bool bApplyMask = false );
	static CPictureObject* CreatePictureObject( short nID, LPPICTUREDISP NewPicture );

	void Clear();
	void Render( CDC *pdc, int nPicLeft, int nPicTop, CRect &rcThis, bool bAutoSize = false ) const;

	// File I/O
	virtual void Serialize(CArchive& ar);
  IOStatus ReadFromTextFile(std::ifstream& sFile, const CString &fileName);
  IOStatus ReadFromTextFile3(std::ifstream& sFile, const CString &fileName);
  //IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
	
protected:
	void CalcLogicalSize();
	BOOL PX_IUnknown(CArchive& ar, LPUNKNOWN& pUnk, REFIID iid, LPUNKNOWN pUnkDefault = NULL);
	BOOL PX_Picture(CArchive& ar, CPictureHolder& pict);
	BOOL ExchangePersistentProp(CArchive& ar, LPUNKNOWN* ppUnk, REFIID iid, LPUNKNOWN pUnkDefault);
	BOOL AFX_CDECL PX_IUnknown2(CPropExchange* pPX, LPCTSTR pszPropName, LPUNKNOWN& pUnk,
		REFIID iid, LPUNKNOWN pUnkDefault = NULL);
	BOOL AFX_CDECL PX_Picture(CPropExchange* pPX, LPCTSTR pszPropName, CPictureHolder& pict);

	DECLARE_SERIAL(CPictureObject)
};
