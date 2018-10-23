
#include "pch.h"
#include "Archivator.h"

using namespace std;

vector<string> GetContentList(const string &path, bool files, bool directories);

int main()
{
	setlocale(LC_ALL, "rus");
	Archivator *archivator = 0;
	string currentPath("D:\\git\\ITarArchivator\\Example\\");
	string command;
	string item;
	while (1) {
		if (archivator)
			cout << '[' << archivator->getName() << "] ";
		cout << currentPath << ">";
		getline(cin, command);
		stringstream ss(command);
		vector<string> parts;
		while (getline(ss, item, ' '))
			parts.push_back(item);

		if (parts.size() > 0)
			if (parts[0] == "dir") {
				vector<string> files = GetContentList(currentPath, true, true);
				for (int i = 0; i < files.size(); ++i)
					cout << files[i] << endl;
			}
			else if (parts[0] == "cd") {
				if (parts.size() > 1 && parts[1] != ".") {
					if (parts[1] == "..") {
						string::reverse_iterator pos;
						if ((pos = find(++currentPath.rbegin(), currentPath.rend(), '\\')) != currentPath.rend())
							currentPath = string(currentPath.begin(), pos.base());
					}
					else {
						vector<string> directories = GetContentList(currentPath, false, true);
						string dir = parts[1];
						for (int i = 2; i < parts.size(); ++i)
							dir += " " + parts[i];
						if (find(directories.begin(), directories.end(), dir) != directories.end())
							currentPath += dir + "\\";
					}
				}
			}
			else if (parts[0] == "open") {
				if (parts.size() > 1) {
					string filename = parts[1];
					for (int i = 2; i < parts.size(); ++i)
						filename += " " + parts[i];
					if (filename.substr(filename.find_last_of('.')) == ".itar") {
						if (archivator != 0)
							delete archivator;
						archivator = new Archivator(currentPath + filename);
					}
				}
			}
			else if (parts[0] == "close") {
				delete archivator;
				archivator = 0;
			}
			else if (parts[0] == "add") {
				if (parts.size() > 1) {
					vector<string> files = GetContentList(currentPath, true, false);
					string filename = parts[1];
					for (int i = 2; i < parts.size(); ++i)
						filename += " " + parts[i];
					if (find(files.begin(), files.end(), filename) != files.end() && archivator)
						archivator->AddFile(currentPath + filename);
				}
			}
			else if (parts[0] == "list") {
				if (archivator) {
					vector<string> filesList = archivator->GetList();
					for (int i = 0; i < filesList.size(); ++i)
						cout << filesList[i] << endl;
				}
			}
			else if (parts[0] == "remove") {

			}
			else if (parts[0] == "extract") {

			}
	}
}

vector<string> GetContentList(const string &path, bool files, bool directories) {
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