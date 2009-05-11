/* -------------------------------------------------------------------------------------------------
Copyright (c) 2000 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZThreadSimple__
#define __ZThreadSimple__ 1
#include "zconfig.h"

#include "zoolib/ZThread.h"

NAMESPACE_ZOOLIB_BEGIN

class ZThreadSimpleVoid_t;

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadSimple

template <class T = ZThreadSimpleVoid_t>
class ZThreadSimple : public ZThread
	{
public:
	typedef void (*ThreadProc)(T inArgument);
	ZThreadSimple(ThreadProc iProc, T iArgument, const char* iName = nil)
	:	ZThread(iName),
		fProc(iProc),
		fArgument(iArgument)
		{}

protected:
	virtual void Run()
		{ fProc(fArgument); }

private:
	ThreadProc fProc;
	T fArgument;
	};

// =================================================================================================
#pragma mark -
#pragma mark * ZThreadSimple, dummy

template <>
class ZThreadSimple<ZThreadSimpleVoid_t> : public ZThread
	{
public:
	typedef void (*ThreadProcNoArg)();
	ZThreadSimple(ThreadProcNoArg iProc, const char* iName = nil)
	:	ZThread(iName),
		fProcNoArg(iProc)
		{}

protected:
	virtual void Run()
		{ fProcNoArg(); }

private:
	ThreadProcNoArg fProcNoArg;
	};

NAMESPACE_ZOOLIB_END

#endif // __ZThreadSimple__
