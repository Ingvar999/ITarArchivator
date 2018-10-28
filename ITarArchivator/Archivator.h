#pragma once
#include "pch.h"

using namespace std;

struct Header {
	char name[100];
	uint16_t nextOffset;
	uint32_t size;
	bool isValid;
};

class Archivator{

public:
	Archivator(const string &filename);
	~Archivator();
	const string &getName();
	void AddFile(const string &filename);
	vector<string> GetList();
	void RemoveFile(const string &filename);
	void ExtractFile(const string &directory, const string &filename);
	void Clear();

private:
	static const int blockSize = 512;

	int currentBlocksCount;
	string archivename;
	fstream *archive;
	char buffer[blockSize];

	void Compact();
	int ResizeFile(const string& filename, int size);
};

