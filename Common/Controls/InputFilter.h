// InputFilter.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CInputFilter class

class CInputFilter
{
	CString msLastValidInput;

public:
	CInputFilter() {}
	virtual ~CInputFilter() {}

public:
	virtual const CString& GetFilter() const = 0; //return string of acceptable characters
	virtual bool FilterInput( TCHAR& chInput ) const //return false to disallow character, else fix it up
		{
			if( GetFilter().Find( chInput ) < 0 )
				return false;
			return true;
		}
	virtual bool FilterInput( CString& sInput ) const //return false to discard input, else fix it up
		{
			if( sInput.SpanIncluding( GetFilter() ) != sInput )
				return false;
			return true;
		}
	virtual bool ConvertForDisplay( CString& sRaw ) const //convert raw text to displayable string
		{
			return true;
		}
	virtual bool OnValidateInput( CString& sInput ) //return false to reject final input
		{
			if( !FilterInput( sInput ) )
				return false;
			msLastValidInput = sInput;
			return true;
		}
	virtual LPCTSTR GetLastValidInput() const //return last valid input
		{
			return msLastValidInput;
		}
	virtual LPCTSTR OnBadInput() const //warn user and return replacement input (default: beep and return last valid input)
		{
			MessageBeep( (UINT)-1 );
			return GetLastValidInput();
		}
	virtual bool IsMaskedInput() const { return false; } //return true for password input
};
