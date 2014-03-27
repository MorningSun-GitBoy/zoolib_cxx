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

#include "zoolib/ZCallable_PMF.h"
#include "zoolib/ZMACRO_foreach.h"
#include "zoolib/ZUtil_STL_map.h"
#include "zoolib/ZUtil_STL_vector.h"

#include "zoolib/dataspace/ZDataspace_SourceMUX.h"

namespace ZooLib {
namespace ZDataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

using namespace ZUtil_STL;

typedef map<int64,pair<ZRef<QueryEngine::Result>,ZRef<Event> > > Map_Refcon_Result;

// =================================================================================================
// MARK: - SourceMUX::ClientSource

class SourceMUX::ClientSource
:	public Source
	{
public:
	ClientSource(ZRef<SourceMUX> iMUX)
	:	fMUX(iMUX)
		{}

// From ZCounted via Source
	virtual void Finalize()
		{ fMUX->pFinalizeClientSource(this); }

// From Source
	virtual bool Intersects(const RelHead& iRelHead)
		{ return fMUX->pIntersects(this, iRelHead); }

	virtual void ModifyRegistrations(
		const AddedQuery* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount)
		{ fMUX->pModifyRegistrations(this, iAdded, iAddedCount, iRemoved, iRemovedCount); }

	virtual void CollectResults(vector<QueryResult>& oChanged)
		{
		this->pCollectResultsCalled();
		fMUX->pCollectResults(this, oChanged);
		}

// Our protocol
	void ResultsAvailable()
		{ Source::pTriggerResultsAvailable(); }

	ZRef<SourceMUX> fMUX;

	map<int64,int64> fMap_ClientToPRefcon;
	Map_Refcon_Result fResults;
	};

// =================================================================================================
// MARK: - SourceMUX

SourceMUX::SourceMUX(ZRef<Source> iSource)
:	fSource(iSource)
,	fResultsAvailable(false)
,	fNextPRefcon(1)
	{}

SourceMUX::~SourceMUX()
	{}

void SourceMUX::Initialize()
	{
	SourceFactory::Initialize();
	fSource->SetCallable_ResultsAvailable(
		sCallable(sWeakRef(this), &SourceMUX::pResultsAvailable));
	}

void SourceMUX::Finalize()
	{
	fSource->SetCallable_ResultsAvailable(null);
	SourceFactory::Finalize();
	}

ZRef<Source> SourceMUX::Make()
	{
	ZAcqMtxR acq(fMtxR);

	ZRef<ClientSource> theCS = new ClientSource(this);
	fClientSources.insert(theCS.Get());
	return theCS;
	}

bool SourceMUX::pIntersects(ZRef<ClientSource> iCS, const RelHead& iRelHead)
	{ return fSource->Intersects(iRelHead); }

void SourceMUX::pModifyRegistrations(ZRef<ClientSource> iCS,
	const AddedQuery* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardMtxR guard(fMtxR);

	vector<AddedQuery> theAddedQueries;
	theAddedQueries.reserve(iAddedCount);
	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		const int64 theClientRefcon = iAdded->GetRefcon();
		const int64 thePRefcon = fNextPRefcon++;

		sInsertMust(kDebug,
			iCS->fMap_ClientToPRefcon, theClientRefcon, thePRefcon);

		sInsertMust(kDebug,
			fPRefconToClient, thePRefcon, make_pair(iCS.Get(), theClientRefcon));

		theAddedQueries.push_back(AddedQuery(thePRefcon, iAdded->GetRel()));
		}

	vector<int64> removedQueries;
	removedQueries.reserve(iRemovedCount);
	while (iRemovedCount--)
		removedQueries.push_back(sGetEraseMust(kDebug, iCS->fMap_ClientToPRefcon, *iRemoved++));

	guard.Release();

	fSource->ModifyRegistrations(
		sFirstOrNil(theAddedQueries), theAddedQueries.size(),
		sFirstOrNil(removedQueries), removedQueries.size());
	}

void SourceMUX::pCollectResults(ZRef<ClientSource> iCS,
	vector<QueryResult>& oChanged)
	{
	ZGuardMtxR guard(fMtxR);

	vector<QueryResult> changes;
	if (sGetSet(fResultsAvailable, false))
		{
		guard.Release();
		fSource->CollectResults(changes);
		guard.Acquire();
		}

	foreachi (iterChanges, changes)
		{
		const pair<ClientSource*,int64>& thePair =
			sGetMust(kDebug, fPRefconToClient, iterChanges->GetRefcon());

		thePair.first->fResults[thePair.second] =
			make_pair(iterChanges->GetResult(), iterChanges->GetEvent());
		}

	oChanged.clear();
	oChanged.reserve(iCS->fResults.size());
	foreachi (iter, iCS->fResults)
		oChanged.push_back(QueryResult(iter->first, iter->second.first, iter->second.second));

	iCS->fResults.clear();
	}

void SourceMUX::pResultsAvailable(ZRef<Source> iSource)
	{
	ZGuardMtxR guard(fMtxR);
	if (not sGetSet(fResultsAvailable, true))
		{
		foreachi (iter, fClientSources)
			(*iter)->ResultsAvailable();
		}
	}

void SourceMUX::pFinalizeClientSource(ClientSource* iCS)
	{
	ZGuardMtxR guard(fMtxR);

	if (not iCS->FinishFinalize())
		return;

	vector<int64> removedQueries;
	removedQueries.reserve(iCS->fMap_ClientToPRefcon.size());
	foreachi (iter, iCS->fMap_ClientToPRefcon)
		removedQueries.push_back(iter->second);

	sEraseMust(fClientSources, iCS);
	delete iCS;

	guard.Release();

	fSource->ModifyRegistrations(
		nullptr, 0,
		sFirstOrNil(removedQueries), removedQueries.size());
	}

} // namespace ZDataspace
} // namespace ZooLib
