/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
http://www.zoolib.org

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,Publish, distribute,
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

#ifndef __ZThreadValue__
#define __ZThreadValue__ 1
#include "zconfig.h"

#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadValue

template <class T, class Tag = T>
class ZThreadValue
	{
public:
	ZThreadValue()
	:	fValue(T())
	,	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	ZThreadValue(const ZThreadValue& iOther)
	:	fValue(iOther.fValue)
	,	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	~ZThreadValue()
		{ ZTSS::sSet(spKey(), fPrior); }

	ZThreadValue& operator=(const ZThreadValue& iOther)
		{
		fValue = iOther.fValue;
		return *this;
		}

	ZThreadValue(const T& iValue)
	:	fValue(iValue)
	,	fPrior(spGet())
		{ ZTSS::sSet(spKey(), this); }

	ZThreadValue& operator=(const T& iValue)
		{
		fValue = iValue;
		return *this;
		}

	T& GetMutable()
		{ return fValue; }

	const T& Get() const
		{ return fValue; }

	void Set(const T& iValue) const
		{ fValue = iValue; }

	//--

	static T& sGetMutable()
		{ return spGet()->GetMutable(); }

	static ZQ<T> sQGet()
		{
		if (ZThreadValue* theTV = spGet())
			return theTV->Get();
		return null;
		}

	static T sDGet(const T& iDefault)
		{
		if (ZThreadValue* theTV = spGet())
			return theTV->Get();
		return iDefault;
		}

	static T sGet()
		{
		if (ZThreadValue* theTV = spGet())
			return theTV->Get();
		return T();
		}

private:
	static ZThreadValue* spGet()
		{ return ((ZThreadValue*)(ZTSS::sGet(spKey()))); }

	static ZTSS::Key& spKey()
		{
		// This method is a sneaky way to have static storage for a template class.
		static ZTSS::Key spKey = ZTSS::sCreate();
		return spKey;
		}

	T fValue;
	ZThreadValue* fPrior;
	};

} // namespace ZooLib

#endif // __ZThreadValue__
