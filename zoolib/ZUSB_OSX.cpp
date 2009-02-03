/* -------------------------------------------------------------------------------------------------
Copyright (c) 2008 Andrew Green
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

#include "zoolib/ZUSB_OSX.h"

#if ZCONFIG_API_Enabled(USB_OSX)

#include "zoolib/ZByteSwap.h"
#include "zoolib/ZLog.h"
#include "zoolib/ZMemory.h" // For ZBlockCopy
#include "zoolib/ZString.h"
#include "zoolib/ZThreadImp.h"
#include "zoolib/ZUtil_Strim_Data.h"

#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOMessage.h>
#include <IOKit/IOCFPlugIn.h>

NAMESPACE_ZOOLIB_BEGIN

using std::exception;
using std::min;
using std::runtime_error;
using std::string;

// =================================================================================================
#pragma mark -
#pragma mark * Other stuff

static string sErrorAsString(IOReturn iError)
	{
	switch (iError)
		{
		case kIOReturnNotResponding: return "kIOReturnNotResponding";
		case kIOReturnNoDevice: return "kIOReturnNotResponding";
		case kIOReturnBadArgument: return "kIOReturnNotResponding";
		case kIOReturnAborted: return "kIOReturnAborted";
		default: return ZString::sFormat("%d", iError);
		}
	}

static void sThrowIfErr(IOReturn iErr)
	{
	if (iErr)
		throw runtime_error("Got error");
	}

static IOCFPlugInInterface** sCreatePluginInterface(io_service_t iService)
	{
	IOCFPlugInInterface** plugInInterface = nil;
	SInt32 theScore;
	::IOCreatePlugInInterfaceForService(iService,
		kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugInInterface, &theScore);
	return plugInInterface;
	}

static IOUSBDeviceInterface182** sCreate_USBDeviceInterface(io_service_t iUSBDevice)
	{
	IOUSBDeviceInterface182** theIOUSBDeviceInterface = nil;
	if (IOCFPlugInInterface** plugInInterface = sCreatePluginInterface(iUSBDevice))
		{
		if (0 != plugInInterface[0]->QueryInterface(plugInInterface,
			CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID182), (LPVOID*)&theIOUSBDeviceInterface))
			{
			theIOUSBDeviceInterface = nil;
			}
		::IODestroyPlugInInterface(plugInInterface);
		}

	return theIOUSBDeviceInterface;
	}	

// =================================================================================================
#pragma mark -
#pragma mark * ZUSBWatcher

static void sSetSInt32(CFMutableDictionaryRef iDict, CFStringRef iPropName, SInt32 iValue)
	{
	ZRef<CFNumberRef> numberRef
		(false, ::CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &iValue));

	::CFDictionarySetValue(iDict, iPropName, numberRef);
	}

ZUSBWatcher::ZUSBWatcher(
	IONotificationPortRef iIONotificationPortRef, SInt32 iUSBVendor, SInt32 iUSBProduct)
:	fObserver(nil),
	fIONotificationPortRef(iIONotificationPortRef),
	fNotification(0)
	{
	CFMutableDictionaryRef matchingDict = ::IOServiceMatching(kIOUSBDeviceClassName);
	sSetSInt32(matchingDict, CFSTR(kUSBVendorID), iUSBVendor);
	sSetSInt32(matchingDict, CFSTR(kUSBProductID), iUSBProduct);

	sThrowIfErr(::IOServiceAddMatchingNotification(
		fIONotificationPortRef, kIOFirstMatchNotification, matchingDict,
		spDeviceAdded, this, &fNotification));
	}

ZUSBWatcher::~ZUSBWatcher()
	{
	if (fNotification)
		::IOObjectRelease(fNotification);
	}

void ZUSBWatcher::SetObserver(Observer* iObserver)
	{
	fObserver = iObserver;
	// Iterate once to get already-present devices and arm the notification
	this->pDeviceAdded(fNotification);
	}

void ZUSBWatcher::pDeviceAdded(io_iterator_t iIterator)
	{
	for (io_service_t iter; iter = ::IOIteratorNext(iIterator); /*no inc*/)
		{
		if (ZLOG(s, eInfo, "ZUSBWatcher"))
			s.Writef("Device added, iterator: 0x%08x.", iter);

		try
			{
			ZRef<ZUSBDevice> theUSBDevice = new ZUSBDevice(fIONotificationPortRef, iter);
			fObserver->Added(theUSBDevice);
			}
		catch (exception& ex)
			{
			if (ZLOG(s, eInfo, "ZUSBWatcher"))
				s << "Couldn't instantiate ZUSBDevice, caught exception: " << ex.what();

			fObserver->Added(ZRef<ZUSBDevice>());
			}

		::IOObjectRelease(iter);
		}
	}

