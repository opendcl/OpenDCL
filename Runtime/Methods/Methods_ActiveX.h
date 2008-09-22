// Methods_ActiveX.h : header file
//

#pragma once


namespace AxControl
{
ADSRESULT Get();
ADSRESULT Put();
ADSRESULT Invoke();
ADSRESULT GetAxObject();
};

namespace AxObject
{
ADSRESULT Get();
ADSRESULT Put();
ADSRESULT Invoke();
ADSRESULT Release();
};

namespace AxGeneral
{
ADSRESULT GetOlePictureFromId();
ADSRESULT GetOlePictureFromFile();
};
