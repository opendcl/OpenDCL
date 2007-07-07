// StringCompare.h : header file
//

#pragma once


inline bool CompareStrings(CString sArg1, CString sArg2) { return (sArg1.CompareNoCase(sArg2) == 0); }
