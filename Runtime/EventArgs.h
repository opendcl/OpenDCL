// EventArgs.h : header file
//

#pragma once

#include "ArxWorkspace.h"


class arg_b
{
public:
	arg_b() {};
	virtual ~arg_b() {};
	virtual resbuf* asResbuf() const { return NULL; }
	virtual CString asString() const { return _T(""); }
};
typedef arg_b arg_null;


template < typename _t > class arg_t;

template<> class arg_t< bool > : arg_b
{
	const bool _arg;
public:
	typedef bool type;
	arg_t( bool arg ) : _arg( arg ) {}
	virtual resbuf* asResbuf() const
		{
			return acutNewRb( _arg? RTT : RTNIL );
		}
	virtual CString asString() const
		{
			return _arg? _T(" T") : _T(" NIL");
		}
};

template<> class arg_t< int > : arg_b
{
	const int _arg;
public:
	typedef int type;
	arg_t( int arg ) : _arg( arg ) {}
	virtual resbuf* asResbuf() const
		{
			resbuf* prb = acutNewRb( RTLONG );
			if( !prb )
				return NULL;
			prb->resval.rlong = static_cast<long>(_arg);
			return prb;
		}
	virtual CString asString() const
		{
			CString sArg;
			sArg.Format( _T(" %d"), _arg );
			return sArg;
		}
};

template<> class arg_t< DWORD_PTR > : arg_b
{
	const DWORD_PTR _arg;
public:
	typedef DWORD_PTR type;
	arg_t( DWORD_PTR arg ) : _arg( arg ) {}
	virtual resbuf* asResbuf() const
		{
			if( !_arg )
				return acutNewRb( RTNIL );
			resbuf* prb = acutNewRb( RTENAME );
			if( !prb )
				return NULL;
			prb->resval.rlname[0] = _arg;
			prb->resval.rlname[1] = odcl::ptrHandle;
			return prb;
		}
	virtual CString asString() const
		{
			CString sArg;
			sArg.Format( _T(" %.1f"), (double)_arg );
			return sArg;
			//ads_real rArg = static_cast<ads_real>(_arg);
			//TCHAR szArg[32] = _T(""); 
			//if( RTNORM == acdbRToS( rArg, 2, 20, szArg ) )
			//	return CString( _T(' ') ) + szArg;
			//return _T(" 0");
		}
};

template<> class arg_t< CPoint > : arg_b
{
	const CPoint _arg;
public:
	typedef CPoint type;
	arg_t( const CPoint& arg ) : _arg( arg ) {}
	virtual resbuf* asResbuf() const
		{
			resbuf* prb = acutNewRb( RTPOINT );
			if( !prb )
				return NULL;
			prb->resval.rpoint[0] = _arg.x;
			prb->resval.rpoint[1] = _arg.y;
			prb->resval.rpoint[2] = 0;
			return prb;
		}
	virtual CString asString() const
		{
			CString sArg;
			sArg.Format( _T(" '(%d %d)"), _arg.x, _arg.y );
			return sArg;
		}
};

template<> class arg_t< acedDwgPoint > : arg_b
{
	acedDwgPoint _arg;
public:
	typedef acedDwgPoint type;
	arg_t( const acedDwgPoint arg ) { _arg[0] = arg[0]; _arg[1] = arg[1]; _arg[2] = arg[2]; }
	virtual resbuf* asResbuf() const
		{
			resbuf* prb = acutNewRb( RT3DPOINT );
			if( !prb )
				return NULL;
			prb->resval.rpoint[0] = _arg[0];
			prb->resval.rpoint[1] = _arg[1];
			prb->resval.rpoint[2] = _arg[2];
			return prb;
		}
	virtual CString asString() const
		{
			CString sArg;
			sArg.Format( _T(" '(%.8g %.8g %.8g)"), _arg[0], _arg[1], _arg[2] );
			return sArg;
		}
};

template<> class arg_t< LPCTSTR > : arg_b
{
	const LPCTSTR _arg;
public:
	typedef LPCTSTR type;
	arg_t( LPCTSTR arg ) : _arg( arg ) {}
	virtual resbuf* asResbuf() const
		{
			resbuf* prb = acutNewRb( RTSTR );
			if( !prb )
				return NULL;
			if( Acad::eOk != acutNewString( _arg, prb->resval.rstring ) )
			{
				delete prb;
				return NULL;
			}
			return prb;
		}
	virtual CString asString() const
		{
			CString sArg( _arg );
			sArg.Replace( _T("\\"), _T("\\\\") );
			sArg.Replace( _T("\""), _T("\\\"") );
			sArg.Replace( _T("\r"), _T("\\r") );
			sArg.Replace( _T("\n"), _T("\\n") );
			sArg.Replace( _T("\t"), _T("\\t") );
			CString sResult = _T(" \"");
			sResult += sArg;
			sResult += _T("\"");
			return sResult;
		}
};

