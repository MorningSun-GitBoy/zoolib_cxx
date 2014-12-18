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

#include "zoolib/Chan_Bin_Data.h"
#include "zoolib/ChanW_Bin_More.h"
#include "zoolib/Chan_XX_Tee.h"
#include "zoolib/ChanRU_XX_Unreader.h"
#include "zoolib/HTTP.h"
#include "zoolib/HTTP_Content.h" // For ChanW_Bin_Chunked.
#include "zoolib/HTTP_Requests.h"
#include "zoolib/Log.h"
#include "zoolib/MIME.h"
#include "zoolib/Stringf.h"
#include "zoolib/Util_string.h"

namespace ZooLib {
namespace HTTP {

// =================================================================================================
// MARK: - Helpers (anonymous)

namespace { // anonymous

static bool spQReadResponse(const ChanR_Bin& iChanR, int32* oResponseCode, Map* oHeader)
	{
	MIME::ChanR_Bin_Header theSIH_Server(iChanR);
	ChanRU_XX_Unreader<byte> theChanRU(theSIH_Server);

	string serverResultMessage;
	if (not sQReadResponse(theChanRU, theChanRU, oResponseCode, nullptr))
		return false;

	return sQReadHeader(theChanRU, oHeader);
	}

static
ZQ<Connection_t> spQConnect(ZRef<Callable_QConnect> iCallable_QConnect,
	const string& iScheme, const string& iHost, uint16 iPort)
	{
	const bool useSSL = Util_string::sEquali("https", iScheme)
		|| Util_string::sEquali("wss", iScheme);

	uint16 thePort = iPort;
	if (not thePort)
		{
		if (useSSL)
			thePort = 443;
		else
			thePort = 80;
		}

	if (iCallable_QConnect)
		return iCallable_QConnect->Call(iHost, thePort, useSSL);

	return sQConnect(iHost, thePort, useSSL);
	}

} // anonymous namespace

// =================================================================================================
// MARK: - HTTP::sRequest

static bool spQRequest(const ChanR_Bin& iChanR, const ChanW_Bin& iChanW,
	const string& iMethod, const string& iHost, const string& iPath, const Map* iHeader,
	bool iSendConnectionClose,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	iChanW << iMethod << " " << iPath << " HTTP/1.1\r\n" << "Host: " << iHost << "\r\n";

	if (iHeader)
		sWrite_Header(*iHeader, iChanW);

	if (iSendConnectionClose)
		iChanW << "Connection: close\r\n";

	iChanW << "\r\n";

	sFlush(iChanW);

	if (oRawHeader)
		{
		ChanRWPos_Bin_Data<Data> theSRWP_Data(oRawHeader);
		ChanR_XX_Tee<byte> theTee(iChanR, theSRWP_Data);
		return spQReadResponse(theTee, oResponseCode, oHeader);
		}
	else
		{
		return spQReadResponse(iChanR, oResponseCode, oHeader);
		}
	}

bool sQRequest(ZQ<Connection_t>& ioConnectionQ,
	const ZRef<Callable_QConnect>& iCallable_QConnect,
	const string& iMethod, const string& iURL, const Map* iHeader,
	bool iConnectionClose,
	string* oURL, int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	string theURL = iURL;

	for (;;)
		{
		if (oURL)
			*oURL = theURL;

		if (oResponseCode)
			*oResponseCode = 0;

		if (oHeader)
			oHeader->Clear();

		if (oRawHeader)
			oRawHeader->SetSize(0);

		string theScheme;
		string theHost;
		uint16 thePort;
		string thePath;
		if (sParseURL(theURL, &theScheme, &theHost, &thePort, &thePath))
			{
			// If ioConnectionQ is valid, we *assume* that it is a live connection
			// to the server for theURL
			const bool wasExtantConnection = bool(ioConnectionQ);
			if (not wasExtantConnection)
				ioConnectionQ = spQConnect(iCallable_QConnect, theScheme, theHost, thePort);

			if (not ioConnectionQ)
				{
				// We have no physical connection, our caller can do any retry it desires.
				return false;
				}

			int32 theResponseCode;
			Map theResponseHeader;
			if (not spQRequest(
				sGetChan(ioConnectionQ->GetR()), sGetChan(ioConnectionQ->GetW()),
				iMethod, theHost + sStringf(":%d", thePort), thePath, iHeader,
				iConnectionClose,
				&theResponseCode, &theResponseHeader, oRawHeader))
				{
				// We failed to interpret the response. We've thus lost track of our position in any
				// request/response sequence and the connection cannot (continue to) be reused.
				sClear(ioConnectionQ);

				if (wasExtantConnection)
					{
					// The connection was reused, so a prior request had worked. Assume something
					// minor messed up and go around. If the same error recurs wasExtantConnection
					// will be false and we won't enter this branch, so we get one retry for
					// each physical connection.
					if (ZLOGF(w, eDebug))
						w << "Request failed on reused connection, retrying once: " << theURL;
					continue;
					}
				// The connection was fresh, assume the problem is real.
				return false;
				}

			if (oResponseCode)
				*oResponseCode = theResponseCode;

			if (oHeader)
				*oHeader = theResponseHeader;

			switch (theResponseCode)
				{
				case 301:
				case 302:
				case 303:
					{
					theURL = sAbsoluteURI(theURL, sGetString0(theResponseHeader.Get("location")));
					// Read and discard the body
					if (ZRef<ChannerR_Bin> theChannerR_Content = sMakeContentChanner(
						iMethod, theResponseCode, theResponseHeader, ioConnectionQ->GetR()))
						{ sSkipAll(sGetChan(theChannerR_Content)); }
					break;
					}
				default:
					{
					return true;
					}
				}
			}
		}
	}

// =================================================================================================
// MARK: - HTTP::sPOST

static void spPOST_Prefix(const ChanW_Bin& w,
	const string& iMethod,
	const string& iHost, const string& iPath, const Map* iHeader, bool iSendConnectionClose)
	{
	w << iMethod << " " << iPath << " HTTP/1.1\r\n" << "Host: " << iHost << "\r\n";
	if (iSendConnectionClose)
		w << "Connection: close\r\n";
	if (iHeader)
		sWrite_Header(*iHeader, w);
	}

ZQ<Connection_t> sQPOST_Send(ZRef<Callable_QConnect> iCallable_QConnect,
	const string& iMethod,
	const string& iURL, const Map* iHeader, const ChanR_Bin& iBody, ZQ<uint64> iBodyCountQ)
	{
	string theScheme;
	string theHost;
	uint16 thePort;
	string thePath;
	if (sParseURL(iURL, &theScheme, &theHost, &thePort, &thePath))
		{
		if (ZQ<Connection_t> theConnQ = spQConnect(iCallable_QConnect, theScheme, theHost, thePort))
			{
			const ChanW_Bin& theChanW = sGetChan(theConnQ->GetW());

			spPOST_Prefix(theChanW, iMethod, theHost, thePath, iHeader, true);

			if (iBodyCountQ)
				{
				sWritefMust(theChanW, "Content-Length: %lld\r\n", *iBodyCountQ);
				theChanW << "\r\n";
				sCopyFully(iBody, theChanW, *iBodyCountQ);
				}
			else
				{
				theChanW << "Transfer-Encoding: chunked\r\n";
				theChanW << "\r\n";
				sCopyAll(iBody, ChanW_Bin_Chunked(16 *1024, theChanW));
				}
			sFlush(theChanW);
			return theConnQ;
			}
		}
	return null;
	}

static bool spQPOST_Suffix(const ChanR_Bin& iChanR,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (oRawHeader)
		{
		ChanRWPos_Bin_Data<Data> theSRWP_Data(oRawHeader);
		ChanR_XX_Tee<byte> theTee(iChanR, theSRWP_Data);
		return spQReadResponse(theTee, oResponseCode, oHeader);
		}
	else
		{
		return spQReadResponse(iChanR, oResponseCode, oHeader);
		}
	}

ZQ<Connection_t> sQPOST_Receive(const ZQ<Connection_t>& iConnQ,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (iConnQ)
		{
		int32 theResponseCode;
		Map theResponseHeader;
		if (spQPOST_Suffix(sGetChan(iConnQ->GetR()),
			&theResponseCode, &theResponseHeader, oRawHeader))
			{
			if (oResponseCode)
				*oResponseCode = theResponseCode;

			if (theResponseCode >= 200 && theResponseCode < 300)
				{
				if (oHeader)
					*oHeader = theResponseHeader;

				ZQ<Connection_t> theConnQ = iConnQ;
				if (ZRef<ChannerR_Bin> theChanner =
					sMakeContentChanner(theResponseHeader, theConnQ->GetR()))
					{ theConnQ->SetR(theChanner); }

				return theConnQ;
				}
			}
		}
	return null;
	}

ZQ<Connection_t> sQPOST(ZRef<Callable_QConnect> iCallable_QConnect,
	const string& iURL, const Map* iHeader, const ChanR_Bin& iBody, ZQ<uint64> iBodyCountQ,
	int32* oResponseCode, Map* oHeader, Data* oRawHeader)
	{
	if (ZQ<Connection_t> theConnQ = sQPOST_Send(iCallable_QConnect,
		"POST", iURL, iHeader, iBody, iBodyCountQ))
		{
		return sQPOST_Receive(theConnQ, oResponseCode, oHeader, oRawHeader);
		}
	return null;
	}

// =================================================================================================
// MARK: - sQCONNECT

bool sQCONNECT(const ChanR_Bin& r, const ChanW_Bin& w,
	const std::string& iAddress, const Map* iHeader,
	int32* oResponseCode, Map* oHeader)
	{
	if (oResponseCode)
		*oResponseCode = 0;

	w << "CONNECT " << iAddress << " HTTP/1.0\r\n";

	if (iHeader)
		sWrite_Header(*iHeader, w);

	w << "\r\n";

	sFlush(w);

	int32 serverResponseCode;
	ChanRU_XX_Unreader<byte> theChanRU(r);
	if (sQReadResponse(theChanRU, theChanRU, &serverResponseCode, nullptr))
		{
		if (oResponseCode)
			*oResponseCode = serverResponseCode;

		if (sQReadHeader(r, oHeader))
			return serverResponseCode == 200;
		}

	return false;
	}

} // namespace HTTP
} // namespace ZooLib
