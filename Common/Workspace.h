#pragma once

#include "FontCollection.h"
#include "OleControlObject.h"

class CProject;
class AxPropertyDescriptor;
class AxMethodDescriptor;


#define theWorkspace (*theWorkspacePtr()) //use 'theWorkspace' to access the single global workspace instance
extern class CWorkspace* theWorkspacePtr(); //each client must define this function to return a client-specific workspace


//The workspace class is designed to be a singleton base class representing common functionality
//shared by the editor and the ARX modules. Each client should create one instance of a class derived
//from CWorkspace and use that single instance to store all global project-related state data.
class CWorkspace
{
	CFontCollection mFonts;

	//global settings
	bool mbMessagesSuppressed;

public:
	CWorkspace();
	~CWorkspace();

public:
	//Workspace constants
	virtual DWORD GetMinSupportedAcadVersion(void) const { return 2000; }
	virtual CString GetProjectFileExtension(void) const { return _T(".odcl"); }

	//Services
	virtual CString GetSettingsRegPath(void) const { return _T("Software\\OpenDCL"); }
	virtual CString GetUserProfilePrefix(void) const { return CString(); }
	virtual HMODULE GetResourceModule(void) const { return GetModuleHandle( NULL ); }
	virtual HMODULE GetLocalResourceModule(void) const { return GetModuleHandle( NULL ); }
	virtual CString LoadResourceString( int nResId, HMODULE hmodRes = NULL ) const;
	virtual CString GetLanguage(void);
	virtual bool DisplayAlert( LPCTSTR pszMessage ) const; //display alert dialog; returns true if displayed, false if suppressed
	virtual bool DisplayStatus( LPCTSTR pszMessage ) const; //display modeless status message; returns true if displayed, false if suppressed
	virtual void SetModified( bool bModified ) {}
	virtual CProject* GetActiveProject(void) const = 0; //must be overridden in derived class
	virtual CDocument* GetActiveDocument(void) const { return NULL; }
	virtual RefCountedPtr< COleControlObject > GetOleControlFor( const AxPropertyDescriptor* pProperty ) const = 0;
	virtual RefCountedPtr< COleControlObject > GetOleControlFor( const AxMethodDescriptor* pMethod ) const = 0;
	virtual CString FindFile( LPCTSTR pszFilePath ) const;

	virtual bool GetModuleVersionInfo( DWORD& dwMajor, DWORD&dwMinor, DWORD& dwThird, DWORD& dwFourth, HMODULE hmodTarget = NULL ) const;
	virtual bool DisplayAlert( UINT nResourceId, HMODULE hmodRes = NULL ) const;
	virtual bool DisplayStatus( UINT nResourceId, HMODULE hmodRes = NULL ) const;
	virtual bool GetDwordSetting( DWORD& dwValue, LPCTSTR pszValue );
	virtual bool SetDwordSetting( DWORD dwValue, LPCTSTR pszValue );
	virtual bool IsAutoUpdateCheckEnabled(void) const;
	virtual bool SetAutoUpdateCheckEnabled( bool bEnabled = true );

	//Attributes
	bool IsMessagesSuppressed(void) const { return mbMessagesSuppressed; }
	void SetMessagesSuppressed( bool bSuppressed = true ) { mbMessagesSuppressed = bSuppressed; }
	const CFontCollection& GetFontCollection(void) const { return mFonts; }
	CFontCollection& GetFontCollection(void) { return mFonts; }
};
