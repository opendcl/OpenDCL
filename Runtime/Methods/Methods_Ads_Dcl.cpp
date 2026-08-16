// Methods_Ads_Dcl.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Ads_Dcl.h"
#include "ArxWorkspace.h"
#include "ArxProject.h"
#include "DialogObject.h"
#include "DclFormTemplate.h"
#include "DclControlTemplate.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "ArxAcadSlideCtrl.h"
#include "AcadColorService.h"


static LPCTSTR gpszDclEventsLspFileName = _T("DclEvents.lsp");
static LPCTSTR gpszOnActionEventLispFunction = _T("(defun c:OnActionEvent_%s()%s)(princ)\n");

// Session for classic DCL tile/dialog ADS (was CARXApp statics).
static CString msDialogToBeShown;
static CPoint mptToBeShown(INT_MIN, INT_MIN);
static TArxProjectPtr mpProjectToBeShown = NULL;
static CString msActionToBeShown;
static TDclFormPtr mpDclToBeShown = NULL;
static int mnDoneDialogValue = -1;
static int mnListOperation = 3; // 1 = change selection, 2 = append, 3 = replace all


static TDclControlPtr FindCurrentControl(LPCTSTR pszKey)
{
	if (!mpDclToBeShown || !pszKey)
		return TDclControlPtr();
	return mpDclToBeShown->FindControl(pszKey);
}

ADSRESULT AdsDcl::NewDialog()
{
	struct resbuf *pArgs =acedGetArgs () ;
	if (pArgs == NULL)
		return RSERR; //argument expected

	if (pArgs->restype != RTSTR)
		return RSERR; //wrong argument type
	msDialogToBeShown = pArgs->resval.rstring;
	pArgs = pArgs->rbnext; //move to the next argument

	switch (pArgs->restype)
	{
	case RTLONG:
		mpProjectToBeShown = TArxProjectLockedPtr( (CArxProject*)pArgs->resval.rlong );
		break;
	case RTENAME:
		mpProjectToBeShown = TArxProjectLockedPtr( (CArxProject*)pArgs->resval.rlname[0] );
		break;
	default:
		return RSERR; //wrong argument type
	};
	assert (mpProjectToBeShown != NULL);
	pArgs = pArgs->rbnext; //move to the next argument

	//optional arguments
	msActionToBeShown.Empty();
	mptToBeShown.SetPoint(INT_MIN, INT_MIN); //should be encapsulated in an instance class [ORW]
	if (pArgs)
	{
		if (pArgs->restype != RTSTR)
			return RSERR; //wrong argument type
		msActionToBeShown = pArgs->resval.rstring;
		pArgs = pArgs->rbnext; //move to the next argument

		if (pArgs)
		{
			if (pArgs->restype != RTPOINT)
				return RSERR; //wrong argument type

			mptToBeShown.x = (int)pArgs->resval.rpoint[X];
			mptToBeShown.y = (int)pArgs->resval.rpoint[Y];

			if (pArgs->rbnext)
				return RSERR; //too many arguments
		}
	}

	// get the dcl form object that will be displayed
	mpDclToBeShown = mpProjectToBeShown->FindDclForm(msDialogToBeShown);
	TDclControlPtr pProps = mpDclToBeShown->GetControlProperties();
	pProps->SetStringProperty(Prop::FormEventInitialize, msActionToBeShown);

	TCHAR lpPathBuffer[MAX_PATH];
	::GetTempPath(MAX_PATH, lpPathBuffer);
	CString sTempFile = lpPathBuffer;
	sTempFile += gpszDclEventsLspFileName; /*"DclEvents.lsp"*/
	::DeleteFile(sTempFile);

	// create and open the action events lsp file to be loaded.
	CStdioFile tempFile(sTempFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite);
	tempFile.WriteString(_T("; This temp file is used for managing the events of dialog boxes using\n"));
	tempFile.WriteString(_T("; DCL equivalent functions in ObjectDCL.\n"));
	tempFile.Close();

	acedRetT();

	return (RSRSLT) ;
}

