#pragma once
#include "IFileSystem.h"

class SimpleFileSystem :
	public IFileSystem
{
public:
	SimpleFileSystem();
	~SimpleFileSystem();
	vector<string> GetContentList(const string &path, bool files, bool directories) override;
	int ResizeFile(const string& filename, int size) override;
	string GetShortName(const string& filename) override;
};

