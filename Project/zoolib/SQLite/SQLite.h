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

#ifndef __ZooLib_SQLite_SQLite_h__
#define __ZooLib_SQLite_SQLite_h__ 1
#include "zconfig.h"

#include "zoolib/Any.h"
#include "zoolib/Counted.h"
#include "zoolib/UnicodeString.h"

#include <sqlite3.h>

namespace ZooLib {
namespace SQLite {

// =================================================================================================
#pragma mark - SQLite

class DB : public Counted
	{
public:
	DB(const string8& iPath);
	DB(sqlite3* iDB, bool iAdopt);

	virtual ~DB();

	sqlite3* GetDB();

private:
	sqlite3* fDB;
	bool fAdopted;
	};

// =================================================================================================
#pragma mark - Iter

class Iter : public Counted
	{
	Iter(ZP<DB> iDB, const string8& iSQL, uint64 iPosition);

public:
	Iter(ZP<DB> iDB, const string8& iSQL);
	virtual ~Iter();

	ZP<Iter> Clone(bool iRewound);
	void Rewind();
	bool HasValue();
	void Advance();

	size_t Count();
	string8 NameOf(size_t iIndex);
	Any Get(size_t iIndex);

private:
	void pAdvance();

	ZP<DB> fDB;
	const string8 fSQL;
	sqlite3_stmt* fStmt;
	bool fHasValue;
	uint64 fPosition;
	};

} // namespace SQLite
} // namespace ZooLib

#endif // __ZooLib_SQLite_SQLite_h__
