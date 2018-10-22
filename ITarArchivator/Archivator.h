#pragma once
#include "pch.h"

using namespace std;

struct Header {
	char name[100];
	uint16_t nextOffset;
	uint32_t size;
};

class Archivator{

public:
	Archivator(const string &filename);
	~Archivator();
	const string &getName();
	void AddFile(const string &filename);
	vector<string> GetList();
	void RemoveFile(const string &filename);
	void Exctract(const string &directory);

private:
	string filename;
	ofstream *archiv;
};

