class CStdDropSource : public COleDropSource
{
public:
	virtual SCODE QueryContinueDrag(BOOL bEscapePressed, DWORD dwKeyState);
	virtual SCODE GiveFeedback(DROPEFFECT dropEffect);
};
