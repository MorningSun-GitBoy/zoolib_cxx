/* -------------------------------------------------------------------------------------------------
Copyright (c) 2019 Andrew Green
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

#include "zoolib/Dataspace/ResultHandler.h"

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - ResultHandler

ResultHandler::ResultHandler(const ZRef<Callable_t>& iCallable)
:	fCallable(iCallable)
	{}

ZQ<void> ResultHandler::QCall(
	const ZRef<ZCounted>& iRegistration,
	int64 iChangeCount,
	const ZRef<Result>& iResult,
	const ZRef<ResultDeltas>& iResultDeltas)
	{
	if (iResult)
		{
		fResult = iResult;
		}
	else
		{
		ZAssert(iResultDeltas && fResult);

		fResult = fResult->Fresh();

		const size_t theColCount = fResult->GetRelHead().size();
		for (size_t xx = 0; xx < iResultDeltas->fMapping.size(); ++xx)
			{
			const size_t target = iResultDeltas->fMapping[xx];

			std::copy_n(&iResultDeltas->fPackedRows[xx * theColCount],
				theColCount,
				&fResult->fPackedRows[target * theColCount]);
			}
		}
	sCall(fCallable, iRegistration, fResult);

	return notnull;
	}

} // namespace Dataspace
} // namespace ZooLib