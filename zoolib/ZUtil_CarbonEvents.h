/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES
OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------------------------- */

#ifndef __ZUtil_CarbonEvents__
#define __ZUtil_CarbonEvents__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_SPI.h"

#if ZCONFIG_SPI_Enabled(Carbon)

#include ZMACINCLUDE3(Carbon,HIToolbox,CarbonEvents.h)

#include <string>

NAMESPACE_ZOOLIB_BEGIN

// =================================================================================================
#pragma mark -
#pragma mark * ZUtil_CarbonEvents

namespace ZUtil_CarbonEvents {

bool sGetParam(EventRef iEventRef, EventParamName iName, EventParamType iType,
	size_t iBufferSize, void* iBuffer);
  
size_t sGetParamLength(EventRef iEventRef, EventParamName iName, EventParamType iType);

template <typename T>
bool sGetParam_T(EventRef iEventRef, EventParamName iName, EventParamType iType, T& oT)
	{
	return sGetParam(iEventRef, iName, iType, sizeof(oT), &oT);
	}

template <typename T>
T sGetParam_T(EventRef iEventRef, EventParamName iName, EventParamType iType)
	{
	T theT;
	if (sGetParam(iEventRef, iName, iType, sizeof(theT), &theT))
		return theT;
	return T();
	}

void sSetParam(EventRef iEventRef, EventParamName iName, EventParamType iType,
	size_t iBufferSize, const void* iBuffer);

template <typename T>
void sSetParam_T(EventRef iEventRef, EventParamName iName, EventParamType iType, const T& iParam)
	{
	sSetParam(iEventRef, iName, iType, sizeof(T), &iParam);
	}

typedef void (*Callback_t)(void* iRefcon);

void sInvokeOnMainThread(Callback_t iCallback, void* iRefcon);

std::string sEventAsString(EventClass iEC, EventKind iEK);

std::string sEventTypeAsString(UInt16 iEventType);

} // namespace ZUtil_CarbonEvents

NAMESPACE_ZOOLIB_END


#if defined(UNIVERSAL_INTERFACES_VERSION) && UNIVERSAL_INTERFACES_VERSION <= 0x0341
// Taken from Universal Headers 3.4.2
enum {

  /*
   * This event parameter may be added to any event that is posted to
   * the main event queue. When the event is removed from the queue and
   * sent to the event dispatcher, the dispatcher will retrieve the
   * EventTargetRef contained in this parameter and send the event
   * directly to that event target. If this parameter is not available
   * in the event, the dispatcher will send the event to a suitable
   * target, or to the application target if no more specific target is
   * appropriate. Available in CarbonLib 1.3.1 and later, and Mac OS X.
   */
  kEventParamPostTarget         = FOUR_CHAR_CODE('ptrg'), /* typeEventTargetRef*/

  /*
   * Indicates an event parameter of type EventTargetRef.
   */
  typeEventTargetRef            = FOUR_CHAR_CODE('etrg') /* EventTargetRef*/
};
#endif

#endif // ZCONFIG_SPI_Enabled(Carbon)


#endif // __ZUtil_CarbonEvents__
