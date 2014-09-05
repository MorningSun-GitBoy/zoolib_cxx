/* -------------------------------------------------------------------------------------------------
Copyright (c) 2005 Andrew Green and Learning in Motion, Inc.
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

#include "zoolib/ZTextCoder_Std.h"

#include "zoolib/ZCompat_algorithm.h" // For lower_bound
#include "zoolib/ZFunctionChain.h"
#include "zoolib/ZUnicode.h"

#include <string>

ZMACRO_MSVCStaticLib_cpp(TextCoder_Std)

using std::string;

namespace ZooLib {

// =================================================================================================
// MARK: - Factory functions

namespace { // anonymous

class Make_Decoder
:	public ZFunctionChain_T<ZTextDecoder*, const string&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (false) {}
		else if (iParam == "ascii") oResult = new ZTextDecoder_ASCII;
		else if (iParam == "cp1252") oResult = new ZTextDecoder_CP1252;
		else if (iParam == "cp850") oResult = new ZTextDecoder_CP850;
		else if (iParam == "macroman") oResult = new ZTextDecoder_MacRoman;
		else if (iParam == "iso_8859-1" || iParam == "iso-8859-1")
			oResult = new ZTextDecoder_ISO8859_1;
		else return false;

		return true;
		}
	} sMaker0;

class Make_Encoder
:	public ZFunctionChain_T<ZTextEncoder*, const string&>
	{
	virtual bool Invoke(Result_t& oResult, Param_t iParam)
		{
		if (false) {}
		else if (iParam == "ascii") oResult = new ZTextEncoder_ASCII;
		else if (iParam == "cp1252") oResult = new ZTextEncoder_CP1252;
		else if (iParam == "cp850") oResult = new ZTextEncoder_CP850;
		else if (iParam == "macroman") oResult = new ZTextEncoder_MacRoman;
		else if (iParam == "iso_8859-1" || iParam == "iso-8859-1")
			oResult = new ZTextEncoder_ISO8859_1;
		else return false;

		return true;
		}
	} sMaker1;

} // anonymous namespace

// =================================================================================================
// MARK: - ZTextDecoder_ASCII

bool ZTextDecoder_ASCII::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	const uint8* localSource = static_cast<const uint8*>(iSource);
	const uint8* localSourceEnd = localSource + iSourceBytes;
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iDestCU;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		{
		uint32 theCP = *localSource++;
		if (theCP <= 0x7F)
			*localDest++ = theCP;
		}

	if (oSourceBytes)
		*oSourceBytes = localSource - static_cast<const uint8*>(iSource);
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = 0;
	if (oDestCU)
		*oDestCU = localDest - oDest;
	return true;
	}

// =================================================================================================
// MARK: - ZTextEncoder_ASCII

void ZTextEncoder_ASCII::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	const UTF32* localSource = iSource;
	const UTF32* localSourceEnd = iSource + iSourceCU;
	uint8* localDest = static_cast<uint8*>(oDest);
	uint8* localDestEnd = localDest + iDestBytes;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		{
		UTF32 theCP;
		if (not ZUnicode::sReadInc(localSource, localSourceEnd, theCP))
			break;
		if (uint32(theCP) <= 0x7F)
			*localDest++ = uint8(theCP);
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestBytes)
		*oDestBytes = localDest - static_cast<uint8*>(oDest);
	}

// =================================================================================================
// MARK: - ZTextDecoder_ISO8859_1

bool ZTextDecoder_ISO8859_1::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	const uint8* localSource = static_cast<const uint8*>(iSource);
	const uint8* localSourceEnd = localSource + iSourceBytes;
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iDestCU;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		*localDest++ = uint32(*localSource++);

	if (oSourceBytes)
		*oSourceBytes = localSource - static_cast<const uint8*>(iSource);
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = 0;
	if (oDestCU)
		*oDestCU = localDest - oDest;
	return true;
	}

// =================================================================================================
// MARK: - ZTextEncoder_ISO8859_1

void ZTextEncoder_ISO8859_1::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	const UTF32* localSource = iSource;
	const UTF32* localSourceEnd = iSource + iSourceCU;
	uint8* localDest = static_cast<uint8*>(oDest);
	uint8* localDestEnd = localDest + iDestBytes;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		{
		UTF32 theCP;
		if (not ZUnicode::sReadInc(localSource, localSourceEnd, theCP))
			break;
		if (uint32(theCP) <= 0xFF)
			*localDest++ = theCP;
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestBytes)
		*oDestBytes = localDest - static_cast<uint8*>(oDest);
	}

// =================================================================================================
// MARK: - ZTextDecoder_MacRoman

static const UTF32 spMacToUnicode[256] =
	{
	0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
	0x0008, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x000F,
	0x0010, 0x0011, 0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017,
	0x0018, 0x0019, 0x001A, 0x001B, 0x001C, 0x001D, 0x001E, 0x001F,
	0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
	0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 0x002F,
	0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
	0x0038, 0x0039, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F,
	0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
	0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
	0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
	0x0058, 0x0059, 0x005A, 0x005B, 0x005C, 0x005D, 0x005E, 0x005F,
	0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
	0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
	0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
	0x0078, 0x0079, 0x007A, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F,
	0x00C4, 0x00C5, 0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1,
	0x00E0, 0x00E2, 0x00E4, 0x00E3, 0x00E5, 0x00E7, 0x00E9, 0x00E8,
	0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 0x00F1, 0x00F3,
	0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC,
	0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF,
	0x00AE, 0x00A9, 0x2122, 0x00B4, 0x00A8, 0x2260, 0x00C6, 0x00D8,
	0x221E, 0x00B1, 0x2264, 0x2265, 0x00A5, 0x00B5, 0x2202, 0x2211,
	0x220F, 0x03C0, 0x222B, 0x00AA, 0x00BA, 0x03A9, 0x00E6, 0x00F8,
	0x00BF, 0x00A1, 0x00AC, 0x221A, 0x0192, 0x2248, 0x2206, 0x00AB,
	0x00BB, 0x2026, 0x00A0, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153,
	0x2013, 0x2014, 0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x25CA,
	0x00FF, 0x0178, 0x2044, 0x20AC, 0x2039, 0x203A, 0xFB01, 0xFB02,
	0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 0x00CA, 0x00C1,
	0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4,
	0xF8FF, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC,
	0x00AF, 0x02D8, 0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7
	};

bool ZTextDecoder_MacRoman::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	const uint8* localSource = static_cast<const uint8*>(iSource);
	const uint8* localSourceEnd = localSource + iSourceBytes;
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iDestCU;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		*localDest++ = spMacToUnicode[*localSource++];

	if (oSourceBytes)
		*oSourceBytes = localSource - static_cast<const uint8*>(iSource);
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = 0;
	if (oDestCU)
		*oDestCU = localDest - oDest;
	return true;
	}

// =================================================================================================
// MARK: - ZTextEncoder_MacRoman

struct UnicodeToMac_t
	{
	uint8 fMac;
	UTF32 fUnicode;

	bool operator==(const UnicodeToMac_t& rhs) const { return fUnicode == rhs.fUnicode; }
	bool operator<(const UnicodeToMac_t& rhs) const { return fUnicode < rhs.fUnicode; }
	};

static const UnicodeToMac_t spUnicodeToMac[256] =
	{
	{0x00, 0x0000}, {0x01, 0x0001}, {0x02, 0x0002}, {0x03, 0x0003},
	{0x04, 0x0004}, {0x05, 0x0005}, {0x06, 0x0006}, {0x07, 0x0007},
	{0x08, 0x0008}, {0x09, 0x0009}, {0x0A, 0x000A}, {0x0B, 0x000B},
	{0x0C, 0x000C}, {0x0D, 0x000D}, {0x0E, 0x000E}, {0x0F, 0x000F},
	{0x10, 0x0010}, {0x11, 0x0011}, {0x12, 0x0012}, {0x13, 0x0013},
	{0x14, 0x0014}, {0x15, 0x0015}, {0x16, 0x0016}, {0x17, 0x0017},
	{0x18, 0x0018}, {0x19, 0x0019}, {0x1A, 0x001A}, {0x1B, 0x001B},
	{0x1C, 0x001C}, {0x1D, 0x001D}, {0x1E, 0x001E}, {0x1F, 0x001F},
	{0x20, 0x0020}, {0x21, 0x0021}, {0x22, 0x0022}, {0x23, 0x0023},
	{0x24, 0x0024}, {0x25, 0x0025}, {0x26, 0x0026}, {0x27, 0x0027},
	{0x28, 0x0028}, {0x29, 0x0029}, {0x2A, 0x002A}, {0x2B, 0x002B},
	{0x2C, 0x002C}, {0x2D, 0x002D}, {0x2E, 0x002E}, {0x2F, 0x002F},
	{0x30, 0x0030}, {0x31, 0x0031}, {0x32, 0x0032}, {0x33, 0x0033},
	{0x34, 0x0034}, {0x35, 0x0035}, {0x36, 0x0036}, {0x37, 0x0037},
	{0x38, 0x0038}, {0x39, 0x0039}, {0x3A, 0x003A}, {0x3B, 0x003B},
	{0x3C, 0x003C}, {0x3D, 0x003D}, {0x3E, 0x003E}, {0x3F, 0x003F},
	{0x40, 0x0040}, {0x41, 0x0041}, {0x42, 0x0042}, {0x43, 0x0043},
	{0x44, 0x0044}, {0x45, 0x0045}, {0x46, 0x0046}, {0x47, 0x0047},
	{0x48, 0x0048}, {0x49, 0x0049}, {0x4A, 0x004A}, {0x4B, 0x004B},
	{0x4C, 0x004C}, {0x4D, 0x004D}, {0x4E, 0x004E}, {0x4F, 0x004F},
	{0x50, 0x0050}, {0x51, 0x0051}, {0x52, 0x0052}, {0x53, 0x0053},
	{0x54, 0x0054}, {0x55, 0x0055}, {0x56, 0x0056}, {0x57, 0x0057},
	{0x58, 0x0058}, {0x59, 0x0059}, {0x5A, 0x005A}, {0x5B, 0x005B},
	{0x5C, 0x005C}, {0x5D, 0x005D}, {0x5E, 0x005E}, {0x5F, 0x005F},
	{0x60, 0x0060}, {0x61, 0x0061}, {0x62, 0x0062}, {0x63, 0x0063},
	{0x64, 0x0064}, {0x65, 0x0065}, {0x66, 0x0066}, {0x67, 0x0067},
	{0x68, 0x0068}, {0x69, 0x0069}, {0x6A, 0x006A}, {0x6B, 0x006B},
	{0x6C, 0x006C}, {0x6D, 0x006D}, {0x6E, 0x006E}, {0x6F, 0x006F},
	{0x70, 0x0070}, {0x71, 0x0071}, {0x72, 0x0072}, {0x73, 0x0073},
	{0x74, 0x0074}, {0x75, 0x0075}, {0x76, 0x0076}, {0x77, 0x0077},
	{0x78, 0x0078}, {0x79, 0x0079}, {0x7A, 0x007A}, {0x7B, 0x007B},
	{0x7C, 0x007C}, {0x7D, 0x007D}, {0x7E, 0x007E}, {0x7F, 0x007F},
	{0xCA, 0x00A0}, {0xC1, 0x00A1}, {0xA2, 0x00A2}, {0xA3, 0x00A3},
	{0xB4, 0x00A5}, {0xA4, 0x00A7}, {0xAC, 0x00A8}, {0xA9, 0x00A9},
	{0xBB, 0x00AA}, {0xC7, 0x00AB}, {0xC2, 0x00AC}, {0xA8, 0x00AE},
	{0xF8, 0x00AF}, {0xA1, 0x00B0}, {0xB1, 0x00B1}, {0xAB, 0x00B4},
	{0xB5, 0x00B5}, {0xA6, 0x00B6}, {0xE1, 0x00B7}, {0xFC, 0x00B8},
	{0xBC, 0x00BA}, {0xC8, 0x00BB}, {0xC0, 0x00BF}, {0xCB, 0x00C0},
	{0xE7, 0x00C1}, {0xE5, 0x00C2}, {0xCC, 0x00C3}, {0x80, 0x00C4},
	{0x81, 0x00C5}, {0xAE, 0x00C6}, {0x82, 0x00C7}, {0xE9, 0x00C8},
	{0x83, 0x00C9}, {0xE6, 0x00CA}, {0xE8, 0x00CB}, {0xED, 0x00CC},
	{0xEA, 0x00CD}, {0xEB, 0x00CE}, {0xEC, 0x00CF}, {0x84, 0x00D1},
	{0xF1, 0x00D2}, {0xEE, 0x00D3}, {0xEF, 0x00D4}, {0xCD, 0x00D5},
	{0x85, 0x00D6}, {0xAF, 0x00D8}, {0xF4, 0x00D9}, {0xF2, 0x00DA},
	{0xF3, 0x00DB}, {0x86, 0x00DC}, {0xA7, 0x00DF}, {0x88, 0x00E0},
	{0x87, 0x00E1}, {0x89, 0x00E2}, {0x8B, 0x00E3}, {0x8A, 0x00E4},
	{0x8C, 0x00E5}, {0xBE, 0x00E6}, {0x8D, 0x00E7}, {0x8F, 0x00E8},
	{0x8E, 0x00E9}, {0x90, 0x00EA}, {0x91, 0x00EB}, {0x93, 0x00EC},
	{0x92, 0x00ED}, {0x94, 0x00EE}, {0x95, 0x00EF}, {0x96, 0x00F1},
	{0x98, 0x00F2}, {0x97, 0x00F3}, {0x99, 0x00F4}, {0x9B, 0x00F5},
	{0x9A, 0x00F6}, {0xD6, 0x00F7}, {0xBF, 0x00F8}, {0x9D, 0x00F9},
	{0x9C, 0x00FA}, {0x9E, 0x00FB}, {0x9F, 0x00FC}, {0xD8, 0x00FF},
	{0xF5, 0x0131}, {0xCE, 0x0152}, {0xCF, 0x0153}, {0xD9, 0x0178},
	{0xC4, 0x0192}, {0xF6, 0x02C6}, {0xFF, 0x02C7}, {0xF9, 0x02D8},
	{0xFA, 0x02D9}, {0xFB, 0x02DA}, {0xFE, 0x02DB}, {0xF7, 0x02DC},
	{0xFD, 0x02DD}, {0xBD, 0x03A9}, {0xB9, 0x03C0}, {0xD0, 0x2013},
	{0xD1, 0x2014}, {0xD4, 0x2018}, {0xD5, 0x2019}, {0xE2, 0x201A},
	{0xD2, 0x201C}, {0xD3, 0x201D}, {0xE3, 0x201E}, {0xA0, 0x2020},
	{0xE0, 0x2021}, {0xA5, 0x2022}, {0xC9, 0x2026}, {0xE4, 0x2030},
	{0xDC, 0x2039}, {0xDD, 0x203A}, {0xDA, 0x2044}, {0xDB, 0x20AC},
	{0xAA, 0x2122}, {0xB6, 0x2202}, {0xC6, 0x2206}, {0xB8, 0x220F},
	{0xB7, 0x2211}, {0xC3, 0x221A}, {0xB0, 0x221E}, {0xBA, 0x222B},
	{0xC5, 0x2248}, {0xAD, 0x2260}, {0xB2, 0x2264}, {0xB3, 0x2265},
	{0xD7, 0x25CA}, {0xF0, 0xF8FF}, {0xDE, 0xFB01}, {0xDF, 0xFB02}
	};

void ZTextEncoder_MacRoman::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	const UTF32* localSource = iSource;
	const UTF32* localSourceEnd = iSource + iSourceCU;
	uint8* localDest = static_cast<uint8*>(oDest);
	uint8* localDestEnd = localDest + iDestBytes;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		{
		UnicodeToMac_t codeLookup;
		if (not ZUnicode::sReadInc(localSource, localSourceEnd, codeLookup.fUnicode))
			break;

		const UnicodeToMac_t* iter = std::lower_bound(
			spUnicodeToMac, spUnicodeToMac + 256, codeLookup);

		if (iter < spUnicodeToMac + 256 && *iter == codeLookup)
			*localDest++ = iter->fMac;
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestBytes)
		*oDestBytes = localDest - static_cast<uint8*>(oDest);
	}

// =================================================================================================
// MARK: - ZTextDecoder_CP1252

// CP1252 is also known as Latin1. It's a superset of ISO-8859-1, but has various
// dingbat characters in CL and CR

static const UTF32 spCP1252ToUnicode[128] =
	{
	/* 0x80 */
	0x20ac, 0xfffd, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
	0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0xfffd, 0x017d, 0xfffd,
	/* 0x90 */
	0xfffd, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
	0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0xfffd, 0x017e, 0x0178,
	};

