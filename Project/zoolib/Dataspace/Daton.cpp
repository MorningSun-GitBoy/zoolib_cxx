/* -------------------------------------------------------------------------------------------------
Copyright (c) 2016 Andrew Green
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

#include "zoolib/Dataspace/Daton.h"

#include "zoolib/Compare.h"

// =================================================================================================
#pragma mark - sCompare_T

namespace ZooLib {

template <>
int sCompare_T<Dataspace::Daton>(const Dataspace::Daton& iL, const Dataspace::Daton& iR)
	{ return sCompare_T(iL.GetData(), iR.GetData()); }

ZMACRO_CompareRegistration_T(Dataspace::Daton)

} // namespace ZooLib

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - Daton

Daton::Daton()
	{}

Daton::Daton(const Daton& iOther)
:	fData(iOther.fData)
	{}

Daton::~Daton()
	{}

Daton& Daton::operator=(const Daton& iOther)
	{
	fData = iOther.fData;
	return *this;
	}

Daton::Daton(Data_ZZ iData)
:	fData(iData)
	{}

bool Daton::operator==(const Daton& iOther) const
	{ return fData == iOther.fData; }

bool Daton::operator<(const Daton& iOther) const
	{ return fData < iOther.fData; }

Data_ZZ Daton::GetData() const
	{ return fData; }

} // namespace Dataspace
} // namespace ZooLib