ADSRESULT AdsDcl::DoneDialog()
{
	struct resbuf *pArgs = acedGetArgs();

	mnDoneDialogValue = 0;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	mnDoneDialogValue = pArgs->resval.rint;

	if (pArgs->rbnext)
		return RSERR;

	if (!mpDclToBeShown)
		return RSERR;
	CDialogObject *pDialog = mpDclToBeShown->GetFormInstance();
	if (!pDialog)
		return RSERR;

	if (!pDialog->IsModeless())
		pDialog->CloseDialog(mnDoneDialogValue);

	return (RSRSLT);
}

ADSRESULT AdsDcl::StartDialog()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs)
		return RSERR;

	if (!mpDclToBeShown)
		return RSERR;

	TCHAR lpPathBuffer[MAX_PATH];
	::GetTempPath(MAX_PATH, lpPathBuffer);
	CString sTempFile = lpPathBuffer;
	sTempFile += gpszDclEventsLspFileName;
	resbuf rbArg = { NULL, RTSTR };
	rbArg.resval.rstring = sTempFile.LockBuffer();
	resbuf rbCallee = { &rbArg, RTSTR };
	rbCallee.resval.rstring = (ACHAR*)_T("c:loadlisp");
	resbuf* prbResult = NULL;
	if (acedInvoke(&rbCallee, &prbResult) != RTNORM)
		return RSERR;
	acutRelRb(prbResult);

	DialogParams params(mptToBeShown, CSize(0, 0));
	theArxWorkspace.ActivateDclForm(mpDclToBeShown, &params);

	acedRetInt(mnDoneDialogValue);
	return (RSRSLT);
}

ADSRESULT AdsDcl::StartList()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszListKey = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	mnListOperation = 3;
	int nIndex = 0;
	if (pArgs)
	{
		if (pArgs->restype != RTSHORT)
			return RSERR;
		mnListOperation = pArgs->resval.rint;
		pArgs = pArgs->rbnext;

		if (pArgs)
		{
			if (pArgs->restype != RTSHORT)
				return RSERR;
			nIndex = pArgs->resval.rint;
			if (pArgs->rbnext)
				return RSERR;
		}
	}
	(void)nIndex;

	TDclControlPtr pCtrl = FindCurrentControl(pszListKey);
	if (!pCtrl)
		return RSERR;

	if (pCtrl->GetWindow() == NULL)
	{
		TPropertyPtr pProp = pCtrl->GetPropertyObject(Prop::List);
		if (!pProp)
			return RSERR;
		pProp->GetStringArrayPtr()->clear();
	}
	else
	{
		switch (pCtrl->GetType())
		{
		case CtlListBox:
			if (mnListOperation == 3)
				((CListBox*)pCtrl->GetWindow())->ResetContent();
			break;
		case CtlComboBox:
			if (mnListOperation == 3)
				((CComboBox*)pCtrl->GetWindow())->ResetContent();
			break;
		case CtlListView:
			if (mnListOperation == 3)
				((CListCtrl*)pCtrl->GetWindow())->DeleteAllItems();
			break;
		}
	}

	return (RSRSLT);
}