bool ZTextDecoder_CP1252::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	const uint8* localSource = static_cast<const uint8*>(iSource);
	const uint8* localSourceEnd = localSource + iSourceBytes;
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iDestCU;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		{
		uint8 readByte = *localSource++;
		if (readByte < 0x80 || readByte >= 0xa0)
			*localDest++ = readByte;
		else
			*localDest++ = spCP1252ToUnicode[readByte - 0x80];
		}

	if (oSourceBytes)
		*oSourceBytes = localSource - static_cast<const uint8*>(iSource);
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = 0;
	if (oDestCU)
		*oDestCU = localDest - oDest;
	return true;
	}

// =================================================================================================
// MARK: - ZTextEncoder_CP1252

static const uint8 spCP1252_Page01[72] =
	{
	0x00, 0x00, 0x8c, 0x9c, 0x00, 0x00, 0x00, 0x00, /* 0x50-0x57 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x58-0x5f */
	0x8a, 0x9a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x60-0x67 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x68-0x6f */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x70-0x77 */
	0x9f, 0x00, 0x00, 0x00, 0x00, 0x8e, 0x9e, 0x00, /* 0x78-0x7f */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x80-0x87 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x88-0x8f */
	0x00, 0x00, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x90-0x97 */
	};
static const uint8 spCP1252_Page02[32] =
	{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, /* 0xc0-0xc7 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xc8-0xcf */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xd0-0xd7 */
	0x00, 0x00, 0x00, 0x00, 0x98, 0x00, 0x00, 0x00, /* 0xd8-0xdf */
	};
