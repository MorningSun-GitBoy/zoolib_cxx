/* -------------------------------------------------------------------------------------------------
Copyright (c) 2014 Andrew Green
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

#ifndef __ZooLib_Util_Chan_h__
#define __ZooLib_Util_Chan_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"
#include "zoolib/ChanW.h"
#include "zoolib/ChanPos.h"
#include "zoolib/ChanSizeSet.h"

namespace ZooLib {

inline void sTruncate(const ChanSizeSet& iChanSizeSet, const ChanPos& iChanPos)
	{
	sSizeSet(0, iChanSizeSet);
	sSetPos(0, iChanPos);
	}

// =================================================================================================
#pragma mark -

template <class Elmt_t>
std::pair<uint64,uint64> sCopyFully(
	const ChanR<Elmt_t>& iChanR, const ChanW<Elmt_t>& iChanW, uint64 iCount)
	{
	Elmt_t buf[sStackBufferSize / sizeof(Elmt_t)];

	for (uint64 countRemaining = iCount; /*no test*/; /*no inc*/)
		{
		if (const size_t countRead = sQRead(
			buf,
			std::min<size_t>(countRemaining, countof(buf)),
			iChanR))
			{
			const size_t countWritten = sQWriteFully(buf, countRead, iChanW);

			if (countWritten == countRead)
				{
				countRemaining -= countRead;
				// Here's where we return to the beginning of the loop.
				// In all other cases we exit.
				continue;
				}

			return std::pair<uint64,uint64>(
				iCount - countRemaining + countRead,
				iCount - countRemaining + countWritten);
			}

		return std::pair<uint64,uint64>(
			iCount - countRemaining,
			iCount - countRemaining);
		}
	}

template <class Elmt_t>
std::pair<uint64,uint64> sCopyAll(const ChanR<Elmt_t>& iChanR, const ChanW<Elmt_t>& iChanW)
	{
	Elmt_t buf[sStackBufferSize / sizeof(Elmt_t)];

	uint64 totalCopied = 0;
	for (;;)
		{
		if (const size_t countRead = sQRead(buf, countof(buf), iChanR))
			{
			const size_t countWritten = sQWriteFully(buf, countRead, iChanW);

			if (countWritten == countRead)
				{
				totalCopied += countRead;
				continue;
				}

			return std::pair<uint64,uint64>(totalCopied + countRead, totalCopied + countWritten);
			}

		return std::pair<uint64,uint64>(totalCopied, totalCopied);
		}
	}

// Not sure about the XX_Until name -- it made sense in ZUtil_Strim, maybe not so much elsewhere.
template <class Elmt_t>
bool sCopy_Until(const ChanR<Elmt_t>& iChanR, const Elmt_t& iTerminator, const ChanW<Elmt_t>& iChanW)
	{
	for (;;)
		{
		if (ZQ<Elmt_t,false> theQ = sQRead(iChanR))
			return false;
		else if (iTerminator == *theQ)
			return true;
		else
			sWriteMust(*theQ, iChanW);
		}
	}

template <class Elmt_t>
bool sSkip_Until(const ChanR<Elmt_t>& iChanR, const Elmt_t& iTerminator)
	{ return sCopy_Until(iChanR, iTerminator, ChanW_XX_Discard<Elmt_t>()); }

} // namespace ZooLib

#endif // __ZooLib_Util_Chan_h__
