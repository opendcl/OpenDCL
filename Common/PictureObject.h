#pragma once

class CStgFile;
enum IOStatus;


class CPictureObject : public CObject
{
protected:
	int m_nID;
	int m_Height;
	int m_Width;
	
	CPictureHolder m_hPicture;
	HICON m_hIcon;
	bool m_ToBeAdded;
	bool m_ToBeDeleted;
	short m_PicType; //ARX only
	bool m_bLoaded; //ARX only
	HBITMAP m_hBitmap; //ARX only
	CString m_sFileName; //ARX only

public:
	HICON m_hLoadedIcon; //ARX only

public:
	CPictureObject(void);
	virtual ~CPictureObject(void);

	int GetID() const { return m_nID; }
	void SetID(int nID) { m_nID = nID; }
	int GetWidth() const { return m_Width; }
	void SetWidth( int nWidth ) { m_Width = nWidth; }
	int GetHeight() const { return m_Height; }
	void SetHeight( int nHeight ) { m_Height = nHeight; }
	bool GetToBeAdded() const { return m_ToBeAdded; }
	void SetToBeAdded(bool bNew = true) { m_ToBeAdded = bNew; }
	bool GetToBeDeleted() const { return m_ToBeDeleted; }
	void SetToBeDeleted(bool bNew = true) { m_ToBeDeleted = bNew; }
	HICON GetIcon() const;
	const CPictureHolder& GetPicture() const { return m_hPicture; }
	CPictureHolder& GetPicture() { return m_hPicture; } //need to refactor so this function can be removed [ORW]
	HBITMAP GetBitmap() const;
	void SetBitmap(HBITMAP hBitmap) { m_hBitmap = hBitmap; }
	LPCTSTR GetFileName() const { return m_sFileName; }
	void SetFileName(LPCTSTR pszFileName) { m_sFileName = pszFileName; }

	//Copied from non-member utility function in Editor
	void Update(short nID, LPPICTUREDISP NewPicture);
	void LoadFile(LPCTSTR szFile, int nID);
	static CPictureObject* CreatePictureObject(short nID, LPPICTUREDISP NewPicture);

	//Editor Only
	short GetPicType() const;	
	void Clear();
	void SaveSS(CStgFile &FileStg, CDocument *pDoc) const;
	virtual void Serialize(CArchive& ar);
  IOStatus WriteToTextFile(FILE* pFile, const CString &fileName) const;
	static CPictureObject* ReadSS(int nID, CStgFile &FileStg, CDocument *pDoc);

	//ARX Only
  IOStatus ReadFromTextFile(std::ifstream& sFile, const CString &fileName);
  IOStatus ReadFromTextFile3(std::ifstream& sFile, const CString &fileName);
	void Render(CDC *pdc, int nPicLeft, int nPicTop, CRect &rcThis, bool bAutoSize = false);
	void EnsurePictureIsLoaded();
	void SaveSS(CStgFile &FileStg) const;
	static CPictureObject* ReadSS(int nID, CStgFile &FileStg);
	
protected:
	BOOL PX_IUnknown(CArchive& ar, LPUNKNOWN& pUnk, REFIID iid, LPUNKNOWN pUnkDefault = NULL);
	BOOL PX_Picture(CArchive& ar, CPictureHolder& pict);
	BOOL ExchangePersistentProp(CArchive& ar, LPUNKNOWN* ppUnk, REFIID iid, LPUNKNOWN pUnkDefault);
	BOOL AFX_CDECL PX_IUnknown2(CPropExchange* pPX, LPCTSTR pszPropName, LPUNKNOWN& pUnk,
		REFIID iid, LPUNKNOWN pUnkDefault = NULL);
	BOOL AFX_CDECL PX_Picture(CPropExchange* pPX, LPCTSTR pszPropName, CPictureHolder& pict);

	DECLARE_SERIAL(CPictureObject)
};
