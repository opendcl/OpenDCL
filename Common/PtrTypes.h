#pragma once


#include "RefCountedPtr.h"

typedef RefCountedPtr< class CProject > TProjectPtr;
typedef LockedPtr< class CProject > TProjectLockedPtr;

typedef RefCountedPtr< class CDclFormObject > TDclFormPtr;
typedef LockedPtr< class CDclFormObject > TDclFormLockedPtr;

typedef RefCountedPtr< class CDclControlObject > TDclControlPtr;
typedef LockedPtr< class CDclControlObject > TDclControlLockedPtr;

typedef RefCountedPtr< class CPropertyObject > TPropertyPtr;
typedef LockedPtr< class CPropertyObject > TPropertyLockedPtr;

typedef RefCountedPtr< class CPictureObject > TPicturePtr;
typedef LockedPtr< class CPictureObject > TPictureLockedPtr;