template<> class arg_t< TCHAR > : arg_b
{
	const TCHAR _arg;
public:
	typedef TCHAR type;
	arg_t( TCHAR arg ) : _arg( arg ) {}
	virtual resbuf* asResbuf() const
		{
			resbuf* prb = acutNewRb( RTSTR );
			if( !prb )
				return NULL;
			TCHAR sz[2] = { _arg, _T('\0') };
			if( Acad::eOk != acutNewString( sz, prb->resval.rstring ) )
			{
				delete prb;
				return NULL;
			}
			return prb;
		}
	virtual CString asString() const
		{
			CString sArg( _arg );
			sArg.Replace( _T("\\"), _T("\\\\") );
			sArg.Replace( _T("\""), _T("\\\"") );
			sArg.Replace( _T("\r"), _T("\\r") );
			sArg.Replace( _T("\n"), _T("\\n") );
			sArg.Replace( _T("\t"), _T("\\t") );
			if( sArg.GetLength() == 1 && _arg < _T(' '))
				sArg.Format( _T("\\\\%.3o"), _arg );
			CString sResult = _T(" \"");
			sResult += sArg;
			sResult += _T("\"");
			return sResult;
		}
};


template< typename t1 >
class args_1 : public arg_b
{
	const arg_t< t1 > arg1;
public:
	args_1( t1 _arg1 ) : arg1( _arg1 ) {}
	virtual resbuf* asResbuf() const { return arg1.asResbuf(); }
	virtual CString asString() const { return arg1.asString(); }
};

template< typename t1, typename t2 >
class args_2 : public arg_b
{
	const arg_t< t1 > arg1;
	const arg_t< t2 > arg2;
public:
	args_2( t1 _arg1, t2 _arg2 ) : arg1( _arg1 ), arg2( _arg2 ) {}
	virtual resbuf* asResbuf() const
		{
			resbuf* prbHead = arg1.asResbuf();
			if( prbHead )
				prbHead->rbnext = arg2.asResbuf();
			else
				prbHead = arg2.asResbuf();
			return prbHead;
		}
	virtual CString asString() const
		{
			return arg1.asString() + arg2.asString();
		}
};

template< typename t1, typename t2, typename t3 >
class args_3 : public arg_b
{
	const arg_t< t1 > arg1;
	const arg_t< t2 > arg2;
	const arg_t< t3 > arg3;
public:
	args_3( t1 _arg1, t2 _arg2, t3 _arg3 ) : arg1( _arg1 ), arg2( _arg2 ), arg3( _arg3 ) {}
	virtual resbuf* asResbuf() const
		{
			resbuf* prbArg1 = arg1.asResbuf();
			resbuf* prbArg2 = arg2.asResbuf();
			resbuf* prbArg3 = arg3.asResbuf();
			if( prbArg1 )
			{
				if( prbArg2 )
				{
					prbArg1->rbnext = prbArg2;
					prbArg2->rbnext = prbArg3;
				}
				else
					prbArg1->rbnext = prbArg3;
				return prbArg1;
			}
			if( prbArg2 )
			{
				prbArg2->rbnext = prbArg3;
				return prbArg2;
			}
			return prbArg3;
		}
	virtual CString asString() const
		{
			return arg1.asString() + arg2.asString() + arg3.asString();
		}
};

template< typename t1, typename t2, typename t3, typename t4 >
class args_4 : public arg_b
{
	const arg_t< t1 > arg1;
	const arg_t< t2 > arg2;
	const arg_t< t3 > arg3;
	const arg_t< t4 > arg4;
public:
	args_4( t1 _arg1, t2 _arg2, t3 _arg3, t4 _arg4 )
		: arg1( _arg1 ), arg2( _arg2 ), arg3( _arg3 ), arg4( _arg4 ) {}
	virtual resbuf* asResbuf() const
		{
			resbuf* prbArg1 = arg1.asResbuf();
			resbuf* prbArg2 = arg2.asResbuf();
			resbuf* prbArg3 = arg3.asResbuf();
			resbuf* prbArg4 = arg4.asResbuf();
			if( prbArg1 )
			{
				if( prbArg2 )
				{
					prbArg1->rbnext = prbArg2;
					if( prbArg3 )
					{
						prbArg2->rbnext = prbArg3;
						prbArg3->rbnext = prbArg4;
					}
					else
						prbArg2->rbnext = prbArg4;
				}
				else if( prbArg3 )
				{
					prbArg1->rbnext = prbArg3;
					prbArg3->rbnext = prbArg4;
				}
				else
					prbArg1->rbnext = prbArg4;
				return prbArg1;
			}
			if( prbArg2 )
			{
				if( prbArg3 )
				{
					prbArg2->rbnext = prbArg3;
					prbArg3->rbnext = prbArg4;
				}
				else
					prbArg2->rbnext = prbArg4;
				return prbArg2;
			}
			if( prbArg3 )
			{
				prbArg3->rbnext = prbArg4;
				return prbArg3;
			}
			return prbArg4;
		}
	virtual CString asString() const
		{
			return arg1.asString() + arg2.asString() + arg3.asString() + arg4.asString();
		}
};

