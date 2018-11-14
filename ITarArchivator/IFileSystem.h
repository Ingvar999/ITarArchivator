#pragma once
#include <string>
#include <vector>
#include <string>
using namespace std;

class IFileSystem {
public:
	virtual vector<string> GetContentList(const string &path, bool files, bool directories) = 0;
	virtual int ResizeFile(const string& filename, int size) = 0;
	virtual string GetShortName(const string& filename) = 0;
};