/* -------------------------------------------------------------------------------------------------
Copyright (c) 2012 Andrew Green
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

#ifndef __ZCallOnNewThread_h__
#define __ZCallOnNewThread_h__ 1
#include "zconfig.h"

#include "zoolib/ZCallable.h"
#include "zoolib/ZThread.h"

namespace ZooLib {

// =================================================================================================
// MARK: - sCallOnNewThread

inline
void sCallOnNewThread(const ZRef<ZCallable<void(void)> >& iCallable)
	{ ZThread::sCreate_T<ZRef<ZCallable<void(void)> > >(sCallVoid<void>, iCallable); }

template <class T>
void sCallOnNewThread(const ZRef<ZCallable<T(void)> >& iCallable)
	{ ZThread::sCreate_T<ZRef<ZCallable<T(void)> > >(sCallVoid<T>, iCallable); }

} // namespace ZooLib

#endif // __ZCallOnNewThread_h__