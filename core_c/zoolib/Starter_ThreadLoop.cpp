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

#include "zoolib/Starter_ThreadLoop.h"

#include "zoolib/ZThread.h"

#include <vector>

namespace ZooLib {

using std::vector;

// =================================================================================================
// MARK: - Starter_ThreadLoop

class Starter_ThreadLoop
:	public Starter
	{
public:
	Starter_ThreadLoop()
	:	fKeepRunning(false)
		{}

	virtual ~Starter_ThreadLoop()
		{}

// From ZCounted via Starter
	virtual void Initialize()
		{
		ZCounted::Initialize();
		ZAcqMtxR acq(fMtxR);

		ZAssert(not fKeepRunning);

		fKeepRunning = true;
		ZThread::sCreate_T<Starter_ThreadLoop*>(&Starter_ThreadLoop::spRun, this);
		}

	virtual void Finalize()
		{
		ZGuardMtxR guard(fMtxR);
		if (not this->FinishFinalize())
			return;

		ZAssert(fKeepRunning);

		fKeepRunning = false;
		fCnd.Broadcast();
		}

// From Starter
	virtual bool QStart(const ZRef<Callable_Void>& iCallable)
		{
		ZGuardMtxR guard(fMtxR);
		fCallables.push_back(iCallable);
		fCnd.Broadcast();
		return true;
		}

private:
	void pRun()
		{
		ZThread::sSetName("Starter_ThreadLoop");

		ZGuardMtxR guard(fMtxR);

		while (fKeepRunning)
			{
			if (fCallables.empty())
				{ fCnd.Wait(fMtxR); }
			else
				{
				ZRef<ZCounted> self_ref = this;

				vector<ZRef<Callable_Void> > calling;

				fCallables.swap(calling);

				guard.Release();

				for (vector<ZRef<Callable_Void> >::iterator iter = calling.begin();
					iter != calling.end(); ++iter)
					{
					try { (*iter)->Call(); }
					catch (...) {}
					}

				guard.Acquire();
				}
			}
		guard.Release();
		delete this;
		}

	static void spRun(Starter_ThreadLoop* iStarter)
		{ iStarter->pRun(); }

	ZMtxR fMtxR;
	ZCnd fCnd;
	bool fKeepRunning;
	std::vector<ZRef<Callable_Void> > fCallables;
	};

// =================================================================================================
// MARK: - sStarter_ThreadLoop

ZRef<Starter> sStarter_ThreadLoop()
	{ return new Starter_ThreadLoop; }

} // namespace ZooLib