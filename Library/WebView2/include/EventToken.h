// Win8+ Windows SDK header. Vendored so CAD toolsets that still use the
// Win7 / XP SDK (v100, v90, v141_xp, ...) can compile WebView2.h.
// ABI matches SDK <EventToken.h> (EventRegistrationToken is an int64).
#pragma once

#ifndef __eventtoken_h__
#define __eventtoken_h__

#ifndef EVENTTOKEN_DEFINED
#define EVENTTOKEN_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EventRegistrationToken
{
	__int64 value;
} EventRegistrationToken;

#ifdef __cplusplus
}
#endif

#endif /* EVENTTOKEN_DEFINED */

#endif /* __eventtoken_h__ */
