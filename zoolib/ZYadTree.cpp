/* -------------------------------------------------------------------------------------------------
Copyright (c) 2011 Andrew Green
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

#include "zoolib/ZCountedVal.h"
#include "zoolib/ZTrail.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZVisitor_Do_T.h"
#include "zoolib/ZYadTree.h"

using std::map;
using std::string;
using std::vector;

namespace ZooLib {
namespace YadTree {
namespace { // anonymous

using namespace ZUtil_STL;

typedef ZCountedVal<string> CountedString;

class Link;
class YadMapAt;

// =================================================================================================
// MARK: - Helpers

ZRef<ZYadR> spWrap(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad);
ZRef<ZYadR> spWrapNoMat(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad);

// =================================================================================================
// MARK: - Link declaration

class Link
:	public ZCounted
	{
public:
// ctor that establishes a tree
	Link(const ZRef<CountedString>& iProtoName, const ZRef<ZYadMapAt>& iYadMapAt);

// ctor used as we walk down a tree.
	Link(const ZRef<Link>& iParent, const ZRef<ZYadMapAt>& iYadMapAt);

	ZRef<ZYadMapAt> GetYadMapAt();

	ZRef<ZYadR> ReadAt(const string& iName);

	ZRef<Link> WithRootAugment(const string& iRootAugmentName, const ZRef<Link>& iRootAugment);

private:
	const ZRef<CountedString> fProtoName;
	const ZRef<Link> fParent;
	const ZRef<ZYadMapAt> fYadMapAt;
	map<string,ZRef<Link> > fChildren;
	};

// =================================================================================================
// MARK: - YadSeqR

class YadSeqR
:	public ZYadSeqR
	{
public:
	YadSeqR(const ZRef<Link>& iLink, const ZRef<ZYadSeqR>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqR> fYad;
	};

// =================================================================================================
// MARK: - YadSeqRClone

class YadSeqRClone
:	public ZYadSeqRClone
	{
public:
	YadSeqRClone(const ZRef<Link>& iLink, const ZRef<ZYadSeqRClone>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRClone(fLink, fYad->Clone()); }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqRClone> fYad;
	};

// =================================================================================================
// MARK: - YadSeqRPos

class YadSeqRPos
:	public ZYadSeqRPos
	{
public:
	YadSeqRPos(const ZRef<Link>& iLink, const ZRef<ZYadSeqRPos>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRPos(fLink, fYad->Clone().DynamicCast<ZYadSeqRPos>()); }

// From ZYadSeqRPos
	virtual uint64 GetPosition()
		{ return fYad->GetPosition(); }

	virtual void SetPosition(uint64 iPosition)
		{ fYad->SetPosition(iPosition); }

	virtual uint64 GetSize()
		{ return fYad->GetSize(); }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqRPos> fYad;
	};

// =================================================================================================
// MARK: - YadSeqAt

class YadSeqAt
:	public ZYadSeqAt
	{
public:
	YadSeqAt(const ZRef<Link>& iLink, const ZRef<ZYadSeqAt>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
	,	fChildren(fYad->Count(), null)
		{}

	virtual bool IsSimple(const ZYadOptions& iOptions)
		{ return false; }

// From ZYadSeqAt
	virtual uint64 Count()
		{ return fYad->Count(); }

	virtual ZRef<ZYadR> ReadAt(uint64 iPosition)
		{
		ZRef<ZYadR> theYad;
		if (iPosition < fChildren.size())
			{
			theYad = fChildren[iPosition];
			if (not theYad)
				{
				theYad = spWrap(fLink, fYad->ReadAt(iPosition));
				fChildren[iPosition] = theYad;
				}
			}
		return theYad;
		}

// Our protocol
	ZRef<ZYadSeqAt> GetYad()
		{ return fYad; }

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqAt> fYad;
	vector<ZRef<ZYadR> > fChildren;
	};

// =================================================================================================
// MARK: - YadSeqAtRPos

class YadSeqAtRPos
:	public ZYadSeqAtRPos
	{
public:
	YadSeqAtRPos(const ZRef<Link>& iLink, const ZRef<ZYadSeqAtRPos>& iYad)
	:	fLink(iLink)
	,	fYad(iYad)
	,	fChildren(fYad->Count(), null)
		{}

// From ZYadSeqR
	virtual ZRef<ZYadR> ReadInc()
		{ return spWrap(fLink, fYad->ReadInc()); }

	virtual bool Skip()
		{ return fYad->Skip(); }

	virtual void SkipAll()
		{ fYad->SkipAll(); }

// From ZYadSeqRClone
	virtual ZRef<ZYadSeqRClone> Clone()
		{ return new YadSeqRPos(fLink, fYad->Clone().DynamicCast<ZYadSeqAtRPos>()); }

// From ZYadSeqRPos
	virtual uint64 GetPosition()
		{ return fYad->GetPosition(); }

	virtual void SetPosition(uint64 iPosition)
		{ fYad->SetPosition(iPosition); }

	virtual uint64 GetSize()
		{ return fYad->GetSize(); }

// From ZYadSeqAt
	virtual uint64 Count()
		{ return fYad->Count(); }

	virtual ZRef<ZYadR> ReadAt(uint64 iPosition)
		{
		ZRef<ZYadR> theYad;
		if (iPosition < fChildren.size())
			{
			theYad = fChildren[iPosition];
			if (not theYad)
				{
				theYad = spWrap(fLink, fYad->ReadAt(iPosition));
				fChildren[iPosition] = theYad;
				}
			}
		return theYad;
		}

private:
	const ZRef<Link> fLink;
	const ZRef<ZYadSeqAtRPos> fYad;
	vector<ZRef<ZYadR> > fChildren;
	};

// =================================================================================================
// MARK: - YadMapAt

class YadMapAt
:	public ZYadMapAt
	{
public:
	YadMapAt(const ZRef<Link>& iLink)
	:	fLink(iLink)
		{}

// From ZYadMapAt
	virtual bool IsSimple(const ZYadOptions& iOptions)
		{ return false; }

	virtual ZRef<ZYadR> ReadAt(const string& iName)
		{ return fLink->ReadAt(iName); }

	virtual ZQ<ZAny> QAsAny()
		{
		if (ZRef<ZYadMapAt> theYadMapAt = fLink->GetYadMapAt())
			return theYadMapAt->QAsAny();
		return null;
		}

// Our protocol
	ZRef<Link> GetLink()
		{ return fLink; }

private:
	const ZRef<Link> fLink;
	};

// =================================================================================================
// MARK: - Link definition

Link::Link(const ZRef<CountedString>& iProtoName, const ZRef<ZYadMapAt>& iYadMapAt)
:	fProtoName(iProtoName)
,	fYadMapAt(iYadMapAt)
	{}

Link::Link(const ZRef<Link>& iParent, const ZRef<ZYadMapAt>& iYadMapAt)
:	fProtoName(iParent->fProtoName)
,	fParent(iParent)
,	fYadMapAt(iYadMapAt)
	{}

ZRef<ZYadMapAt> Link::GetYadMapAt()
	{ return fYadMapAt; }

ZRef<ZYadR> Link::ReadAt(const string& iName)
	{
	if (ZQ<ZRef<Link> > theQ = sQGet(fChildren, iName))
		return new YadMapAt(*theQ);

	if (ZRef<ZYadR> theYad = fYadMapAt->ReadAt(iName))
		{
		if (ZRef<ZYadMapAt> asYadMapAt = theYad.DynamicCast<ZYadMapAt>())
			{
			ZRef<Link> theLink = new Link(this, asYadMapAt);
			sInsertMustNotContain(fChildren, iName, theLink);
			return new YadMapAt(theLink);
			}
		else
			{
			return spWrapNoMat(this, theYad);
			}
		}

	if (ZRef<ZYadStrimR> theProtoYad = fYadMapAt->ReadAt(fProtoName->Get()).DynamicCast<ZYadStrimR>())
		{
		const string theTrailString = theProtoYad->GetStrimR().ReadAll8();

		if (theTrailString.size())
			{
			// Our Yad has a non-empty proto trail.
			size_t index = 0;
			const ZTrail theTrail = ZTrail(theTrailString).Normalized() + iName;
			
			ZRef<Link> cur = this;

			if (theTrailString[0] == '/')
				{
				// Walk up to the root.
				for (ZRef<Link> next = null; next = cur->fParent; cur = next)
					{}
				}
			else
				{
				// Follow any leading bounces.
				for (/*no init*/;
					cur && index < theTrail.Count() && theTrail.At(index).empty();
					++index, cur = cur->fParent)
					{}
				}
						
			// Walk down the tree.
			for (;;)
				{
				if (ZRef<ZYadR> theYadR = cur->ReadAt(theTrail.At(index)))
					{
					if (++index == theTrail.Count())
						return theYadR;

					if (ZRef<YadMapAt> theYadMapAt = theYadR.DynamicCast<YadMapAt>())
						{
						cur = theYadMapAt->GetLink();
						continue;
						}
					}
				break;
				}
			}
		}
	return null;
	}

