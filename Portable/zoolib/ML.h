/* -------------------------------------------------------------------------------------------------
Copyright (c) 2004 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_ML_h__
#define __ZooLib_ML_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/ChanRU_UTF.h"
#include "zoolib/Name.h"

#include <vector>

namespace ZooLib {
namespace ML {

typedef std::pair<Name, std::string> Attr_t;
typedef std::vector<Attr_t> Attrs_t;

typedef Callable<std::string(std::string)> Callable_Entity;

// =================================================================================================
#pragma mark - ML

std::string sReadReference(const ChanRU_UTF& iChanRU, const ZP<Callable_Entity>& iCallable);

bool sReadIdentifier(const ChanRU_UTF& iChanRU, std::string& oText);

bool sReadAttributeName(const ChanRU_UTF& iChanRU, std::string& oName);

bool sReadAttributeValue(
	const ChanRU_UTF& iChanRU,
	bool iRecognizeEntities, const ZP<Callable_Entity>& iCallable,
	std::string& oValue);

} // namespace ML
} // namespace ZooLib

#endif // __ZooLib_ML_h__
