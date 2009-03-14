// UndoActions.h : header file
//

#pragma once

#include "PtrTypes.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyObject.h"

class CStudioProject;


namespace Undo
{
	enum Action
	{
		SaveProject,
		BeginGroup,
		EndGroup,
		AddForm,
		DeleteForm,
		SelectForm,
		AddControl,
		DeleteControl,
		SelectControl,
		ReorderControl,
		PropertyChange,
	};
};


class CUndoAction
{
	Undo::Action mType;

public:
	CUndoAction( Undo::Action type )
		: mType( type ) {}
	virtual ~CUndoAction() {}

	Undo::Action GetType() const { return mType; }
	virtual CString GetDisplayName() const = 0;
	virtual bool Undo() = 0;

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const = 0;
#endif
};


class CSaveProjectUA : public CUndoAction
{
	CStudioProject* mpProject;
public:
	CSaveProjectUA( CStudioProject* pProject )
		: CUndoAction( Undo::SaveProject ), mpProject( pProject ) {}
	virtual ~CSaveProjectUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const { return _T("SaveProject"); }
#endif
};

class CBeginGroupUA : public CUndoAction
{
	CString msDisplayName;
public:
	CBeginGroupUA( LPCTSTR pszDisplayName )
		: CUndoAction( Undo::BeginGroup ), msDisplayName( pszDisplayName ) {}
	virtual ~CBeginGroupUA() {}

	virtual CString GetDisplayName() const { return msDisplayName; }
	virtual bool Undo() { return true; }

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("BeginGroup[%s]"), (LPCTSTR)msDisplayName );
		return sResult;
	}
#endif
};

class CEndGroupUA : public CUndoAction
{
public:
	CEndGroupUA()
		: CUndoAction( Undo::EndGroup ) {}
	virtual ~CEndGroupUA() {}

	virtual CString GetDisplayName() const { return NULL; }
	virtual bool Undo() { return true; }

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const { return _T("EndGroup"); }
#endif
};

class CSelectFormUA : public CUndoAction
{
	TDclFormPtr mpForm;

public:
	CSelectFormUA( TDclFormPtr pNewForm )
		: CUndoAction( Undo::SelectForm ), mpForm( pNewForm ) {}
	virtual ~CSelectFormUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("SelectForm[%s]"), (LPCTSTR)mpForm->GetKeyName() );
		return sResult;
	}
#endif
};

class CAddFormUA : public CUndoAction
{
	TDclFormPtr mpForm;

public:
	CAddFormUA( TDclFormPtr pNewForm )
		: CUndoAction( Undo::AddForm ), mpForm( pNewForm ) {}
	virtual ~CAddFormUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("AddForm[%s]"), (LPCTSTR)mpForm->GetKeyName() );
		return sResult;
	}
#endif
};

class CDeleteFormUA : public CUndoAction
{
	TDclFormPtr mpForm;

public:
	CDeleteFormUA( TDclFormPtr pNewForm )
		: CUndoAction( Undo::DeleteForm ), mpForm( pNewForm ) {}
	virtual ~CDeleteFormUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("DeleteForm[%s]"), (LPCTSTR)mpForm->GetKeyName() );
		return sResult;
	}
#endif
};

class CSelectControlUA : public CUndoAction
{
	TDclControlPtr mpDclControl;

public:
	CSelectControlUA( TDclControlPtr pDclControl )
		: CUndoAction( Undo::SelectControl ), mpDclControl( pDclControl ) {}
	virtual ~CSelectControlUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("SelectControl[%s]"), (LPCTSTR)mpDclControl->GetKeyName() );
		return sResult;
	}
#endif
};

class CAddControlUA : public CUndoAction
{
	TDclControlPtr mpDclControl;

public:
	CAddControlUA( TDclControlPtr pDclControl )
		: CUndoAction( Undo::AddControl ), mpDclControl( pDclControl ) {}
	virtual ~CAddControlUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("AddControl[%s]"), (LPCTSTR)mpDclControl->GetKeyName() );
		return sResult;
	}
#endif
};

class CDeleteControlUA : public CUndoAction
{
	TDclControlPtr mpDclControl;
	size_t mIdx;

public:
	CDeleteControlUA( TDclControlPtr pDclControl );
	virtual ~CDeleteControlUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("DeleteControl[%s]"), (LPCTSTR)mpDclControl->GetKeyName() );
		return sResult;
	}
#endif
};

class CReorderControlUA : public CUndoAction
{
	TDclControlPtr mpDclControl;
	size_t mIdx;

public:
	CReorderControlUA( TDclControlPtr pDclControl );
	virtual ~CReorderControlUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("ReorderControl[%s]"), (LPCTSTR)mpDclControl->GetKeyName() );
		return sResult;
	}
#endif
};

class CPropertyChangeUA : public CUndoAction
{
	TDclControlPtr mpDclControl;
	Prop::Id mId;
	CMemFile mValue;

public:
	CPropertyChangeUA( TPropertyPtr pProperty );
	virtual ~CPropertyChangeUA() {}

	virtual CString GetDisplayName() const;
	virtual bool Undo();

#ifdef _DIAGNOSTIC
public:
	virtual CString toString() const
	{
		CString sResult;
		sResult.Format( _T("PropertyChange[%s/%s]"), (LPCTSTR)mpDclControl->GetKeyName(), GetPropertyApiName( mId ) );
		return sResult;
	}
#endif
};