ADSRESULT AdsDcl::AddList()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszListKey = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	if (!pArgs)
		return RSRSLT;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszValue = pArgs->resval.rstring;
	if (!pszValue)
		pszValue = _T("");

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszListKey);
	if (!pCtrl)
		return RSERR;

	if (pCtrl->GetWindow() == NULL)
	{
		TPropertyPtr pProp = pCtrl->GetPropertyObject(Prop::List);
		if (!pProp)
			return RSERR;
		pProp->GetStringArrayPtr()->push_back(pszValue);
	}
	else
	{
		switch (pCtrl->GetType())
		{
		case CtlListBox:
			{
				CListBox *pListBox = (CListBox*)pCtrl->GetWindow();
				if (mnListOperation == 1)
				{
					int nIndex = pListBox->GetCurSel();
					pListBox->DeleteString(nIndex);
					pListBox->InsertString(nIndex, pszValue);
				}
				else
					pListBox->AddString(pszValue);
				break;
			}
		case CtlComboBox:
			{
				CComboBox *pComboBox = (CComboBox*)pCtrl->GetWindow();
				if (mnListOperation == 1)
				{
					int nIndex = pComboBox->GetCurSel();
					pComboBox->DeleteString(nIndex);
					pComboBox->InsertString(nIndex, pszValue);
				}
				else
					pComboBox->AddString(pszValue);
				break;
			}
		case CtlListView:
			{
				CListCtrl *pListCtrl = (CListCtrl*)pCtrl->GetWindow();
				if (mnListOperation == 1)
				{
					int nIndex = -1;
					POSITION pos = pListCtrl->GetFirstSelectedItemPosition();
					nIndex = pListCtrl->GetNextSelectedItem(pos);
					if (nIndex > -1)
						pListCtrl->SetItemText(nIndex, 0, pszValue);
					else
					{
						LV_ITEM lvItem;
						lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_IMAGE;
						lvItem.iItem = pListCtrl->GetItemCount() + 1;
						lvItem.iSubItem = 0;
						lvItem.pszText = (LPTSTR)pszValue;
						lvItem.iImage = -1;
						lvItem.iIndent = 0;
						pListCtrl->InsertItem(&lvItem);
					}
				}
				else
				{
					LV_ITEM lvItem;
					lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_IMAGE;
					lvItem.iItem = pListCtrl->GetItemCount() + 1;
					lvItem.iSubItem = 0;
					lvItem.pszText = (LPTSTR)pszValue;
					lvItem.iImage = -1;
					lvItem.iIndent = 0;
					pListCtrl->InsertItem(&lvItem);
				}
				break;
			}
		}
	}

	return (RSRSLT);
}

ADSRESULT AdsDcl::EndList()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs)
		return RSERR;
	return (RSRSLT);
}

ADSRESULT AdsDcl::ActionTile()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	if (!pArgs)
		return RSRSLT;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszAction = pArgs->resval.rstring;
	if (!pszAction)
		pszAction = _T("");

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl)
		return RSERR;

	CString sAction;
	sAction.Format(gpszOnActionEventLispFunction, pszKey, pszAction);

	switch (pCtrl->GetType())
	{
	case CtlTextButton:
	case CtlCheckBox:
	case CtlGraphicButton:
	case CtlOptionButton:
	case CtlSlideView:
		pCtrl->SetStringProperty(Prop::EventClicked, sAction);
		break;
	case CtlTextBox:
		pCtrl->SetStringProperty(Prop::EventEditChanged, sAction);
		break;
	case CtlGrid:
	case CtlListView:
		pCtrl->SetStringProperty(Prop::EventClicked, sAction);
		break;
	case CtlComboBox:
	case CtlListBox:
		pCtrl->SetStringProperty(Prop::EventSelChanged, sAction);
		break;
	case CtlScrollBar:
		pCtrl->SetStringProperty(Prop::EventScroll, sAction);
		break;
	}

	TCHAR lpPathBuffer[4096];
	::GetTempPath(4096, lpPathBuffer);
	CString sTempFile = lpPathBuffer;
	sTempFile += gpszDclEventsLspFileName;

	CStdioFile tempFile(sTempFile, CFile::modeWrite);
	tempFile.SeekToEnd();
	tempFile.WriteString(sAction);
	tempFile.Close();

	acedRetT();
	return (RSRSLT);
}

ADSRESULT AdsDcl::SetTile()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	if (!pArgs)
		return RSRSLT;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszValue = pArgs->resval.rstring;
	if (!pszValue)
		pszValue = _T("");

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl)
		return RSERR;

	pCtrl->SetStringProperty(Prop::Text, pszValue);
	if (pCtrl->GetWindow())
		pCtrl->GetWindow()->SetWindowText(pszValue);

	return (RSRSLT);
}

