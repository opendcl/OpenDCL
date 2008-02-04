// R2006AcUiMRUComboBoxFixup.h : header file
//

#pragma once

typedef CAcUiColorComboBox CAcUiColorComboBoxOriginal;
typedef CAcUiLineWeightComboBox CAcUiLineWeightComboBoxOriginal;
typedef CAcUiArrowHeadComboBox CAcUiArrowHeadComboBoxOriginal;
typedef CAcUiPlotStyleNamesComboBox CAcUiPlotStyleNamesComboBoxOriginal;
typedef CAcUiPlotStyleTablesComboBox CAcUiPlotStyleTablesComboBoxOriginal;


// In AutoCAD 2006, Autodesk added a new member to CAcUiMRUComboBox (m_pOldPalette),
// causing the binary layout of the class to change. Since OpenDCL is built using
// the ObjectARX 2004 SDK, this causes the code to fail when running in AutoCAD
// 2006. This hack provides space for the extra member and adds any needed accessor
// functions to access the shifted members based on a runtime check of the acui16.dll
// version. [ORW]
template< typename TAcUiMRUComboBoxBase >
class CAcUiMRUComboBoxFixup : public TAcUiMRUComboBoxBase
{
#if (_ACADTARGET == 16)
	CPalette* __padding; //add space for the 2006 member in case it's needed
	static bool isR2006() 
	{
		static bool b2006 =
			(GetProcAddress( GetModuleHandle( _T("acui16.dll") ),
											 "??1CAcUiLineTypeComboBox@@UAE@XZ" ) != NULL);
		return b2006;
	}
public:
	void SetMRULen( int nLen )
	{
		if( !isR2006() )
			m_mruLen = nLen;
		else
			*(int*)(((BYTE*)&m_mruLen) + sizeof(__padding)) = nLen;
	}
#else //(_ACADTARGET == 16)
public:
	void SetMRULen( int nLen )
	{
		m_mruLen = nLen;
	}
#endif //(_ACADTARGET == 16)
public:
	CAcUiMRUComboBoxFixup() {}
	virtual ~CAcUiMRUComboBoxFixup() {}
};
#define CAcUiColorComboBox CAcUiMRUComboBoxFixup< CAcUiColorComboBoxOriginal >
#define CAcUiLineWeightComboBox CAcUiMRUComboBoxFixup< CAcUiLineWeightComboBoxOriginal >
#define CAcUiArrowHeadComboBox CAcUiMRUComboBoxFixup< CAcUiArrowHeadComboBoxOriginal >
#define CAcUiPlotStyleNamesComboBox CAcUiMRUComboBoxFixup< CAcUiPlotStyleNamesComboBoxOriginal >
#define CAcUiPlotStyleTablesComboBox CAcUiMRUComboBoxFixup< CAcUiPlotStyleTablesComboBoxOriginal >
