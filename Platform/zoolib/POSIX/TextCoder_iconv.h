/* -------------------------------------------------------------------------------------------------
Copyright (c) 2003 Andrew Green and Learning in Motion, Inc.
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

#ifndef __ZooLib_POSIX_TextCoder_iconv_h__
#define __ZooLib_POSIX_TextCoder_iconv_h__ 1
#include "zconfig.h"
#include "zoolib/ZCONFIG_API.h"
#include "zoolib/ZCONFIG_SPI.h"

#ifndef ZCONFIG_API_Avail__TextCoder_iconv
// We can't simply do this:
// #define ZCONFIG_API_Avail__TextCoder_iconv ZCONFIG_SPI_Enabled(iconv)
// because <iconv.h> may #define 'iconv' as 'libiconv', and any use of
// ZCONFIG_SPI_Enabled(iconv) subsequent to the include of iconv.h
// becomes ZCONFIG_SPI_Enabled(libiconv) which of course evaluates to false.
	#if ZCONFIG_SPI_Enabled(iconv)
		#define ZCONFIG_API_Avail__TextCoder_iconv 1
	#else
		#define ZCONFIG_API_Avail__TextCoder_iconv 0
	#endif
#endif

#ifndef ZCONFIG_API_Desired__TextCoder_iconv
	#define ZCONFIG_API_Desired__TextCoder_iconv 1
#endif

#include "zoolib/TextCoder.h"

#if ZCONFIG_API_Enabled(TextCoder_iconv)

namespace ZooLib {

ZP<TextDecoder> sMake_TextDecoder_iconv(const std::string& iSourceName);

ZP<TextEncoder> sMake_TextEncoder_iconv(const std::string& iDestName);

} // namespace ZooLib

#endif // ZCONFIG_API_Enabled(TextCoder_iconv)

#endif // __ZooLib_POSIX_TextCoder_iconv_h__