void ZUSBWatcher::spDeviceAdded(void* iRefcon, io_iterator_t iIterator)
	{ static_cast<ZUSBWatcher*>(iRefcon)->pDeviceAdded(iIterator); }

// =================================================================================================
#pragma mark -
#pragma mark * ZUSBDevice

ZUSBDevice::ZUSBDevice(IONotificationPortRef iIONotificationPortRef, io_service_t iUSBDevice)
:	fIOUSBDeviceInterface(nil),
	fNotification(0),
	fLocationID(0),
	fObserver(nil),
	fDetached(false),
	fHasIOUSBDeviceDescriptor(false)
	{
	try
		{
		fIOUSBDeviceInterface = sCreate_USBDeviceInterface(iUSBDevice);
		if (!fIOUSBDeviceInterface)
			{
			if (ZLOG(s, eDebug, "ZUSBDevice"))
				s.Writef("sCreate_USBDeviceInterface returned null");
			throw runtime_error("Couldn't create fIOUSBDeviceInterface");
			}

		fIOUSBDeviceInterface[0]->GetLocationID(fIOUSBDeviceInterface, &fLocationID);

		kern_return_t result = ::IOServiceAddInterestNotification(
			iIONotificationPortRef, iUSBDevice, kIOGeneralInterest,
			spDeviceNotification, this, &fNotification);

		if (result != KERN_SUCCESS || fNotification == 0)
			throw runtime_error("Couldn't create notification");

		result = fIOUSBDeviceInterface[0]->USBDeviceOpen(fIOUSBDeviceInterface);

		if (result)
			{
			if (ZLOG(s, eInfo, "ZUSBDevice"))
				s.Writef("ZUSBDevice, USBDeviceOpen failed");
			throw runtime_error("ZUSBDevice, USBDeviceOpen failed");
			}

		UInt8 numConf;
		sThrowIfErr(fIOUSBDeviceInterface[0]->
			GetNumberOfConfigurations(fIOUSBDeviceInterface, &numConf));

		if (!numConf)
			{
			if (ZLOG(s, eDebug, "ZUSBDevice"))
				s.Writef("Got zero configurations");
			throw runtime_error("Got zero configurations");
			}

		IOUSBConfigurationDescriptorPtr confDesc;
		sThrowIfErr(fIOUSBDeviceInterface[0]->
			GetConfigurationDescriptorPtr(fIOUSBDeviceInterface, 0, &confDesc));

// I'm disabling this unconditional SetConfiguration as it causes what looks like
// a device re-enumeration, and does not appear to be essential for connecting to BlackBerries.
//##		sThrowIfErr(fIOUSBDeviceInterface[0]->
//##			SetConfiguration(fIOUSBDeviceInterface, confDesc->bConfigurationValue));
		}
	catch (...)
		{
		if (fNotification)
			{
			::IOObjectRelease(fNotification);
			fNotification = 0;
			}

		if (fIOUSBDeviceInterface)
			{
			fIOUSBDeviceInterface[0]->USBDeviceClose(fIOUSBDeviceInterface);
			fIOUSBDeviceInterface[0]->Release(fIOUSBDeviceInterface);
			fIOUSBDeviceInterface = nil;
			}
		throw;
		}
	}

ZUSBDevice::~ZUSBDevice()
	{
	if (fNotification)
		{
		::IOObjectRelease(fNotification);
		fNotification = 0;
		}

	if (fIOUSBDeviceInterface )
		{
		fIOUSBDeviceInterface[0]->USBDeviceClose(fIOUSBDeviceInterface);
		fIOUSBDeviceInterface[0]->Release(fIOUSBDeviceInterface);
		fIOUSBDeviceInterface = nil;
		}
	}

void ZUSBDevice::SetObserver(Observer* iObserver)
	{
	fObserver = iObserver;
	if (fDetached)
		{
		fDetached = false;
		fObserver->Detached(this);
		}
	}