static const uint8 spCP1252_Page20[48] =
	{
	0x00, 0x00, 0x00, 0x96, 0x97, 0x00, 0x00, 0x00, /* 0x10-0x17 */
	0x91, 0x92, 0x82, 0x00, 0x93, 0x94, 0x84, 0x00, /* 0x18-0x1f */
	0x86, 0x87, 0x95, 0x00, 0x00, 0x00, 0x85, 0x00, /* 0x20-0x27 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x28-0x2f */
	0x89, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x30-0x37 */
	0x00, 0x8b, 0x9b, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x38-0x3f */
	};

void ZTextEncoder_CP1252::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	const UTF32* localSource = iSource;
	const UTF32* localSourceEnd = iSource + iSourceCU;
	uint8* localDest = static_cast<uint8*>(oDest);
	uint8* localDestEnd = localDest + iDestBytes;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		{
		uint32 theCP = *localSource++;
		if (theCP < 0x80) *localDest++ = theCP;
		else if (theCP >= 0x00a0 && theCP < 0x0100) *localDest++ = theCP;
		else if (theCP >= 0x0150 && theCP < 0x0198) *localDest++ = spCP1252_Page01[theCP - 0x0150];
		else if (theCP >= 0x02c0 && theCP < 0x02e0) *localDest++ = spCP1252_Page02[theCP - 0x02c0];
		else if (theCP >= 0x2010 && theCP < 0x2040) *localDest++ = spCP1252_Page20[theCP - 0x2010];
		else if (theCP == 0x20ac) *localDest++ = 0x80;
		else if (theCP == 0x2122) *localDest++ = 0x99;
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestBytes)
		*oDestBytes = localDest - static_cast<uint8*>(oDest);
	}

