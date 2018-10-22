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

private:
	string filename;
	fstream *archiv;
};