IOUSBDeviceInterface182** ZUSBDevice::GetIOUSBDeviceInterface()
	{ return fIOUSBDeviceInterface; }

ZRef<ZUSBInterfaceInterface> ZUSBDevice::CreateInterfaceInterface(
	CFRunLoopRef iRunLoopRef, uint8 iProtocol)
	{
	IOUSBFindInterfaceRequest request; 
	request.bInterfaceClass = kIOUSBFindInterfaceDontCare; 
	request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare; 
	request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare; 
	request.bAlternateSetting = kIOUSBFindInterfaceDontCare; 

	io_iterator_t iterator;
	if (kIOReturnSuccess != fIOUSBDeviceInterface[0]->CreateInterfaceIterator(
		fIOUSBDeviceInterface, &request, &iterator))
		{
		iterator = 0;
		}

	while (iterator)
		{
		io_service_t usbInterface = ::IOIteratorNext(iterator);
		if (!usbInterface)
			{
			::IOObjectRelease(iterator);
			iterator = 0;
			continue;
			}

		SInt32 score; 
		IOCFPlugInInterface** plugInInterface;
		if (kIOReturnSuccess != ::IOCreatePlugInInterfaceForService(usbInterface, 
			kIOUSBInterfaceUserClientTypeID, 
			kIOCFPlugInInterfaceID, 
			&plugInInterface, &score))
			{
			plugInInterface = nil;
			}
		::IOObjectRelease(usbInterface);

		if (plugInInterface)
			{
			IOUSBInterfaceInterface190** theIOUSBInterfaceInterface;

			if (plugInInterface[0]->QueryInterface(plugInInterface, 
				CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID190),
				(LPVOID*)&theIOUSBInterfaceInterface))
				{
				theIOUSBInterfaceInterface = nil;
				}
			::IODestroyPlugInInterface(plugInInterface);

			if (theIOUSBInterfaceInterface)
				{
				UInt8 protocol = 0;
				if (0 == theIOUSBInterfaceInterface[0]->
					GetInterfaceProtocol(theIOUSBInterfaceInterface, &protocol))
					{
					if (protocol == iProtocol)
						{
						if (kIOReturnSuccess == theIOUSBInterfaceInterface[0]->
							USBInterfaceOpen(theIOUSBInterfaceInterface))
							{
							return new ZUSBInterfaceInterface(this,
								iRunLoopRef, theIOUSBInterfaceInterface);
							}
						}
					}
				theIOUSBInterfaceInterface[0]->Release(theIOUSBInterfaceInterface);
				}
			}
		}
	return ZRef<ZUSBInterfaceInterface>();
	}

void ZUSBDevice::pDeviceNotification(
	io_service_t iService, natural_t iMessageType, void* iMessageArgument)
	{
	if (iMessageType == kIOMessageServiceIsTerminated)
		{
		if (ZLOG(s, eInfo, "ZUSBDevice"))
			s.Writef("Device removed, service: 0x%08x.", iService);

		if (fObserver)
			fObserver->Detached(this);
		else
			fDetached = true;
		}
	}

void ZUSBDevice::spDeviceNotification(
	void* iRefcon, io_service_t iService, natural_t iMessageType, void* iMessageArgument)
	{
	static_cast<ZUSBDevice*>(iRefcon)->
		pDeviceNotification(iService, iMessageType, iMessageArgument);
	}

uint16 ZUSBDevice::GetIDVendor()
	{
	this->pFetchDeviceDescriptor();
	return ZByteSwap_LittleToHost16(fIOUSBDeviceDescriptor.idVendor);
	}

uint16 ZUSBDevice::GetIDProduct()
	{
	this->pFetchDeviceDescriptor();
	return ZByteSwap_LittleToHost16(fIOUSBDeviceDescriptor.idProduct);
	}

const IOUSBDeviceDescriptor& ZUSBDevice::GetDeviceDescriptor()
	{
	this->pFetchDeviceDescriptor();
	return fIOUSBDeviceDescriptor;
	}
	