// =================================================================================================
// MARK: - ZTextDecoder_CP850

static const UTF32 spCP850ToUnicode[128] =
	{
	/* 0x80 */
	0x20ac, 0xfffd, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021,
	0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0xfffd, 0x017d, 0xfffd,
	/* 0x90 */
	0xfffd, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
	0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0xfffd, 0x017e, 0x0178,
	};

bool ZTextDecoder_CP850::Decode(
	const void* iSource, size_t iSourceBytes, size_t* oSourceBytes, size_t* oSourceBytesSkipped,
	UTF32* oDest, size_t iDestCU, size_t* oDestCU)
	{
	const uint8* localSource = static_cast<const uint8*>(iSource);
	const uint8* localSourceEnd = localSource + iSourceBytes;
	UTF32* localDest = oDest;
	UTF32* localDestEnd = oDest + iDestCU;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		{
		uint8 readByte = *localSource++;
		if (readByte < 0x80 || readByte >= 0xa0)
			*localDest++ = readByte;
		else
			*localDest++ = spCP850ToUnicode[readByte - 0x80];
		}

	if (oSourceBytes)
		*oSourceBytes = localSource - static_cast<const uint8*>(iSource);
	if (oSourceBytesSkipped)
		*oSourceBytesSkipped = 0;
	if (oDestCU)
		*oDestCU = localDest - oDest;
	return true;
	}

