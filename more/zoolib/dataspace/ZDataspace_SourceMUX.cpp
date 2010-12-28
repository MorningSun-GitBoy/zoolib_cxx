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
#include "zoolib/dataspace/ZDataspace_SourceMUX.h"

namespace ZooLib {
namespace ZDataspace {

using std::make_pair;
using std::map;
using std::pair;
using std::set;
using std::vector;

// =================================================================================================
#pragma mark -
#pragma mark * SourceMUX::ClientSource

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
	virtual RelHead GetRelHead()
		{ return fMUX->pGetRelHead(this); }

	virtual void ModifyRegistrations(
		const AddedSearch* iAdded, size_t iAddedCount,
		const int64* iRemoved, size_t iRemovedCount)
		{ fMUX->pModifyRegistrations(this, iAdded, iAddedCount, iRemoved, iRemovedCount); }

	virtual void CollectResults(vector<SearchResult>& oChanged)
		{ fMUX->pCollectResults(this, oChanged); }

// Our protocol
	void ResultsAvailable()
		{ this->pInvokeCallable_ResultsAvailable(); }

	ZRef<SourceMUX> fMUX;
	
	map<int64,int64> fMap_ClientToPRefcon;
	map<int64,ZRef<ZQE::Result> > fPendingResults;
	};

// =================================================================================================
#pragma mark -
#pragma mark * SourceMUX

SourceMUX::SourceMUX(ZRef<Source> iSource)
:	fSource(iSource)
,	fCallable_ResultsAvailable(MakeCallable(MakeWeakRef(this), &SourceMUX::pResultsAvailable))
,	fNextPRefcon(1)
	{
	fSource->SetCallable_ResultsAvailable(fCallable_ResultsAvailable);
	}

SourceMUX::~SourceMUX()
	{
	fSource->SetCallable_ResultsAvailable(null);
	}

ZRef<Source> SourceMUX::Make()
	{
	ZAcqMtxR acq(fMtxR);

	ZRef<ClientSource> theCS = new ClientSource(this);
	fClientSources.insert(theCS.Get());
	return theCS;
	}

RelHead SourceMUX::pGetRelHead(ZRef<ClientSource> iCS)
	{ return fSource->GetRelHead(); }

void SourceMUX::pModifyRegistrations(ZRef<ClientSource> iCS,
	const AddedSearch* iAdded, size_t iAddedCount,
	const int64* iRemoved, size_t iRemovedCount)
	{
	ZGuardRMtxR guard(fMtxR);

	vector<AddedSearch> theAddedSearches;
	theAddedSearches.reserve(iAddedCount);
	for (/*no init*/; iAddedCount--; ++iAdded)
		{
		const int64 theClientRefcon = iAdded->GetRefcon();
		const int64 thePRefcon = fNextPRefcon++;
		ZUtil_STL::sInsertMustNotContain(kDebug,
			iCS->fMap_ClientToPRefcon, theClientRefcon, thePRefcon);
		ZUtil_STL::sInsertMustNotContain(kDebug,
			fPRefconToClient, thePRefcon, make_pair(iCS.Get(), theClientRefcon));

		theAddedSearches.push_back(AddedSearch(thePRefcon, iAdded->GetRel()));
		}
	
	vector<int64> removedSearches;
	removedSearches.reserve(iRemovedCount);
	while (iRemovedCount--)
		{
		removedSearches.push_back(ZUtil_STL::sEraseAndReturn(kDebug,
			iCS->fMap_ClientToPRefcon, *iRemoved++));
		}

	fSource->ModifyRegistrations(
		ZUtil_STL::sFirstOrNil(theAddedSearches), theAddedSearches.size(),
		ZUtil_STL::sFirstOrNil(removedSearches), removedSearches.size());
	}

void SourceMUX::pCollectResults(ZRef<ClientSource> iCS,
	vector<SearchResult>& oChanged)
	{
	vector<SearchResult> changes;
	fSource->CollectResults(changes);

	ZGuardRMtxR guard(fMtxR);

	for (vector<SearchResult>::iterator
		iterChanges = changes.begin(), endChanges = changes.end();
		iterChanges != endChanges; ++iterChanges)
		{
		const pair<ClientSource*,int64>& thePair =
			ZUtil_STL::sGetMustContain(kDebug, fPRefconToClient, iterChanges->GetRefcon());
		thePair.first->fPendingResults[thePair.second] = iterChanges->GetResult();
		}

	oChanged.clear();
	oChanged.reserve(iCS->fPendingResults.size());
	for (map<int64,ZRef<ZQE::Result> >::iterator
		iter = iCS->fPendingResults.begin(), end = iCS->fPendingResults.end();
		iter != end; ++iter)
		{ oChanged.push_back(SearchResult(iter->first, iter->second, null)); }

	iCS->fPendingResults.clear();
	}

void SourceMUX::pResultsAvailable(ZRef<Source> iSource)
	{
	ZGuardRMtxR guard(fMtxR);
	for (set<ClientSource*>::iterator
		iter = fClientSources.begin(), end = fClientSources.end();
		iter != end; ++iter)
		{ (*iter)->ResultsAvailable(); }
	}

void SourceMUX::pFinalizeClientSource(ClientSource* iCS)
	{
	ZGuardRMtxR guard(fMtxR);

	if (!iCS->FinishFinalize())
		return;

	vector<int64> removedSearches;
	removedSearches.reserve(iCS->fMap_ClientToPRefcon.size());
	for (map<int64,int64>::iterator
		iter = iCS->fMap_ClientToPRefcon.begin(), end = iCS->fMap_ClientToPRefcon.begin();
		iter != end; ++iter)
		{ removedSearches.push_back(iter->second); }

	ZUtil_STL::sEraseMustContain(1, fClientSources, iCS);
	delete iCS;

	guard.Release();

	fSource->ModifyRegistrations(
		nullptr, 0,
		ZUtil_STL::sFirstOrNil(removedSearches), removedSearches.size());
	}

} // namespace ZDataspace
} // namespace ZooLib
