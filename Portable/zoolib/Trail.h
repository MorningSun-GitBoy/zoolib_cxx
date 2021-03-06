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

#ifndef __ZooLib_Trail_h__
#define __ZooLib_Trail_h__ 1
#include "zconfig.h"

#include "zoolib/Name.h"
#include "zoolib/UnicodeString.h"
#include "zoolib/Util_Relops.h"

#include <vector>

namespace ZooLib {

// =================================================================================================
#pragma mark - Trail

class Trail
	{
	Trail(bool); // Not implemented
public:
	typedef Name Comp;

	Trail();
	Trail(const Trail& iTrail);
	~Trail();
	Trail& operator=(const Trail& iTrail);

	Trail(const string8& iPOSIXTrail);
	Trail(const char* iPOSIXTrail);
	Trail(const char* iPOSIXTrail, size_t iSize);

	Trail(const string8& iSeparator, const string8& iIgnore,
		const string8& iBounce, const string8& iTrail);

	template <class I>
	Trail(const I& iBegin, const I& iEnd)
	:	fComps(iBegin, iEnd)
		{}

	bool operator==(const Trail& iOther) const;
	bool operator<(const Trail& iOther) const;

	Trail operator+(const Trail& iTrail) const;
	Trail& operator+=(const Trail& iTrail);

	template <class I>
	void AppendTrail(const I& iBegin, const I& iEnd)
		{ fComps.insert(fComps.end(), iBegin, iEnd); }
	void AppendTrail(const Trail& iTrail);
	void AppendComp(const Comp& iComp);
	void AppendBounce();
	void AppendBounces(size_t iCount);

	template <class I>
	void PrependTrail(const I& iBegin, const I& iEnd)
		{ fComps.insert(fComps.begin(), iBegin, iEnd);}
	void PrependTrail(const Trail& iTrail);
	void PrependComp(const Comp& iComp);
	void PrependBounce();
	void PrependBounces(size_t iCount);

	string8 AsString() const;
	string8 AsString(const string8& iSeparator, const string8& iBounce) const;

	Trail Branch() const;
	const Comp& Leaf() const;

	size_t Count() const;
	const Comp& At(size_t iIndex) const;
	Trail SubTrail(size_t iBegin, size_t iEnd) const;
	Trail SubTrail(size_t iBegin) const;

	Trail Normalized() const;
	Trail& Normalize();
	bool IsNormalized() const;

	const std::vector<Comp>& GetComps() const
		{ return fComps; }

	static void sParseStringAndAppend(
		const UTF8* iSeparator, const UTF8* iIgnore, const UTF8* iBounce,
		const UTF8* iPath, size_t iPathSize,
		std::vector<string8>& ioComps);

	static void sParseStringAndAppend(
		const UTF8* iSeparator, const UTF8* iIgnore, const UTF8* iBounce,
		const UTF8* iPath, size_t iPathSize,
		std::vector<Comp>& ioComps);

	static void sParseStringAndAppend(
		const UTF16* iSeparator, const UTF16* iIgnore, const UTF16* iBounce,
		const UTF16* iPath, size_t iPathSize,
		std::vector<string16>& ioComps);

private:
	std::vector<Comp> fComps;
	};

Trail operator+(const string8& iPOSIXTrail, const Trail& iTrail);

template <> struct RelopsTraits_HasEQ<Trail> : public RelopsTraits_Has {};
template <> struct RelopsTraits_HasLT<Trail> : public RelopsTraits_Has {};

template <class Comp>
void sNormalize_KeepLeadingBounces(
	const std::vector<Comp>& iComps, std::vector<Comp>& oComps)
	{
	for (typename std::vector<Comp>::const_iterator current = iComps.begin();
		current != iComps.end(); ++current)
		{
		// Note that an empty string8 is our notation for a bounce.
		if (sIsEmpty(*current) && !oComps.empty() && !sIsEmpty(oComps.back()))
			{
			// The current component is a bounce, the new list is not empty
			// and the component at the end of the the new list is not itself a bounce,
			// so just remove the component at the end of the new list.
			oComps.pop_back();
			}
		else
			{
			oComps.push_back(*current);
			}
		}
	}

template <class Comp>
size_t sNormalize_ReturnLeadingBounces(const std::vector<Comp>& iComps,
	std::vector<Comp>& oComps)
	{
	sNormalize_KeepLeadingBounces(iComps, oComps);
	size_t bounces = 0;
	while (oComps.size() && sIsEmpty(oComps.front()))
		{
		oComps.erase(oComps.begin());
		++bounces;
		}
	return bounces;	
	}

/**
Return a trail that would navigate from iSource to oDest. The vectors iSource and oDest
are considered to be lists of names of nodes starting at the root or other common node.
This code is used by ZFileLoc implementations, but is also available to application code.
*/
template <class Comp>
Trail sTrailFromTo(const std::vector<Comp>& iSource, const std::vector<Comp>& oDest)
	{
	size_t matchUntil = 0;
	while (matchUntil < iSource.size() && matchUntil < oDest.size()
		&& iSource[matchUntil] == oDest[matchUntil])
		{ ++matchUntil; }

	Trail theTrail;
	if (matchUntil < iSource.size())
		theTrail.AppendBounces(iSource.size() - matchUntil);

	if (matchUntil < oDest.size())
		theTrail.AppendTrail(oDest.begin() + matchUntil, oDest.end());

	return theTrail;
	}

} // namespace ZooLib

#endif // __ZooLib_Trail_h__
