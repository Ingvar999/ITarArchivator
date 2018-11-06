#pragma once
#include "pch.h"
#include "IArchivator.h"
#include "IFileSystem.h"

using namespace std;

struct Header {
	char name[100];
	uint16_t nextOffset;
	uint32_t size;
	bool isValid;
};

class ITarArchivator : public IArchivator{

public:
	ITarArchivator(const string &filename);
	~ITarArchivator();
	const string &getArchiveName() override;
	vector<string> GetList() override;
	void AddFile(const string &filename) override;
	void RemoveFile(const string &filename) override;
	void ExtractFile(const string &directory, const string &filename) override;
	void AddFolder(const string &foldername) override;
	void RemoveFolder(const string &foldername) override;
	void ExtractFolder(const string &directory, const string &foldername) override;
	void Clear() override;

private:
	static const int blockSize = 512;

	int currentBlocksCount;
	string archivename;
	fstream *archive;
	char buffer[blockSize];
	IFileSystem *filesystem;

	void Compact();
};

