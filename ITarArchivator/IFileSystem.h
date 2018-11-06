#pragma once
#include <string>
#include <vector>
using namespace std;

class IFileSystem {
public:
	virtual vector<string> GetContentList(const string &path, bool files, bool directories) = 0;
	virtual int ResizeFile(const string& filename, int size) = 0;
};