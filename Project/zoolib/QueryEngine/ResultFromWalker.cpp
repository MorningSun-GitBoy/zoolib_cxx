/* -------------------------------------------------------------------------------------------------
Copyright (c) 2010 Andrew Green
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

#include "zoolib/QueryEngine/ResultFromWalker.h"

#include "zoolib/Default.h"

#include "zoolib/ZMACRO_foreach.h"

namespace ZooLib {
namespace QueryEngine {

using std::map;
using std::set;
using std::vector;
using RelationalAlgebra::RelHead;

// =================================================================================================
#pragma mark - sQuery

ZP<Result> sResultFromWalker(ZP<Walker> iWalker)
	{
	map<string8,size_t> offsets;
	size_t baseOffset = 0;
	iWalker = iWalker->Prime(sDefault(), offsets, baseOffset);

	vector<Val_DB> thePackedRows;
	vector<Val_DB> theRow(baseOffset);
	for (;;)
		{
		if (not iWalker->QReadInc(&theRow[0]))
			break;

		foreacha (entry, offsets)
			thePackedRows.push_back(theRow[entry.second]);
		}

	RelHead theRelHead;
	foreacha (entry, offsets)
		theRelHead.insert(entry.first);

	return new Result(&theRelHead, &thePackedRows);
	}

} // namespace QueryEngine
} // namespace ZooLib
