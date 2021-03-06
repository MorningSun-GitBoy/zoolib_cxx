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

#include "zoolib/HTTP/Content.h"

#include "zoolib/Chan_XX_Limited.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Memory.h"
#include "zoolib/MIME.h"
#include "zoolib/Util_Chan.h"
#include "zoolib/Util_string.h"

namespace ZooLib {
namespace HTTP {

using std::max;
using std::min;

// =================================================================================================
#pragma mark - Helpers (anonymous)

namespace { // anonymous

} // anonymous namespace

// =================================================================================================
#pragma mark - HTTP::ChanR_Bin_Chunked

static uint64 spReadChunkSize(const ChanR_Bin& iChanR)
	{
	uint64 result = 0;
	for (;;)
		{
		ZQ<byte> theQ = sQRead(iChanR);
		if (not theQ)
			return 0;

		int theXDigit = 0;
		if (*theQ >= '0' && *theQ <= '9')
			{
			theXDigit = *theQ - '0';
			}
		else if (*theQ >= 'a' && *theQ <= 'f')
			{
			theXDigit = *theQ - 'a' + 10;
			}
		else if (*theQ >= 'A' && *theQ <= 'F')
			{
			theXDigit = *theQ - 'A' + 10;
			}
		else
			{
			if (*theQ != '\n')
				{
				// Skip everything till we hit a LF
				sSkip_Until<byte>(iChanR, byte('\n'));
				}
			break;
			}
		result = (result << 4) + theXDigit;
		}

	return result;
	}

ChanR_Bin_Chunked::ChanR_Bin_Chunked(const ChanR_Bin& iChanR)
:	fChanR(iChanR)
	{
	fChunkSize = spReadChunkSize(fChanR);
	fHitEnd = fChunkSize == 0;
	}

ChanR_Bin_Chunked::~ChanR_Bin_Chunked()
	{}

size_t ChanR_Bin_Chunked::Read(byte* oDest, size_t iCount)
	{
	byte* localDest = oDest;
	while (iCount && !fHitEnd)
		{
		if (fChunkSize == 0)
			{
			// Read and discard the CRLF at the end of the chunk.
			const uint64 countSkipped = sSkip(fChanR, 2);
			if (countSkipped == 2)
				fChunkSize = spReadChunkSize(fChanR);
			if (fChunkSize == 0)
				fHitEnd = true;
			}
		else
			{
			const size_t countRead = sRead(fChanR, 
				localDest, std::min<size_t>(iCount, sClamped(fChunkSize)));

			if (countRead == 0)
				fHitEnd = true;
			localDest += countRead;
			iCount -= countRead;
			fChunkSize -= countRead;
			}
		}
	return localDest - oDest;
	}

size_t ChanR_Bin_Chunked::Readable()
	{ return min<size_t>(sClamped(fChunkSize), sReadable(fChanR)); }

// =================================================================================================
#pragma mark - HTTP::ChanW_Bin_Chunked

ChanW_Bin_Chunked::ChanW_Bin_Chunked(size_t iBufferSize, const ChanW_Bin& iChanW)
:	fChanW(iChanW),
	fBuffer(max(size_t(64), iBufferSize), 0),
	fBufferUsed(0)
	{}

ChanW_Bin_Chunked::ChanW_Bin_Chunked(const ChanW_Bin& iChanW)
:	fChanW(iChanW),
	fBuffer(1024, 0),
	fBufferUsed(0)
	{}

ChanW_Bin_Chunked::~ChanW_Bin_Chunked()
	{
	try
		{
		this->pFlush();

		// Terminating zero-length chunk
		sEWrite(fChanW, "0\r\n");

		// There's supposed to be an additional CRLF at the end of all the data,
		// after any trailer entity headers.
		sEWrite(fChanW, "\r\n");
		}
	catch (...)
		{}
	}

size_t ChanW_Bin_Chunked::Write(const byte* iSource, size_t iCount)
	{
	const uint8* localSource = reinterpret_cast<const uint8*>(iSource);
	while (iCount)
		{
		if (fBufferUsed + iCount >= fBuffer.size())
			{
			// The data would overflow the buffer, so we can write the
			// buffer content (if any) plus this new stuff.
			sEWritef(fChanW, "%X\r\n", fBufferUsed + iCount);
			// Hmmm. Do we allow an end of stream exception to propogate?
			sEWrite(fChanW, &fBuffer[0], fBufferUsed);
			fBufferUsed = 0;
			sEWrite(fChanW, localSource, iCount);
			sEWrite(fChanW, "\r\n");
			localSource += iCount;
			iCount = 0;
			}
		else
			{
			size_t countToCopy = min(iCount, size_t(fBuffer.size() - fBufferUsed));
			sMemCopy(&fBuffer[0] + fBufferUsed, localSource, countToCopy);
			fBufferUsed += countToCopy;
			iCount -= countToCopy;
			localSource += countToCopy;
			}
		}
	return localSource - reinterpret_cast<const uint8*>(iSource);
	}

void ChanW_Bin_Chunked::Flush()
	{
	this->pFlush();
	sFlush(fChanW);
	}

void ChanW_Bin_Chunked::pFlush()
	{
	if (const size_t bufferUsed = fBufferUsed)
		{
		fBufferUsed = 0;
		sEWritef(fChanW, "%X\r\n", bufferUsed);
		sEWrite(fChanW, &fBuffer[0], bufferUsed);
		sEWrite(fChanW, "\r\n");
		}
	}

// =================================================================================================
#pragma mark - HTTP::sMakeContentChanner

static ZP<ChannerR_Bin> spMakeChanner_Transfer(
	const Map& iHeader, const ZP<ChannerR_Bin>& iChannerR)
	{
	// According to the spec, if content is chunked, content-length must be ignored.
	// I've seen some pages being returned with transfer-encoding "chunked, chunked", which
	// is either a mistake, or is nested chunking. I'm assuming the former for now.

	if (Util_string::sContainsi(sGetString0(iHeader.Get("transfer-encoding")), "chunked"))
		return sChanner_Channer_T<ChanR_Bin_Chunked>(iChannerR);

	if (ZQ<int64> contentLength = iHeader.QGet<int64>("content-length"))
		return sChanner_Channer_T<ChanR_XX_Limited<ChanR_Bin>>(iChannerR, *contentLength);

	return iChannerR;
	}

ZP<ChannerR_Bin> sMakeContentChanner(const Map& iHeader, ZP<ChannerR_Bin> iChannerR)
	{
	iChannerR = spMakeChanner_Transfer(iHeader, iChannerR);

	// We could/should look for gzip Content-Encoding, and wrap a decoding filter around it.

	return iChannerR;
	}

ZP<ChannerR_Bin> sMakeContentChanner(
	const std::string& iMethod, int iResponseCode,
	const Map& iHeader, const ZP<ChannerR_Bin>& iChannerR)
	{
	if (iMethod == "HEAD")
		{
		return sChanner_T<ChanR_XX_Null<byte>>();
		}

	if (iMethod == "GET" && iResponseCode == 304)
		{
		// RFC 2616, section 10.3.5. 
		// "The 304 response MUST NOT contain a message-body, and thus is
		// always terminated by the first empty line after the header fields."
		return sChanner_T<ChanR_XX_Null<byte>>();
		}

	return sMakeContentChanner(iHeader, iChannerR);
	}

} // namespace HTTP
} // namespace ZooLib
