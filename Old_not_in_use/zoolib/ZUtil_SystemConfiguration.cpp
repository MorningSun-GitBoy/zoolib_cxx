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

#include "zoolib/ZUtil_SystemConfiguration.h"

#if ZCONFIG_SPI_Enabled(SystemConfiguration)

#include "zoolib/Compare_T.h"

#include "zoolib/ZUtil_CF.h"
#include "zoolib/ZUtil_CF_Context.h"
#include "zoolib/ZYad_CF.h"

namespace ZooLib {
namespace ZUtil_SystemConfiguration {

using namespace ZUtil_CF;

// =================================================================================================
#pragma mark - ZUtil_SystemConfiguration::Store

Store::Store(ZRef<Callable> iCallable)
:	fCallable(iCallable)
	{}

Store::~Store()
	{}

void Store::Initialize()
	{
	ZCounted::Initialize();

	fStoreRef = sAdopt& ::SCDynamicStoreCreate(nullptr,
		CFSTR("ZUtil_SystemConfiguration"),
		&Store::spCallback,
		ZUtil_CF::Context<SCDynamicStoreContext>(this->GetWeakRefProxy()));
	}

SCDynamicStoreRef Store::GetStoreRef()
	{ return fStoreRef; }

void Store::spCallback(SCDynamicStoreRef store, CFArrayRef changedKeys, void *info)
	{
	if (ZRef<Store> theStore = ZWeakRef<Store>(static_cast<WeakRefProxy*>(info)))
		sCall(theStore->fCallable, theStore, changedKeys);
	}

// =================================================================================================
#pragma mark - YadMapRPos (anonymous)

namespace { // anonymous 

class YadMapRPos
:	public ZYadMapRPos
	{
	YadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys, size_t iPosition)
	:	fStoreRef(iStoreRef)
	,	fKeys(iKeys)
	,	fPosition(iPosition)
		{}

public:
	YadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys)
	:	fStoreRef(iStoreRef)
	,	fKeys(iKeys)
	,	fPosition(0)
		{}

// From ZYadMapR via ZYadMapRPos
	virtual ZRef<ZYadR> ReadInc(Name& oName)
		{
		while (fPosition < fKeys.Count())
			{
			if (ZQ<ZRef<CFStringRef> > qName = fKeys.QGet<ZRef<CFStringRef> >(fPosition++))
				{
				if (ZVal_CF theVal = ::SCDynamicStoreCopyValue(fStoreRef, *qName))
					{
					oName = ZUtil_CF::sAsUTF8(*qName);
					return sYadR(theVal);
					}
				}
			}
		return null;
		}

// From ZYadMapRClone via ZYadMapRPos
	virtual ZRef<ZYadMapRClone> Clone()
		{ return new YadMapRPos(fStoreRef, fKeys, fPosition); }

// From ZYadMapRPos
	virtual void SetPosition(const Name& iName)
		{
		for (/*no init*/; fPosition < fKeys.Count(); ++fPosition)
			{
			if (ZQ<ZRef<CFStringRef> > qName = fKeys.QGet<ZRef<CFStringRef> >(fPosition))
				{
				if (iName == ZUtil_CF::sAsUTF8(*qName))
					break;
				}
			}
		}

private:
	const ZRef<SCDynamicStoreRef> fStoreRef;
	const ZSeq_CF fKeys;
	size_t fPosition;
	};

} // anonymous namespace 

// =================================================================================================
#pragma mark - sYadMapRPos

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef, CFArrayRef iKeys)
	{ return new YadMapRPos(iStoreRef, iKeys); }

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef, const std::string& iPattern)
	{
	return new YadMapRPos(iStoreRef,
		sTempPtrRef& ::SCDynamicStoreCopyKeyList(iStoreRef, ZUtil_CF::sString(iPattern)));
	}

ZRef<ZYadMapRPos> sYadMapRPos(SCDynamicStoreRef iStoreRef)
	{ return sYadMapRPos(iStoreRef, ".+"); }

} // namespace ZUtil_SystemConfiguration
} // namespace ZooLib

#endif // ZCONFIG_SPI_Enabled(SystemConfiguration)