template< typename t1, typename t2, typename t3, typename t4, typename t5 >
class args_5 : public arg_b
{
	const arg_t< t1 > arg1;
	const arg_t< t2 > arg2;
	const arg_t< t3 > arg3;
	const arg_t< t4 > arg4;
	const arg_t< t5 > arg5;
public:
	args_5( t1 _arg1, t2 _arg2, t3 _arg3, t4 _arg4, t5 _arg5 )
		: arg1( _arg1 ), arg2( _arg2 ), arg3( _arg3 ), arg4( _arg4 ), arg5( _arg5 ) {}
	virtual resbuf* asResbuf() const
		{
			resbuf* prbArg1 = arg1.asResbuf();
			resbuf* prbArg2 = arg2.asResbuf();
			resbuf* prbArg3 = arg3.asResbuf();
			resbuf* prbArg4 = arg4.asResbuf();
			resbuf* prbArg5 = arg5.asResbuf();
			if( prbArg1 )
			{
				if( prbArg2 )
				{
					prbArg1->rbnext = prbArg2;
					if( prbArg3 )
					{
						prbArg2->rbnext = prbArg3;
						if( prbArg4 )
						{
							prbArg3->rbnext = prbArg4;
							prbArg4->rbnext = prbArg5;
						}
						else
							prbArg3->rbnext = prbArg5;
					}
					else if( prbArg4 )
					{
						prbArg3->rbnext = prbArg4;
						prbArg4->rbnext = prbArg5;
					}
					else
						prbArg3->rbnext = prbArg5;
				}
				else if( prbArg3 )
				{
					prbArg1->rbnext = prbArg3;
					if( prbArg4 )
					{
						prbArg3->rbnext = prbArg4;
						prbArg4->rbnext = prbArg5;
					}
					else
						prbArg3->rbnext = prbArg5;
				}
				else if( prbArg4 )
				{
					prbArg1->rbnext = prbArg4;
					prbArg4->rbnext = prbArg5;
				}
				else
					prbArg1->rbnext = prbArg5;
				return prbArg1;
			}
			if( prbArg2 )
			{
				if( prbArg3 )
				{
					prbArg2->rbnext = prbArg3;
					if( prbArg4 )
					{
						prbArg3->rbnext = prbArg4;
						prbArg4->rbnext = prbArg5;
					}
					else
						prbArg3->rbnext = prbArg5;
				}
				else if( prbArg4 )
				{
					prbArg2->rbnext = prbArg4;
					prbArg4->rbnext = prbArg5;
				}
				else
					prbArg2->rbnext = prbArg5;
				return prbArg2;
			}
			if( prbArg3 )
			{
				if( prbArg4 )
				{
					prbArg3->rbnext = prbArg4;
					prbArg4->rbnext = prbArg5;
				}
				else
					prbArg3->rbnext = prbArg5;
				return prbArg3;
			}
			if( prbArg4 )
			{
				prbArg4->rbnext = prbArg5;
				return prbArg4;
			}
			return prbArg5;
		}
	virtual CString asString() const
		{
			return arg1.asString() + arg2.asString() + arg3.asString() + arg4.asString() + arg5.asString();
		}
};


typedef arg_null args_null;
typedef args_1< int > args_N;
typedef args_1< bool > args_B;
typedef args_1< LPCTSTR > args_S;
typedef args_1< DWORD_PTR > args_H;
typedef args_1< CPoint > args_P;
typedef args_2< int, int > args_NN;
typedef args_2< int, LPCTSTR > args_NS;
typedef args_2< LPCTSTR, DWORD_PTR > args_SH;
typedef args_2< LPCTSTR, LPCTSTR > args_SS;
typedef args_2< LPCTSTR, int > args_SN;
typedef args_2< acedDwgPoint, int > args_P3N;
typedef args_3< int, int, int > args_NNN;
typedef args_3< TCHAR, int, int > args_CNN;
typedef args_3< LPCTSTR, int, int > args_SNN;
typedef args_4< int, int, int, int > args_NNNN;
typedef args_4< LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR > args_SSSS;
typedef args_4< LPCTSTR, LPCTSTR, LPCTSTR, DWORD_PTR > args_SSSH;
typedef args_4< LPCTSTR, LPCTSTR, LPCTSTR, CPoint > args_SSSP;
typedef args_5< LPCTSTR, LPCTSTR, LPCTSTR, LPCTSTR, int > args_SSSSN;
typedef args_5< LPCTSTR, LPCTSTR, LPCTSTR, DWORD_PTR, int > args_SSSHN;
