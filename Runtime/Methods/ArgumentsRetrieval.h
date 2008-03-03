// ArgumentsRetrieval.h : header file
//

#pragma once

#include "DialogObject.h"
#include "ArxProject.h"
#include "ControlTypes.h"
#include "AxArg.h"

class CPropertyObject;


namespace odcl
{
	enum ArgErr
	{
		argNil,
		argNotEnough,
		argWrongType,
		argTooMany,
		argInvalid,
		argNoInstance,
	};
};

ADSRESULT HandleArgValueError( /*in*/ const resbuf* pArgs, /*in*/ UINT nMsgId = ~UINT(0), ... );
bool AssertOutOfArgs( /*in*/ const resbuf* pArgs, /*in*/ bool bQuiet = false );

bool GetProjectArgument( /*in-out*/ resbuf*& pArgs, /*out*/ TArxProjectPtr& pProject, /*in*/ bool bQuiet = false );
bool GetFormArgument( /*in-out*/ resbuf*& pArgs, /*out*/ TDclFormPtr& pForm, /*in*/ bool bQuiet = false );
bool GetControlArgument( /*in-out*/ resbuf*& pArgs, /*out*/ TDclControlPtr& pDclControl, /*in*/ bool bQuiet = false );
bool GetDialogArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CDialogObject*& pDialog, /*in*/ bool bQuiet = false );
bool GetDlgControlArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CDialogControl*& pDlgControl, /*in*/ ControlType type, /*in*/ bool bQuiet = false );
bool GetPropertyArgument( /*in-out*/ resbuf*& pArgs, /*out*/ TPropertyPtr& pProperty, /*in*/ bool bQuiet = false );

bool GetBoolArgument( /*in-out*/ resbuf*& pArgs, /*out*/ bool& bArg, /*in*/ bool bQuiet = false );
bool GetStringArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CString& sArg, /*in*/ bool bQuiet = false );
bool GetIntArgument( /*in-out*/ resbuf*& pArgs, /*out*/ int& nArg, /*in*/ bool bQuiet = false );
bool GetUIntArgument( /*in-out*/ resbuf*& pArgs, /*out*/ unsigned int& nArg, /*in*/ bool bQuiet = false );
bool GetLongArgument( /*in-out*/ resbuf*& pArgs, /*out*/ long& nArg, /*in*/ bool bQuiet = false );
bool GetHandleArgument( /*in-out*/ resbuf*& pArgs, /*out*/ DWORD_PTR& nArg, /*in*/ bool bQuiet = false );
bool GetDoubleArgument( /*in-out*/ resbuf*& pArgs, /*out*/ double& dblArg, /*in*/ bool bQuiet = false );
bool Get3dPointArgument( /*in-out*/ resbuf*& pArgs, /*out*/ AcGePoint3d& pntArg, /*in*/ bool bQuiet = false );
bool Get2dPointArgument( /*in-out*/ resbuf*& pArgs, /*out*/ AcGePoint2d& pntArg, /*in*/ bool bQuiet = false );
bool GetDateArgument( /*in-out*/ resbuf*& pArgs, /*out*/ COleDateTime& dtArg, /*in*/ bool bQuiet = false );
bool GetIUnknownArgument( /*in-out*/ resbuf*& pArgs, /*out*/ IUnknown*& pUnk, /*in*/ bool bQuiet = false );
bool GetIDispatchArgument( /*in-out*/ resbuf*& pArgs, /*out*/ IDispatch*& pDisp, /*in*/ bool bQuiet = false );
bool GetVariantArgument( /*in-out*/ resbuf*& pArgs, /*out*/ COleVariant& varArg, /*in*/ const AxArg& type, /*in*/ bool bQuiet = false );
bool GetColorArgument( /*in-out*/ resbuf*& pArgs, /*out*/ COLORREF& color, /*in*/ bool bQuiet = false );
bool GetStringArrayArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CStringArray& rsArg, /*in*/ bool bQuiet = false );
bool GetIntArrayArgument( /*in-out*/ resbuf*& pArgs, /*out*/ CArray< int, int >& rnArg, /*in*/ bool bQuiet = false );

bool GetTArgument( /*in-out*/ resbuf*& pArgs, /*in*/ bool bQuiet = false );
bool GetNilArgument( /*in-out*/ resbuf*& pArgs, /*in*/ bool bQuiet = false );
bool GetListBeginArgument( /*in-out*/ resbuf*& pArgs, /*in*/ bool bQuiet = false );
bool GetListEndArgument( /*in-out*/ resbuf*& pArgs, /*in*/ bool bQuiet = false );