// =================================================================================================
// MARK: - ZTextEncoder_CP850

static const unsigned char spCP850_Page00[96] =
	{
	0xff, 0xad, 0xbd, 0x9c, 0xcf, 0xbe, 0xdd, 0xf5, /* 0xa0-0xa7 */
	0xf9, 0xb8, 0xa6, 0xae, 0xaa, 0xf0, 0xa9, 0xee, /* 0xa8-0xaf */
	0xf8, 0xf1, 0xfd, 0xfc, 0xef, 0xe6, 0xf4, 0xfa, /* 0xb0-0xb7 */
	0xf7, 0xfb, 0xa7, 0xaf, 0xac, 0xab, 0xf3, 0xa8, /* 0xb8-0xbf */
	0xb7, 0xb5, 0xb6, 0xc7, 0x8e, 0x8f, 0x92, 0x80, /* 0xc0-0xc7 */
	0xd4, 0x90, 0xd2, 0xd3, 0xde, 0xd6, 0xd7, 0xd8, /* 0xc8-0xcf */
	0xd1, 0xa5, 0xe3, 0xe0, 0xe2, 0xe5, 0x99, 0x9e, /* 0xd0-0xd7 */
	0x9d, 0xeb, 0xe9, 0xea, 0x9a, 0xed, 0xe8, 0xe1, /* 0xd8-0xdf */
	0x85, 0xa0, 0x83, 0xc6, 0x84, 0x86, 0x91, 0x87, /* 0xe0-0xe7 */
	0x8a, 0x82, 0x88, 0x89, 0x8d, 0xa1, 0x8c, 0x8b, /* 0xe8-0xef */
	0xd0, 0xa4, 0x95, 0xa2, 0x93, 0xe4, 0x94, 0xf6, /* 0xf0-0xf7 */
	0x9b, 0x97, 0xa3, 0x96, 0x81, 0xec, 0xe7, 0x98, /* 0xf8-0xff */
	};

