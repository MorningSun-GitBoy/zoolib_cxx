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

#ifndef __ZooLib_Dataspace_Relater_h__
#define __ZooLib_Dataspace_Relater_h__ 1
#include "zconfig.h"

#include "zoolib/Callable.h"
#include "zoolib/ValueOnce.h"

#include "zoolib/Dataspace/Types.h"

#include <set>
#include <vector>

namespace ZooLib {
namespace Dataspace {

// =================================================================================================
#pragma mark - AddedQuery

class AddedQuery
	{
public:
	AddedQuery();
	AddedQuery(const AddedQuery& iOther);
	~AddedQuery();
	AddedQuery& operator=(const AddedQuery& iOther);

	AddedQuery(int64 iRefcon, const ZP<RelationalAlgebra::Expr_Rel>& iRel);

	int64 GetRefcon() const;
	ZP<RelationalAlgebra::Expr_Rel> GetRel() const;

private:
	int64 fRefcon;
	ZP<RelationalAlgebra::Expr_Rel> fRel;
	};

// =================================================================================================
#pragma mark - QueryResult


class QueryResult
	{
public:
	QueryResult();
	QueryResult(const QueryResult& iOther);
	~QueryResult();
	QueryResult& operator=(const QueryResult& iOther);

	QueryResult(int64 iRefcon, const ZP<Result>& iResult);

	QueryResult(int64 iRefcon,
		const ZP<Result>& iResult,
		const ZP<ResultDeltas>& iResultDeltas);

	int64 GetRefcon() const;
	ZP<Result> GetResult() const;
	ZP<ResultDeltas> GetResultDeltas() const;

private:
	int64 fRefcon;
	ZP<Result> fResult;
	ZP<ResultDeltas> fResultDeltas;
	};

// =================================================================================================
#pragma mark - Relater

class Relater : public Counted
	{
protected:
	Relater();

public:
	virtual ~Relater();

	virtual bool Intersects(const RelHead& iRelHead) = 0;

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount) = 0;

	virtual void CollectResults(std::vector<QueryResult>& oChanged, int64& oChangeCount) = 0;

	typedef Callable<void(ZP<Relater>)> Callable_RelaterResultsAvailable;
	void SetCallable_RelaterResultsAvailable(ZP<Callable_RelaterResultsAvailable> iCallable);

protected:
	void pCalled_RelaterCollectResults();
	void pTrigger_RelaterResultsAvailable();

private:
	ZMtx fMtx;
	FalseOnce fCalled_RelaterResultsAvailable;
	ZP<Callable_RelaterResultsAvailable> fCallable_RelaterResultsAvailable;
	};

} // namespace Dataspace
} // namespace ZooLib

#endif // __ZooLib_Dataspace_Relater_h__
