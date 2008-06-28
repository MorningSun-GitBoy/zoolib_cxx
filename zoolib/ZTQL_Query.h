/* -------------------------------------------------------------------------------------------------
Copyright (c) 2007 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZTQL_Query__
#define __ZTQL_Query__ 1
#include "zconfig.h"

#include "zoolib/ZTQL_RelHead.h"
#include "zoolib/ZTQL_Node.h"
#include "zoolib/ZTQL_Spec.h"

#include <set>
#include <vector>

// =================================================================================================
#pragma mark -
#pragma mark * ZTQL

namespace ZTQL {

class Query
	{
public:
	Query();
	Query(const Query& iOther);
	Query(ZRef<Node> iNode);
	Query& operator=(const Query& iOther);
	~Query();

	ZRef<Node> GetNode() const;

	Query(const ZTuple* iTuples, size_t iCount);

	Query(const std::vector<ZTuple>& iTuples);

	static Query sAll(const RelHead& iRelHead);

	static Query sAllID(const ZTName& iIDPropName);

	static Query sAllID(const ZTName& iIDPropName, const RelHead& iRelHead);

//##	Query Difference(const Query& iOther) const;

	Query Intersect(const Query& iOther) const;

	Query Join(const Query& iOther) const;

	Query Project(const RelHead& iRelHead) const;

	Query Rename(const ZTName& iOld, const ZTName& iNew) const;

	Query Select(const Spec& iSpec) const;

	Query Union(const Query& iOther) const;

	RelHead GetRelHead() const;

private:
	ZRef<Node> fNode;
	};

Query sAll(const RelHead& iRelHead);

Query sAllID(const ZTName& iIDName);

Query sAllID(const ZTName& iIDName, const RelHead& iRelHead);

Query sDifference(const Query& iQuery1, const Query& iQuery2);

Query sExplicit(const ZTuple* iTuples, size_t iCount);

Query sExplicit(const std::vector<ZTuple>& iTuples);

Query sIntersect(const Query& iQuery1, const Query& iQuery2);

Query sJoin(const Query& iQuery1, const Query& iQuery2);

Query sProject(const Query& iQuery, const RelHead& iRelHead);

Query sRename(const Query& iQuery, const ZTName& iOldPropName, const ZTName& iNewPropName);

Query sSelect(const Query& iQuery, const Spec& iSpec);

Query sUnion(const Query& iQuery1, const Query& iQuery2);

} // namespace ZTQL

#endif // __ZTQL_Query__