ADSRESULT AdsDcl::ModeTile()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	if (!pArgs)
		return RSERR;

	if (pArgs->restype != RTSHORT)
		return RSERR;
	int nValue = pArgs->resval.rint;

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl)
		return RSERR;

	if (pCtrl->GetWindow() == NULL)
	{
		switch (nValue)
		{
		case 0:
			pCtrl->SetBooleanProperty(Prop::Enabled, true);
			break;
		case 1:
			pCtrl->SetBooleanProperty(Prop::Enabled, false);
			break;
		}
	}
	else
	{
		switch (nValue)
		{
		case 0:
			pCtrl->SetBooleanProperty(Prop::Enabled, true);
			pCtrl->GetWindow()->EnableWindow(TRUE);
			break;
		case 1:
			pCtrl->SetBooleanProperty(Prop::Enabled, false);
			pCtrl->GetWindow()->EnableWindow(FALSE);
			break;
		case 2:
			pCtrl->GetWindow()->SetFocus();
			break;
		case 3:
			if (pCtrl->GetType() == CtlTextBox)
			{
				CEdit *pEdit = (CEdit*)pCtrl->GetWindow();
				pEdit->SetSel(0, -1);
			}
			break;
		case 4:
			if (pCtrl->GetType() == CtlSlideView)
			{
				CArxAcadSlideCtrl *pSlide = (CArxAcadSlideCtrl*)pCtrl->GetWindow();
				if (CAcadColorService::IsTransparentColor(pSlide->GetHighlight()))
					pSlide->SetHighlight(RGB(255, 0, 0));
				else
					pSlide->RemoveHighlight();
			}
			break;
		}
	}

	return (RSRSLT);
}

ADSRESULT AdsDcl::GetTile()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl)
		return RSERR;

	if (pCtrl->GetWindow() == NULL)
		acedRetStr(pCtrl->GetStringProperty(Prop::Text));
	else
	{
		CString sValue;
		pCtrl->GetWindow()->GetWindowText(sValue);
		acedRetStr(sValue);
	}

	return (RSRSLT);
}

ADSRESULT AdsDcl::GetAttr()
{
	struct resbuf *pArgs = acedGetArgs();
	(void)pArgs;
	theWorkspace.DisplayAlert(_T("(dcl_Get_Attr) is not implemented yet!"));
	return (RSRSLT);
}

ADSRESULT AdsDcl::StartImage()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl || !pCtrl->GetWindow())
		return RSERR;

	((CArxAcadSlideCtrl*)pCtrl->GetWindow())->Clear();
	return (RSRSLT);
}

ADSRESULT AdsDcl::VectorImage()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nStartX = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nStartY = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nEndX = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nEndY = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nLineColor = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (pArgs)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl || !pCtrl->GetWindow())
		return RSERR;

	switch (nLineColor)
	{
	case -2: nLineColor = -22; break;
	case -15: nLineColor = -16; break;
	case -16: nLineColor = -9; break;
	case -18: nLineColor = -17; break;
	}

	CArxAcadSlideCtrl* pDlgControl = (CArxAcadSlideCtrl*)pCtrl->GetWindow();
	pDlgControl->DrawLine(pDlgControl->FromDIP(nStartX), pDlgControl->FromDIP(nStartY),
		pDlgControl->FromDIP(nEndX), pDlgControl->FromDIP(nEndY), nLineColor);

	return (RSRSLT);
}

ADSRESULT AdsDcl::FillImage()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nStartX = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nStartY = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nEndX = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nEndY = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nLineColor = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (pArgs)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl || !pCtrl->GetWindow())
		return RSERR;

	switch (nLineColor)
	{
	case -2: nLineColor = -22; break;
	case -15: nLineColor = -16; break;
	case -16: nLineColor = -9; break;
	case -18: nLineColor = -17; break;
	}

	CArxAcadSlideCtrl* pDlgControl = (CArxAcadSlideCtrl*)pCtrl->GetWindow();
	pDlgControl->DrawFillRect(pDlgControl->FromDIP(nStartX), pDlgControl->FromDIP(nStartY),
		pDlgControl->FromDIP(nStartX + nEndX), pDlgControl->FromDIP(nStartY + nEndY), nLineColor);

	return (RSRSLT);
}

