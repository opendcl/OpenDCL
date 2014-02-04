#pragma once


#include "RefCountedPtr.h"
#include "DclPicture.h"

typedef RefCountedPtr< class CProject > TProjectPtr;
typedef LockedPtr< class CProject > TProjectLockedPtr;

typedef RefCountedPtr< class CDclFormObject > TDclFormPtr;
typedef LockedPtr< class CDclFormObject > TDclFormLockedPtr;

typedef RefCountedPtr< class CDclControlTemplate > TDclControlPtr;
typedef LockedPtr< class CDclControlTemplate > TDclControlLockedPtr;

typedef RefCountedPtr< class CPropertyObject > TPropertyPtr;
typedef LockedPtr< class CPropertyObject > TPropertyLockedPtr;

typedef RefCountedPtr< class CDclPicture > TPicturePtr;
typedef LockedPtr< class CDclPicture > TPictureLockedPtr;
