
#include "pch.h"
#include "Archivator.h"

using namespace std;

vector<string> Dir(const string &path, bool files, bool directories);

int main()
{
	setlocale(LC_ALL, "rus");
	Archivator *archivator = 0;
	string currentPath("D:\\");
	string command;
	string item;
	while (1) {
		cout << currentPath << ">";
		getline(cin, command);
		stringstream ss(command);
		vector<string> parts;
		while (getline(ss, item, ' '))
			parts.push_back(item);
		if (parts.size() > 0)
			if (parts[0] == "dir") {
				vector<string> files = Dir(currentPath, true, true);
				for (int i = 0; i < files.size(); ++i)
					cout << files[i] << endl;
			}
			else if (parts[0] == "cd") {
				if (parts.size() > 1) {
					if (parts[1] == "..") {
						string::reverse_iterator pos;
						if ((pos = find(++currentPath.rbegin(), currentPath.rend(), '\\')) != currentPath.rend()) 
							currentPath = string(currentPath.begin(), pos.base());
					}
					else {
						vector<string> directories = Dir(currentPath, false, true);
						string dir = parts[1];
						for (int i = 2; i < parts.size(); ++i)
							dir += " " + parts[i];
						if (find(directories.begin(), directories.end(), dir) != directories.end())
							currentPath += dir + "\\";
					}
				}
			}
			else if (parts[0] == "choose") {
				if (parts.size() > 1) {
					string filename = parts[1];
					for (int i = 2; i < parts.size(); ++i)
						filename += " " + parts[i];
					if (archivator != 0)
						delete archivator;
					archivator = new Archivator(currentPath + filename);
				}
			}
	}
}

vector<string> Dir(const string &path, bool files, bool directories) {
	string dir(path + "*");
	vector<string> result;
	WIN32_FIND_DATAA fi;
	ZeroMemory(&fi, sizeof(fi));
	HANDLE h = FindFirstFileA(dir.c_str(), &fi);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				if (directories)
					result.push_back(fi.cFileName);
			}
			else
				if(files)
					result.push_back(fi.cFileName);
		} while (FindNextFileA(h, &fi));
		FindClose(h);
	}
	return result;
}