static const unsigned char spCP850_Page25[168] =
	{
	0xc4, 0x00, 0xb3, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x00-0x07 */
	0x00, 0x00, 0x00, 0x00, 0xda, 0x00, 0x00, 0x00, /* 0x08-0x0f */
	0xbf, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, /* 0x10-0x17 */
	0xd9, 0x00, 0x00, 0x00, 0xc3, 0x00, 0x00, 0x00, /* 0x18-0x1f */
	0x00, 0x00, 0x00, 0x00, 0xb4, 0x00, 0x00, 0x00, /* 0x20-0x27 */
	0x00, 0x00, 0x00, 0x00, 0xc2, 0x00, 0x00, 0x00, /* 0x28-0x2f */
	0x00, 0x00, 0x00, 0x00, 0xc1, 0x00, 0x00, 0x00, /* 0x30-0x37 */
	0x00, 0x00, 0x00, 0x00, 0xc5, 0x00, 0x00, 0x00, /* 0x38-0x3f */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x40-0x47 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x48-0x4f */
	0xcd, 0xba, 0x00, 0x00, 0xc9, 0x00, 0x00, 0xbb, /* 0x50-0x57 */
	0x00, 0x00, 0xc8, 0x00, 0x00, 0xbc, 0x00, 0x00, /* 0x58-0x5f */
	0xcc, 0x00, 0x00, 0xb9, 0x00, 0x00, 0xcb, 0x00, /* 0x60-0x67 */
	0x00, 0xca, 0x00, 0x00, 0xce, 0x00, 0x00, 0x00, /* 0x68-0x6f */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x70-0x77 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x78-0x7f */
	0xdf, 0x00, 0x00, 0x00, 0xdc, 0x00, 0x00, 0x00, /* 0x80-0x87 */
	0xdb, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x88-0x8f */
	0x00, 0xb0, 0xb1, 0xb2, 0x00, 0x00, 0x00, 0x00, /* 0x90-0x97 */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0x98-0x9f */
	0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 0xa0-0xa7 */
	};

