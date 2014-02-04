// StudioWorkspace.h : header file
//

#pragma once

#include "Workspace.h"
#include "StudioProject.h"
#include "PtrTypes.h"
#include <list>

class CProjectPane;
class CStudioFrame;
class CStudioProject;
class CToolboxPane;
class CPropertyPane;
class CTabOrderPane;
class COpenDCLDoc;
class CDclFormView;
class CStudioDialogObject;


#define theStudioWorkspace (*(CStudioWorkspace*)&theWorkspace)


class CStudioWorkspace : public CWorkspace
{
	CStudioFrame* mpStudioFrame;
	CDclFormView* mpActiveFormView;
	CStudioDialogObject* mpActiveDlgObject;
	TDclControlPtr mpActiveDclControl;
	HMODULE mhmodLocalRes;

public:
	CStudioWorkspace();
	virtual ~CStudioWorkspace();

public:
	virtual CString GetAppKey(void) const { return _T("Studio"); }
	virtual CString GetLocalResourceModuleFilename() const { return _T("Studio.Res.dll"); }
	virtual void SetModified( bool bModified = true );
	virtual TStudioProjectPtr GetActiveProject() const;
	virtual CString GetLanguage(void) const;
	virtual FontSettings GetDefaultFontSettings() const;
	virtual void OnGridSpacingChange( UINT nGridSpacing ); // change grid spacing

	//Attributes
	CStudioFrame* GetStudioFrame() const { return mpStudioFrame; }
	void SetStudioFrame( CStudioFrame* pStudioFrame ) { mpStudioFrame = pStudioFrame; }
	void SetActiveFormView( CDclFormView* pFormView ) { mpActiveFormView = pFormView; }
	CDclFormView* GetActiveFormView() const { return mpActiveFormView; }
	CDocument* GetActiveDocument() const;
	CProjectPane* GetProjectPane() const;
	CPropertyPane* GetPropertyPane() const;
	CToolboxPane* GetToolboxPane() const;
	CTabOrderPane* GetTabOrderPane() const;
	CString GetActiveProjectName() const;

	// Undo notifications
	void OnFlushUndoGroup();

	// Manage active project/form view
	void ActivateProject( TStudioProjectPtr pProject, CDocument* pDoc );
	void ActivateDlgObject( CStudioDialogObject* pDlgObject );
	void DeactivateDlgObject( CStudioDialogObject* pDlgObject );
	void ActivateDclControl( TDclControlPtr pDclControl, bool bDeactivateCurrent = false );
	CStudioDialogObject* GetActiveDlgObject() const { return mpActiveDlgObject; }
	TDclControlPtr GetActiveDclControl() const { return mpActiveDclControl; }
};