// If we use IOUSBDeviceInterface197 then we can bypass this crap.
void ZUSBDevice::pFetchDeviceDescriptor()
	{
	if (fHasIOUSBDeviceDescriptor)
		return;

	IOUSBDevRequest req;
	req.bmRequestType = USBmakebmRequestType(kUSBIn, kUSBStandard, kUSBDevice);
	req.bRequest = kUSBRqGetDescriptor;
	req.wValue = kUSBDeviceDesc << 8;
	req.wIndex = 0;
	req.wLength = sizeof(IOUSBDeviceDescriptor);
	req.pData = &fIOUSBDeviceDescriptor;
	if (0 == fIOUSBDeviceInterface[0]->DeviceRequest(fIOUSBDeviceInterface, &req))
		fHasIOUSBDeviceDescriptor = true;
	}

// =================================================================================================
#pragma mark -
#pragma mark * StreamerR_TO

namespace ZANONYMOUS {

class StreamerR_TO
:	public ZStreamerR,
	private ZStreamR
	{
public:
	StreamerR_TO(ZRef<ZUSBInterfaceInterface> iUSBII,
		IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
		int iPipeRefR,
		size_t iBufferSize);

	virtual ~StreamerR_TO();

// From ZStreamerR via ZStreamerR
	virtual const ZStreamR& GetStreamR();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(int iMilliseconds);

private:
	bool pRefill(int iMilliseconds);

	ZRef<ZUSBInterfaceInterface> fUSBII;
	IOUSBInterfaceInterface190** fII;
	int fPipeRefR;
	const size_t fSize;
	uint8* fBuffer;
	size_t fOffset;
	size_t fEnd;
	};

} // anonymous namespace

StreamerR_TO::StreamerR_TO(ZRef<ZUSBInterfaceInterface> iUSBII,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
	int iPipeRefR,
	size_t iBufferSize)
:	fUSBII(iUSBII),
	fII(iIOUSBInterfaceInterface),
	fPipeRefR(iPipeRefR),
	fSize(iBufferSize),
	fBuffer(new uint8[iBufferSize]),
	fOffset(0),
	fEnd(0)
	{}

StreamerR_TO::~StreamerR_TO()
	{
	delete[] fBuffer;
	}

const ZStreamR& StreamerR_TO::GetStreamR()
	{ return *this; }