void ZTextEncoder_CP850::Encode(const UTF32* iSource, size_t iSourceCU, size_t* oSourceCU,
	void* oDest, size_t iDestBytes, size_t* oDestBytes)
	{
	const UTF32* localSource = iSource;
	const UTF32* localSourceEnd = iSource + iSourceCU;
	uint8* localDest = static_cast<uint8*>(oDest);
	uint8* localDestEnd = localDest + iDestBytes;

	while (localSource < localSourceEnd && localDest < localDestEnd)
		{
		uint32 theCP = *localSource++;
		if (theCP < 0x80) *localDest++ = theCP;
		else if (theCP >= 0x00a0 && theCP < 0x0100) *localDest++ = spCP850_Page00[theCP-0x00a0];
		else if (theCP == 0x0131) *localDest++ = 0xd5;
		else if (theCP == 0x0192) *localDest++ = 0x9f;
		else if (theCP == 0x2017) *localDest++ = 0xf2;
		else if (theCP >= 0x2500 && theCP < 0x25a8) *localDest++ = spCP850_Page25[theCP-0x2500];
		}

	if (oSourceCU)
		*oSourceCU = localSource - iSource;
	if (oDestBytes)
		*oDestBytes = localDest - static_cast<uint8*>(oDest);
	}

} // namespace ZooLib