ZRef<Link> Link::WithRootAugment(const string& iRootAugmentName, const ZRef<Link>& iRootAugment)
	{
	if (fParent)
		{
		ZRef<Link> newParent = fParent->WithRootAugment(iRootAugmentName, iRootAugment);
		ZRef<Link> newSelf = new Link(newParent, fYadMapAt);
		return newSelf;
		}

	ZRef<Link> newSelf = new Link(fProtoName, fYadMapAt);
	sInsertMustNotContain(newSelf->fChildren, iRootAugmentName, iRootAugment);
	return newSelf;
	}

// =================================================================================================
// MARK: - Helpers

class Visitor_Wrap
:	public ZVisitor_Do_T<ZRef<ZYadR> >
,	ZVisitor_Yad_PreferAt
	{
public:
	Visitor_Wrap(const ZRef<Link>& iLink)
	:	fLink(iLink)
		{}

	virtual void Visit_YadR(const ZRef<ZYadR>& iYadR)
		{ pSetResult(iYadR); }

	virtual void Visit_YadSeqR(const ZRef<ZYadSeqR>& iYadSeqR)
		{ pSetResult(new YadSeqR(fLink, iYadSeqR)); }

	virtual void Visit_YadSeqRClone(const ZRef<ZYadSeqRClone>& iYadSeqRClone)
		{ pSetResult(new YadSeqRClone(fLink, iYadSeqRClone)); }

	virtual void Visit_YadSeqRPos(const ZRef<ZYadSeqRPos>& iYadSeqRPos)
		{ pSetResult(new YadSeqRPos(fLink, iYadSeqRPos)); }

	virtual void Visit_YadSeqAt(const ZRef<ZYadSeqAt>& iYadSeqAt)
		{ pSetResult(new YadSeqAt(fLink, iYadSeqAt)); }

	virtual void Visit_YadSeqAtRPos(const ZRef<ZYadSeqAtRPos>& iYadSeqAtRPos)
		{ pSetResult(new YadSeqAtRPos(fLink, iYadSeqAtRPos)); }

	virtual void Visit_YadMapAt(const ZRef<ZYadMapAt>& iYadMapAt)
		{ pSetResult(new YadMapAt(new Link(fLink, iYadMapAt))); }

	const ZRef<Link>& fLink;
	};

