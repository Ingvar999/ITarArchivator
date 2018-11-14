#include "pch.h"
#include "SimpleFileSystem.h"


SimpleFileSystem::SimpleFileSystem()
{
}


SimpleFileSystem::~SimpleFileSystem()
{
}

vector<string> SimpleFileSystem::GetContentList(const string &path, bool files, bool directories) {
	string dir(path + "*");
	vector<string> result;
	WIN32_FIND_DATAA fi;
	ZeroMemory(&fi, sizeof(fi));
	HANDLE h = FindFirstFileA(dir.c_str(), &fi);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			if (!(fi.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)) {
				if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (directories)
						result.push_back(fi.cFileName);
				}
				else
					if (files)
						result.push_back(fi.cFileName);
			}
		} while (FindNextFileA(h, &fi));
		FindClose(h);
	}
	return result;
}


int SimpleFileSystem::ResizeFile(const string& filename, int size) {
	HANDLE hFile = CreateFileA(filename.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return 1;
	SetFilePointer(hFile, size, 0, FILE_BEGIN);
	SetEndOfFile(hFile);
	CloseHandle(hFile);
	return 0;
}

string SimpleFileSystem::GetShortName(const string& filename) {
	return filename.substr(filename.find_last_of('\\') + 1);
}
