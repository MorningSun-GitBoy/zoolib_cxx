// Copyright (c) 2019 Andrew Green. MIT License. http://www.zoolib.org

#include "zoolib/Zip/File_Zip.h"

#include "zoolib/Util_STL_map.h"

#include "zip.h"

namespace ZooLib {

using namespace Util_STL;
using std::map;
using std::string;
using std::vector;

// =================================================================================================
#pragma mark - Node (anonymous)

namespace { // anonymous

class Node_Directory;

class Node
:	public CountedWithoutFinalize
	{
public:
	Node(Node_Directory* iParent, const string& iName)
	:	fParent(iParent)
	,	fName(iName)
		{}

	Node_Directory* fParent;
	const string fName;
	};

typedef map<string8, ZP<Node> > MapNameNode;

class Node_Directory
:	public Node
	{
public:
	Node_Directory(Node_Directory* iParent, const string& iName)
	:	Node(iParent, iName)
		{}

	MapNameNode fChildren;
	};

class Node_File
:	public Node
	{
public:
	Node_File(Node_Directory* iParent, const string& iName, size_t iEntryNum)
	:	Node(iParent, iName)
	,	fEntryNum(iEntryNum)
		{}

	const size_t fEntryNum;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - ZipHolder (anonymous)

namespace { // anonymous

struct ZipHolder
:	public Counted
	{
	static
	void spStuff(size_t iEntryNum, const ZP<Node_Directory>& ioParent,
		const Trail& iTrail, size_t iIndex)
		{
		const string theName = iTrail.At(iIndex);

		if (iIndex == iTrail.Count() - 1)
			{
			// We've hit the file
			ioParent->fChildren[theName] = new Node_File(ioParent.Get(), theName, iEntryNum);
			return;
			}

		ZP<Node_Directory> theNode = ioParent->fChildren[theName].StaticCast<Node_Directory>();
		if (not theNode)
			{
			theNode = new Node_Directory(ioParent.Get(), theName);
			ioParent->fChildren[theName] = theNode;
			}

		spStuff(iEntryNum, theNode, iTrail, iIndex + 1);
		}

	ZipHolder(zip* i_zip, bool iAdopt)
	:	f_zip(i_zip)
	,	fOwned(iAdopt)
	,	fRoot(new Node_Directory(nullptr, string()))
		{
		if (not f_zip)
			throw std::runtime_error("Couldn't open zip");

		for (size_t xx = 0, count = ::zip_get_num_files(f_zip); xx < count; ++xx)
			{
			const string theName = ::zip_get_name(f_zip, xx, 0);
			spStuff(xx, fRoot, Trail(theName), 0);
			}
		}

	virtual ~ZipHolder()
		{
		if (fOwned)
			::zip_close(f_zip);
		}

	struct zip* f_zip;
	bool fOwned;

	ZP<Node_Directory> fRoot;
	};

} // anonymous namespace

// =================================================================================================
#pragma mark - ChannerR_Bin_Zip

struct ChannerR_Bin_Zip
:	public ChannerR_Bin
	{
	ZP<ZipHolder> fZipHolder;
	zip_file* f_zip_file;

	ChannerR_Bin_Zip(const ZP<ZipHolder>& iZipHolder, size_t iEntryNum)
	:	fZipHolder(iZipHolder)
	,	f_zip_file(::zip_fopen_index(fZipHolder->f_zip, iEntryNum, 0))
		{}

	virtual ~ChannerR_Bin_Zip()
		{ ::zip_fclose(f_zip_file); }

// From ChanR
	virtual size_t Read(byte* oDest, size_t iCount)
		{
		const ssize_t countRead = ::zip_fread(f_zip_file, oDest, iCount);
		return countRead <= 0 ? 0 : countRead;
		}
	};

// =================================================================================================
#pragma mark - FileIterRep_Zip

class FileIterRep_Zip
:	public FileIterRep
	{
public:
	const ZP<ZipHolder> fZipHolder;
	MapNameNode::const_iterator fIter;
	const MapNameNode::const_iterator fEnd;

	FileIterRep_Zip(const ZP<ZipHolder>& iZipHolder,
		MapNameNode::const_iterator iIter,
		MapNameNode::const_iterator iEnd)
	:	fZipHolder(iZipHolder)
	,	fIter(iIter)
	,	fEnd(iEnd)
		{}

	virtual ~FileIterRep_Zip()
		{}

	virtual bool HasValue()
		{ return fIter != fEnd; }

	virtual void Advance()
		{
		if (fIter != fEnd)
			++fIter;
		}

	virtual FileSpec Current();

	virtual std::string CurrentName() const
		{ return fIter->first; }

	virtual ZP<FileIterRep> Clone()
		{ return new FileIterRep_Zip(fZipHolder, fIter, fEnd); }
	};

// =================================================================================================
#pragma mark - FileLoc_Zip

class FileLoc_Zip : public FileLoc
	{
public:
	FileLoc_Zip(const ZP<ZipHolder>& iZipHolder, const ZP<Node>& iNode)
	:	fZipHolder(iZipHolder)
	,	fNode(iNode)
		{}

	virtual ~FileLoc_Zip()
		{}

// From FileLoc
	virtual ZP<FileIterRep> CreateIterRep()
		{
		if (ZP<Node_Directory> theDir = fNode.DynamicCast<Node_Directory>())
			{
			return new FileIterRep_Zip(fZipHolder,
				theDir->fChildren.begin(), theDir->fChildren.end());
			}
		return null;
		}

	virtual std::string GetName() const
		{
		if (fNode)
			return fNode->fName;
		return string();
		}

	virtual ZQ<Trail> TrailTo(ZP<FileLoc> oDest) const
		{ return null; }

	virtual ZP<FileLoc> GetParent()
		{
		if (fNode && fNode->fParent)
			return new FileLoc_Zip(fZipHolder, fNode->fParent);
		return null;
		}

	virtual ZP<FileLoc> GetDescendant(const std::string* iComps, size_t iCount)
		{
		ZP<Node> theNode = fNode;
		for (;;)
			{
			if (not iCount--)
				{
				if (theNode)
					return new FileLoc_Zip(fZipHolder, theNode);
				return null;
				}

			if (ZP<Node_Directory> theDir = theNode.DynamicCast<Node_Directory>())
				theNode = sGet(theDir->fChildren, *iComps++);
			else
				return null;
			}
		}

	virtual bool IsRoot()
		{ return not fNode->fParent; }

	virtual ZP<FileLoc> Follow()
		{ return this; }

	virtual std::string AsString_POSIX(const std::string* iComps, size_t iCount)
		{
		string result = this->pGetPath();
		for (size_t xx = 0; xx < iCount; ++xx)
			{
			result += "/";
			result += iComps[xx];
			}
		return result;
		}

	virtual std::string AsString_Native(const std::string* iComps, size_t iCount)
		{ return this->AsString_POSIX(iComps, iCount); }

	virtual File::Kind Kind()
		{
		if (not fNode)
			return File::kindNone;

		if (fNode.DynamicCast<Node_Directory>())
			return File::kindDir;

		return File::kindFile;
		}

	virtual uint64 Size()
		{
		ZUnimplemented();
		}

	virtual double TimeCreated()
		{ return 0; }

	virtual double TimeModified()
		{ return 0; }

	virtual ZP<FileLoc> CreateDir()
		{ return null; }

	virtual ZP<FileLoc> MoveTo(ZP<FileLoc> oDest)
		{ return null; }

	virtual bool Delete()
		{ return false; }

	virtual ZP<ChannerR_Bin> OpenR(bool iPreventWriters)
		{
		if (ZP<Node_File> theNode = fNode.DynamicCast<Node_File>())
			return new ChannerR_Bin_Zip(fZipHolder, theNode->fEntryNum);
		return null;
		}

	std::string pGetPath()
		{
		string result;
		for (ZP<Node> theNode = fNode; theNode; theNode = theNode->fParent)
			result = "/" + theNode->fName + result;
		return result;
		}

private:
	ZP<ZipHolder> fZipHolder;
	ZP<Node> fNode;
	};

FileSpec FileIterRep_Zip::Current()
	{
	if (fIter != fEnd)
		return new FileLoc_Zip(fZipHolder, fIter->second);

	return FileSpec();
	}

// =================================================================================================
#pragma mark - sFileLoc_Zip

FileSpec sFileSpec_Zip(const std::string& iZipFilePath)
	{
	if (zip* the_zip = ::zip_open(iZipFilePath.c_str(), 0, nullptr))
		{
		ZP<ZipHolder> theZipHolder = new ZipHolder(the_zip, true);
		return new FileLoc_Zip(theZipHolder, theZipHolder->fRoot);
		}
	return FileSpec();
	}

} // namespace ZooLib
