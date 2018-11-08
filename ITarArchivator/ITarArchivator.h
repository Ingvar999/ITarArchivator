#pragma once
#include "pch.h"
#include "IArchivator.h"
#include "IFileSystem.h"

using namespace std;

struct Header {
	static const byte file_md = 1;
	static const byte folder_md = 2;

	char name[100];
	uint16_t nextOffset;
	uint32_t size;
	bool isValid;
	byte mode;
};

class ITarArchivator : public IArchivator{

public:
	ITarArchivator(const string &filename);
	~ITarArchivator();
	const string &getArchiveName() override;
	vector<string> GetList() override;
	int AddFile(const string &filename) override;
	void RemoveItem(const string &itemname) override;
	void ExtractFile(const string &directory, const string &filename) override;
	int AddFolder(const string &foldername) override;
	void ExtractFolder(const string &directory, const string &foldername) override;
	void Clear() override;

private:
	static const int blockSize = 512;

	int currentBlocksCount;
	string archivename;
	fstream *archive;
	char buffer[blockSize];
	Header *hed;
	IFileSystem *filesystem;

	void Compact();
	void ReadFolder(vector<string> &result, int currentBlock, const string &path);
	int SearchItem(string itemname);
	int SearchItemInFolder(string itemname, int currentBlock);
};