ZRef<ZYadR> spWrap(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad)
	{ return Visitor_Wrap(iLink).Do(iYad); }

class Visitor_Wrap_NoMat
:	public Visitor_Wrap
	{
public:
	Visitor_Wrap_NoMat(const ZRef<Link>& iLink)
	:	Visitor_Wrap(iLink)
		{}

	virtual void Visit_YadMapAt(const ZRef<ZYadMapAt>& iYadMapAt)
		{ ZUnimplemented(); }
	};

ZRef<ZYadR> spWrapNoMat(const ZRef<Link>& iLink, const ZRef<ZYadR>& iYad)
	{ return Visitor_Wrap_NoMat(iLink).Do(iYad); }

} // anonymous namespace
} // namespace YadTree

// =================================================================================================
// MARK: -

using namespace YadTree;

ZRef<ZYadMapAt> sYadTree(const ZRef<ZYadMapAt>& iYadMapAt, const string& iProtoName)
	{ return new YadMapAt(new Link(new CountedString(iProtoName), iYadMapAt)); }

ZRef<ZYadMapAt> sYadTree(const ZRef<ZYadMapAt>& iYadMapAt)
	{ return sYadTree(iYadMapAt, "_"); }

ZRef<ZYadMapAt> sParameterizedYadTree(const ZRef<ZYadMapAt>& iBase,
	const string& iRootAugmentName, const ZRef<ZYadMapAt>& iRootAugment)
	{
	if (ZRef<YadMapAt> theBase = iBase.DynamicCast<YadMapAt>())
		{
		if (ZRef<YadMapAt> theRootAugment = iRootAugment.DynamicCast<YadMapAt>())
			{
			ZRef<Link> newLink =
				theBase->GetLink()->WithRootAugment(iRootAugmentName, theRootAugment->GetLink());
			return new YadMapAt(newLink);
			}
		}
	return iBase;
	}

} // namespace ZooLib
