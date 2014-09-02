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

#ifndef __ZooLib_ChanR_XX_Cat_h__
#define __ZooLib_ChanR_XX_Cat_h__ 1
#include "zconfig.h"

#include "zoolib/ChanR.h"

namespace ZooLib {

// =================================================================================================
// MARK: - ChanR

template <class XX>
class ChanR_XX_Cat
:	public ChanR<XX>
	{
public:
	typedef XX Elmt;

	ChanR_XX_Cat(const ChanR<XX>& iChanR0, const ChanR<XX>& iChanR1)
	:	fChanR0(iChanR0)
	,	fChanR1(iChanR1)
	,	fFirstIsLive(true)
		{}

	virtual size_t Read(Elmt* oDest, size_t iCount)
		{
		if (fFirstIsLive)
			{
			if (size_t countRead = sRead(oDest, iCount, fChanR0))
				return countRead;
			fFirstIsLive = false;
			}
		return sRead(oDest, iCount, fChanR1);
		}

	virtual uint64 Skip(uint64 iCount)
		{
		if (fFirstIsLive)
			return sSkip(iCount, fChanR0);
		return sSkip(iCount, fChanR1);
		}

	virtual size_t Readable()
		{
		if (fFirstIsLive)
			return sReadable(fChanR0);
		return sReadable(fChanR1);
		}

protected:
	const ChanR<XX>& fChanR0;
	const ChanR<XX>& fChanR1;
	bool fFirstIsLive;
	};

} // namespace ZooLib

#endif // __ZooLib_ChanR_XX_Cat_h__