ADSRESULT AdsDcl::DimXTile()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl || !pCtrl->GetWindow())
		return RSERR;

	CArxAcadSlideCtrl* pDlgControl = (CArxAcadSlideCtrl*)pCtrl->GetWindow();
	CRect rc;
	pDlgControl->GetClientRect(&rc);
	acedRetInt(pDlgControl->ToDIP(rc.Width()));
	return (RSRSLT);
}

ADSRESULT AdsDcl::DimYTile()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl || !pCtrl->GetWindow())
		return RSERR;

	CArxAcadSlideCtrl* pDlgControl = (CArxAcadSlideCtrl*)pCtrl->GetWindow();
	CRect rc;
	pDlgControl->GetClientRect(&rc);
	acedRetInt(pDlgControl->ToDIP(rc.Height()));
	return (RSRSLT);
}

ADSRESULT AdsDcl::EndImage()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;

	if (pArgs->rbnext)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl || !pCtrl->GetWindow())
		return RSERR;

	((CArxAcadSlideCtrl*)pCtrl->GetWindow())->Snapshot();
	return (RSRSLT);
}

ADSRESULT AdsDcl::SlideImage()
{
	struct resbuf *pArgs = acedGetArgs();
	if (pArgs == NULL)
		return RSERR;

	if (pArgs->restype != RTSTR)
		return RSERR;
	LPCTSTR pszKey = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nX = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nY = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nWidth = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSHORT)
		return RSERR;
	int nHeight = pArgs->resval.rint;
	pArgs = pArgs->rbnext;

	if (!pArgs || pArgs->restype != RTSTR)
		return RSERR;
	CString sFilename = pArgs->resval.rstring;
	pArgs = pArgs->rbnext;

	if (pArgs)
		return RSERR;

	TDclControlPtr pCtrl = FindCurrentControl(pszKey);
	if (!pCtrl || !pCtrl->GetWindow())
		return RSERR;

	CArxAcadSlideCtrl* pDlgControl = (CArxAcadSlideCtrl*)pCtrl->GetWindow();
	int nBracket = sFilename.Find(_T('('));
	if (nBracket == -1)
	{
		static const LPCTSTR pszSlideFileExt = _T(".sld");
		if (sFilename.Right(4).CompareNoCase(pszSlideFileExt) != 0)
			sFilename += pszSlideFileExt;
		CString sPath = theWorkspace.FindFile(sFilename);
		if (sPath.IsEmpty())
		{
			acedRetInt(-1);
			return RSRSLT;
		}
		pDlgControl->DrawASlide(pDlgControl->FromDIP(nX), pDlgControl->FromDIP(nY),
			pDlgControl->FromDIP(nWidth), pDlgControl->FromDIP(nHeight), sPath, NULL);
	}
	else
	{
		CString sLibName = sFilename.Mid(nBracket + 1, sFilename.GetLength() - nBracket - 2);
		sFilename = sFilename.Left(nBracket - 1);
		static const LPCTSTR pszSlideLibFileExt = _T(".slb");
		if (sFilename.Right(4).CompareNoCase(pszSlideLibFileExt) != 0)
			sFilename += pszSlideLibFileExt;
		CString sPath = theWorkspace.FindFile(sFilename);
		if (sPath.IsEmpty())
		{
			acedRetInt(-1);
			return RSRSLT;
		}
		pDlgControl->DrawASlide(pDlgControl->FromDIP(nX), pDlgControl->FromDIP(nY),
			pDlgControl->FromDIP(nWidth), pDlgControl->FromDIP(nHeight), sPath, sLibName);
	}
	acedRetT();
	return (RSRSLT);
}
