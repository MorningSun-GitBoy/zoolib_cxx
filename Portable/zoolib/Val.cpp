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

#include "zoolib/Val.h"

#include "zoolib/Util_string.h"

namespace ZooLib {

/**
\defgroup Val Variant Types

Variant types show up all over the place, in COM, CFTypes, interfacing with Javascript and other
languages etc. The APIs for each differ wildly even though they're conceptually very similar. To
make life easier ZooLib provides several type suites that put a consistent API atop variants and
related types so the the terminology, constraints and facilities are homogenous.

The three essential types provided in each suite are Val, Seq and Map. A Val is what's often called
a variant, a 'disjoint union' or a 'tagged union'. It has
\link ValueSemantics value semantics\endlink,
holding a single value of some type at any point in time. Assigning a new value of a supported type
replaces the prior value. The type of the stored value and the value itself are both accessible.

Seq is a sequence of Vals -- an array or vector. The number of elements in the Seq is called its
count, each element is accessed using a zero-based index.

Map is an associative map, from name to Val. A name is a std::string, although for particular suites
there may be another form that is native (a fourcc for example).

Seq and Map also maintain
\link ValueSemantics value semantics\endlink.

In each suite the types are named \c Val_XXX, \c Seq_XXX and \c Map_XXX, where \c XXX is an
appropriate distinguishing string. Some of the suites are placed within a namespace YYY, wherein
the types are named \c YYY::Val, \c YYY::Seq and \c YYY:Map.

When working with a Val you may use simple assignment or a templated Set method:
\code
Val theVal = 27;
theVal.Set<long>(19);
\endcode
The latter makes the stored type explicit. The actual stored type is usually important, because
oftentimes you can only retrieve the value if you know what type was used to store it (think
long vs int, or
even unsigned vs signed).

There are several ways to get a value. Which you use depends on what you're doing:
\code
// In the following we assume that something like this:
Val theVal = 27;
// has initialized theVal.

// Example 1:
int theInt1 = theVal.Get<int>();

// Example 2:
int theInt2 = theVal.DGet<int>(19);

// Example 3: (deprecated)
int theInt3;
if (theVal.QGet<int>(theInt3))
	{} // use theInt3

// Example 4:
if (ZQ<int> theQ = theVal.QGet<int>())
	{} // Use theQ.Get();

// Example 5:
if (const int* theIntPtr = theVal.PGet<int>())
	{} // use *theIntptr
\endcode
The first style is the simplest; simply call \c Get<yyy>, and you'll be returned a \c yyy. The
actual value will be either the default value for a \c yyy (e.g. zero for numbers), or it will
be the value that was stored.

The second gives you control over what the default value will be, in this case we're
passing 19, so \c theInt2 will be 19 or whatever int was previously assigned to \c theVal.

In the third example the stored value is placed in \c theInt3 and true retured, or \c theInt3 is
untouched and false returned. The 'Q' stands for Query (cf LISPish function names). This variant is
increasingly being replaced by the following.

Our fourth example returns a \c ZQ<int>. Evaluated in a boolean context a ZQ returns a
true-equivalent if it had a value assigned to it, false otherwise. So we can declare space
for the result \em and check if we got one without polluting the enclosing scope with another name.

The final example is not available in all Val suites. For those where there is an accessible
storage location we can return a pointer to the actual value iff what's stored is of the correct
type. This generally is available with tagged unions, not with more abstracted APIs.

For those APIs with a fixed repertoire of supported types, methods named \c SetXXX, \c GetXXX,
\c DGetXXX and \c QGetXXX are often provided, where \c XXX is the generalized typename
(eg \c Int32, \c Int64, \c Float etc).

The Get/Set API for Seq is based on that for Val, but with an initial index parameter. It
has a \c Count method returning the number of Vals in the Seq, and generally has
\c Erase, \c Insert and \c Append methods.

Map is similar, but here the initial parameter is a name of some sort. Generally a std::string can
be used, which is converted to an underlying type if appropriate. There's usually
also some \c Index_t type, which may be an integer or a more complex type -- the Map will have
\c Begin and \c End methods, which respectively return an \c Index_t referring to the first stored
Val and an \c Index_t that referring to the position just past the end of the map.

Where possible Val, Seq and Map are type-compatible with the appropriate native type. For example,
a ZWinCOM::Variant can be used where a VARIANT would be expected. In other cases the native
API itself does not have an equivalent of Val, so our Val is a subclass of Any, and we
do appropriate marshalling at the Map and Seq level.

There is one more set of ZooLib types that are conceptually part of the Val API. Data_XXX is
equivalent to a CFData or NSData -- it's simply a block of bytes. ZData_CF and ZData_NS wrap
CFData and NSData respectively, and Data_ZZ is signature compatible with them both and is
implemented as a vector<char>.
*/

// =================================================================================================
#pragma mark - sEquali

bool sEquali(const std::string& iLeft, const std::string& iRight)
	{ return Util_string::sEquali(iLeft, iRight); }

} // namespace ZooLib