void StreamerR_TO::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(iDest);

	for (;;)
		{
		if (fEnd > fOffset)
			{
			const size_t countToCopy = min(fEnd - fOffset, iCount);
			ZBlockCopy(fBuffer + fOffset, localDest, countToCopy);
			localDest += countToCopy;
			fOffset += countToCopy;
			break;
			}
		else
			{
			if (!this->pRefill(1000))
				break;
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(iDest);
	}

size_t StreamerR_TO::Imp_CountReadable()
	{ return fEnd - fOffset; }

bool StreamerR_TO::Imp_WaitReadable(int iMilliseconds)
	{
	if (this->pRefill(iMilliseconds))
		return fEnd > fOffset;

	return true;
	}

bool StreamerR_TO::pRefill(int iMilliseconds)
	{
	if (fEnd > fOffset)
		return true;

	fOffset = 0;
	fEnd = 0;
	UInt32 localCount = fSize;
	IOReturn result = fII[0]->ReadPipeTO(
		fII, fPipeRefR, fBuffer, &localCount, iMilliseconds, 1000000);	

	if (kIOUSBTransactionTimeout == result)
		{
		fII[0]->ClearPipeStallBothEnds(fII, fPipeRefR);
		if (ZLOG(s, eDebug + 2, "StreamerR_TO"))
			{
			s << "pRefill, Timeout";
			}
		return true;
		}
	else if (0 == result)
		{
		if (ZLOG(s, eDebug + 2, "StreamerR_TO"))
			{
			s.Writef("pRefill, pipe: %d, ", fPipeRefR);
			ZUtil_Strim_Data::sDumpData(s, fBuffer, localCount);
			}
		fEnd = localCount;
		return true;
		}
	else
		{
		if (ZLOG(s, eDebug, "StreamerR_TO"))
			{
			s << "pRefill, Got result: " << sErrorAsString(result);

			}
		return false;
		}
	}

// =================================================================================================
#pragma mark -
#pragma mark * StreamerR_Async

namespace ZANONYMOUS {

class StreamerR_Async
:	public ZStreamerR,
	private ZStreamR
	{
public:
	StreamerR_Async(ZRef<ZUSBInterfaceInterface> iUSBII,
		IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
		int iPipeRefR,
		size_t iBufferSize);

	virtual ~StreamerR_Async();

// From ZStreamerR via ZStreamerR
	virtual const ZStreamR& GetStreamR();

// From ZStreamR
	virtual void Imp_Read(void* iDest, size_t iCount, size_t* oCountRead);
	virtual size_t Imp_CountReadable();
	virtual bool Imp_WaitReadable(int iMilliseconds);

private:
	void pTriggerRead();
	void pCompletion(IOReturn iResult, void* iArg);
	static void spCompletion(void* iRefcon, IOReturn iResult, void* iArg);

	ZRef<ZUSBInterfaceInterface> fUSBII;
	IOUSBInterfaceInterface190** fII;
	int fPipeRefR;

	ZMtx fMtx;
	ZCnd fCnd;
	uint8* fBuffer;	
	const size_t fSize;
	size_t fOffset;
	size_t fEnd;
	bool fPending;
	bool fOpen;
	};

} // anonymous namespace

StreamerR_Async::StreamerR_Async(ZRef<ZUSBInterfaceInterface> iUSBII,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
	int iPipeRefR,
	size_t iBufferSize)
:	fUSBII(iUSBII),
	fII(iIOUSBInterfaceInterface),
	fPipeRefR(iPipeRefR),
	fSize(iBufferSize),
	fBuffer(new uint8[iBufferSize]),
	fOffset(0),
	fEnd(0),
	fPending(false),
	fOpen(true)
	{}

StreamerR_Async::~StreamerR_Async()
	{
	ZGuardMtx guard(fMtx);
	fOpen = false;
	if (fPending)
		{
		fII[0]->AbortPipe(fII, fPipeRefR);
		while (fPending)
			fCnd.Wait(fMtx);
		}
	delete[] fBuffer;
	}

const ZStreamR& StreamerR_Async::GetStreamR()
	{ return *this; }

void StreamerR_Async::Imp_Read(void* iDest, size_t iCount, size_t* oCountRead)
	{
	uint8* localDest = static_cast<uint8*>(iDest);

	ZGuardMtx guard(fMtx);
	while (fOpen)
		{
		if (fOffset < fEnd)
			{
			const size_t countToCopy = min(fEnd - fOffset, iCount);
			ZBlockCopy(fBuffer + fOffset, localDest, countToCopy);
			localDest += countToCopy;
			fOffset += countToCopy;
			break;
			}
		else if (!fPending)
			{
			this->pTriggerRead();
			}
		else
			{
			fCnd.Wait(fMtx);
			}
		}

	if (oCountRead)
		*oCountRead = localDest - static_cast<uint8*>(iDest);
	}

size_t StreamerR_Async::Imp_CountReadable()
	{ return fEnd - fOffset; }

bool StreamerR_Async::Imp_WaitReadable(int iMilliseconds)
	{
	ZGuardMtx guard(fMtx);

	if (fOpen)
		{
		this->pTriggerRead();

		if (fOffset >= fEnd)
			fCnd.Wait(fMtx, iMilliseconds / 1e3);
		}

	return fOffset < fEnd;
	}

void StreamerR_Async::pTriggerRead()
	{
	if (fPending)
		return;

	ZAssert(fOffset >= fEnd);
	fOffset = 0;
	fEnd = 0;

	IOReturn result = fII[0]->ReadPipeAsync(
		fII, fPipeRefR, fBuffer, fSize, spCompletion, this);
	
	if (kIOReturnSuccess == result)
		fPending = true;
	}

void StreamerR_Async::pCompletion(IOReturn iResult, void* iArg)
	{
	// We'll deadlock if completion is called as a result of ReadPipeAsync being called.
	ZGuardMtx guard(fMtx);
	fPending = false;
	ZAssert(fOffset == 0 && fEnd == 0);
	fEnd = reinterpret_cast<UInt32>(iArg);
	fCnd.Broadcast();
	}

void StreamerR_Async::spCompletion(void* iRefcon, IOReturn iResult, void* iArg)
	{ static_cast<StreamerR_Async*>(iRefcon)->pCompletion(iResult, iArg); }

// =================================================================================================
#pragma mark -
#pragma mark * StreamerW

namespace ZANONYMOUS {

class StreamerW
:	public ZStreamerW,
	private ZStreamW
	{
public:
	StreamerW(ZRef<ZUSBInterfaceInterface> iUSBII,
		IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
		int iPipeRefW);

	virtual ~StreamerW();

// From ZStreamerW via ZStreamerRW
	virtual const ZStreamW& GetStreamW();

// From ZStreamW
	virtual void Imp_Write(const void* iSource, size_t iCount, size_t* oCountWritten);

private:
	ZRef<ZUSBInterfaceInterface> fUSBII;
	IOUSBInterfaceInterface190** fII;
	int fPipeRefW;
	};

} // anonymous namespace

StreamerW::StreamerW(ZRef<ZUSBInterfaceInterface> iUSBII,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface,
	int iPipeRefW)
:	fUSBII(iUSBII),
	fII(iIOUSBInterfaceInterface),
	fPipeRefW(iPipeRefW)
	{}

StreamerW::~StreamerW()
	{}

const ZStreamW& StreamerW::GetStreamW()
	{ return *this; }

void StreamerW::Imp_Write(
	const void* iSource, size_t iCount, size_t* oCountWritten)
	{
//	if (size_t countToWrite = min(size_t(1024), iCount))
	if (size_t countToWrite = iCount)
		{
		if (ZLOG(s, eDebug + 2, "ZUSBInterfaceInterface::StreamerW"))
			{
			s.Writef("Imp_Write, pipe: %d, ", fPipeRefW);
			ZUtil_Strim_Data::sDumpData(s, iSource, countToWrite);
			}

		if (0 == fII[0]->WritePipe(fII, fPipeRefW, const_cast<void*>(iSource), countToWrite))
			{
			if (oCountWritten)
				*oCountWritten = countToWrite;
			return;
			}
		}

	if (oCountWritten)
		*oCountWritten = 0;
	}

// =================================================================================================
#pragma mark -
#pragma mark * ZUSBInterfaceInterface

ZUSBInterfaceInterface::ZUSBInterfaceInterface(ZRef<ZUSBDevice> iUSBDevice,
	CFRunLoopRef iRunLoopRef,
	IOUSBInterfaceInterface190** iIOUSBInterfaceInterface)
:	fUSBDevice(iUSBDevice),
	fII(iIOUSBInterfaceInterface),
	fRunLoopRef(iRunLoopRef),
	fOpen(true)
	{
	CFRunLoopSourceRef theSource;
	fII[0]->CreateInterfaceAsyncEventSource(fII, &theSource);
	::CFRunLoopAddSource(fRunLoopRef, theSource, kCFRunLoopDefaultMode);
	}

ZUSBInterfaceInterface::~ZUSBInterfaceInterface()
	{
	CFRunLoopSourceRef theSource = fII[0]->GetInterfaceAsyncEventSource(fII);
	::CFRunLoopRemoveSource(fRunLoopRef, theSource, kCFRunLoopDefaultMode);


	if (fOpen)
		fII[0]->USBInterfaceClose(fII);
	fII[0]->Release(fII);
	fII = nil;
	}

ZRef<ZUSBDevice> ZUSBInterfaceInterface::GetUSBDevice()
	{ return fUSBDevice; }

ZRef<ZStreamerR> ZUSBInterfaceInterface::OpenR(int iPipeRefR)
	{
	if (fOpen)
//		return new StreamerR_Async(this, fII, iPipeRefR, 1024);
		return new StreamerR_TO(this, fII, iPipeRefR, 1024);



	return ZRef<ZStreamerR>();
	}

ZRef<ZStreamerW> ZUSBInterfaceInterface::OpenW(int iPipeRefW)
	{
	if (fOpen)
		{
		// We call ClearPipeStallBothEnds in case a previous use had not cleanly
		// disconnected, which can leave the peripheral device stalled. Read will
		// do this whenever it times out, so we don't have to do it in OpenR.
		fII[0]->ClearPipeStallBothEnds(fII, iPipeRefW);
		return new StreamerW(this, fII, iPipeRefW);
		}
	return ZRef<ZStreamerW>();
	}

void ZUSBInterfaceInterface::Close()
	{
	if (fOpen)
		{
		fOpen = false;
		fII[0]->USBInterfaceClose(fII);
		}
	}

NAMESPACE_ZOOLIB_END

#endif // ZCONFIG_API_Enabled(USB_OSX)
