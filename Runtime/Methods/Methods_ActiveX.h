// Methods_ActiveX.h : header file
//

#pragma once


namespace AxControl
{
ADSRESULT GetProperty();
ADSRESULT SetProperty();
ADSRESULT DoMethod();
ADSRESULT GetOleObject();
};

namespace AxObject
{
ADSRESULT GetProperty();
ADSRESULT SetProperty();
ADSRESULT DoMethod();
ADSRESULT Close();
};

namespace AxGeneral
{
ADSRESULT GetOlePictureFromId();
ADSRESULT GetOlePictureFromFile();
};
