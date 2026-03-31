class CStdDropSource : public COleDropSource
{
public:
	SCODE QueryContinueDrag(BOOL bEscapePressed, DWORD dwKeyState) override;
	SCODE GiveFeedback(DROPEFFECT dropEffect) override;
};
