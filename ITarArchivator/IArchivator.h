#pragma once
#include <vector>
#include <string>
using namespace std;

class IArchivator {
public :
	virtual const string &getArchiveName() = 0;
	virtual vector<string> GetList() = 0;
	virtual int AddFile(const string &filename)= 0;
	virtual void RemoveFile(const string &filename)= 0;
	virtual void ExtractFile(const string &directory, const string &filename)= 0;
	virtual int AddFolder(const string &foldername) = 0;
	virtual void RemoveFolder(const string &foldername) = 0;
	virtual void ExtractFolder(const string &directory, const string &foldername) = 0;
	virtual void Clear()= 0; 
};