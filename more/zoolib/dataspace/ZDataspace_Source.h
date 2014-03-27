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

#ifndef __ZDataspace_Source_h__
#define __ZDataspace_Source_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZIntervalTreeClock.h"

#include "zoolib/QueryEngine/Result.h"
#include "zoolib/zra/ZRA_Expr_Rel.h"
#include "zoolib/zra/ZRA_RelHead.h"

#include <set>
#include <vector>

namespace ZooLib {
namespace ZDataspace {

using ZIntervalTreeClock::Clock;
using ZIntervalTreeClock::Event;

using ZRA::RelHead;

// =================================================================================================
// MARK: - AddedQuery

class AddedQuery
	{
public:
	AddedQuery();
	AddedQuery(const AddedQuery& iOther);
	~AddedQuery();
	AddedQuery& operator=(const AddedQuery& iOther);

	AddedQuery(int64 iRefcon, const ZRef<ZRA::Expr_Rel>& iRel);

	int64 GetRefcon() const;
	ZRef<ZRA::Expr_Rel> GetRel() const;

private:
	int64 fRefcon;
	ZRef<ZRA::Expr_Rel> fRel;
	};

// =================================================================================================
// MARK: - QueryResult

class QueryResult
	{
public:
	QueryResult();
	QueryResult(const QueryResult& iOther);
	~QueryResult();
	QueryResult& operator=(const QueryResult& iOther);

	QueryResult(int64 iRefcon, const ZRef<QueryEngine::Result>& iResult, const ZRef<Event>& iEvent);

	int64 GetRefcon() const;
	ZRef<QueryEngine::Result> GetResult() const;
	ZRef<Event> GetEvent() const;

private:
	int64 fRefcon;
	ZRef<QueryEngine::Result> fResult;
	ZRef<Event> fEvent;
	};

// =================================================================================================
// MARK: - Source

class Source : public ZCounted
	{
protected:
	Source();

public:
	virtual ~Source();

	virtual bool Intersects(const RelHead& iRelHead) = 0;

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount) = 0;

	virtual void CollectResults(std::vector<QueryResult>& oChanged) = 0;

	typedef ZCallable<void(ZRef<Source>)> Callable_ResultsAvailable;
	void SetCallable_ResultsAvailable(ZRef<Callable_ResultsAvailable> iCallable);

protected:
	void pCollectResultsCalled();
	void pTriggerResultsAvailable();

private:
	ZMtx fMtx;
	bool fCalled_ResultsAvailable;
	ZRef<Callable_ResultsAvailable> fCallable_ResultsAvailable;
	};

// =================================================================================================
// MARK: - SourceFactory

class SourceFactory : public ZCounted
	{
protected:
	SourceFactory();

public:
	virtual ~SourceFactory();

	virtual ZRef<Source> Make() = 0;
	};

} // namespace ZDataspace
} // namespace ZooLib

#endif // __ZDataspace_Source_h__
