
#include "pch.h"
#include "Archivator.h"

using namespace std;

typedef void(*Handler)(const vector<string> &parts, string& path, Archivator* &archivator);

vector<string> GetContentList(const string &path, bool files, bool directories);
void HelpHandler(const vector<string>& parts, string& path, Archivator* &archivator);
void DirHandler(const vector<string>& parts, string& path, Archivator* &archivator);
void CdHandler(const vector<string>& parts, string& path, Archivator* &archivator);
void OpenHandler(const vector<string>& parts, string& path, Archivator* &archivator);
void CloseHandler(const vector<string>& parts, string& path, Archivator* &archivator);
void AddHandler(const vector<string>& parts, string& path, Archivator* &archivator);
void ListHandler(const vector<string>& parts, string& path, Archivator* &archivator);
void RemoveHandler(const vector<string>& parts, string& path, Archivator* &archivator);
void ExtractHandler(const vector<string>& parts, string& path, Archivator* &archivator);


string helpMessage(
"dir\n\
cd <folder_name>\n\
open <archive_name>\n\
close\n\
add <file_name>\n\
list\n\
remove <file_name>\n\
extract");

int main()
{
	setlocale(LC_ALL, "rus");
	map<string, Handler> handlers = {
	{"dir", DirHandler},
	{"cd", CdHandler},
	{"help", HelpHandler},
	{"open", OpenHandler},
	{"close", CloseHandler},
	{"add", AddHandler},
	{"list", ListHandler},
	{"remove", RemoveHandler},
	{"extract", ExtractHandler}
	};
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

		map<string, Handler>::iterator handler;
		if (parts.size() > 0) {
			if ((handler = handlers.find(parts[0])) != handlers.end()) {
				(handler->second)(parts, currentPath, archivator);
			}
			else
				cout << "Command \"" << parts[0] << "\" does not exist" << endl;
		}
	}
}

void HelpHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	cout << helpMessage << endl;
}

void DirHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	vector<string> files = GetContentList(path, true, true);
	for (int i = 0; i < files.size(); ++i)
		cout << files[i] << endl;
}

void CdHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	if (parts.size() > 1) {
		if (parts[1] != ".") {
			if (parts[1] == "..") {
				string::reverse_iterator pos;
				if ((pos = find(++path.rbegin(), path.rend(), '\\')) != path.rend())
					path = string(path.begin(), pos.base());
			}
			else {
				vector<string> directories = GetContentList(path, false, true);
				string dir = parts[1];
				for (int i = 2; i < parts.size(); ++i)
					dir += " " + parts[i];
				if (find(directories.begin(), directories.end(), dir) != directories.end())
					path += dir + "\\";
				else
					cout << "This folder was not found in current directory" << endl;
			}
		}
	}
	else
		cout << "You should give folder name as argument" << endl;
}

void OpenHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	if (parts.size() > 1) {
		string filename = parts[1];
		for (int i = 2; i < parts.size(); ++i)
			filename += " " + parts[i];
		if (filename.substr(filename.find_last_of('.')) == ".itar") {
			if (archivator != 0)
				delete archivator;
			archivator = new Archivator(path + filename);
		}
		else
			cout << "File must have .itar extension" << endl;
	}
	else
		cout << "You should give archive name as argument" << endl;
}

void AddHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	if (archivator)
		if (parts.size() > 1) {
			vector<string> files = GetContentList(path, true, false);
			string filename = parts[1];
			for (int i = 2; i < parts.size(); ++i)
				filename += " " + parts[i];
			if (find(files.begin(), files.end(), filename) != files.end()) {
				vector<string> list = archivator->GetList();
				if (find(list.begin(), list.end(), filename) == list.end())
					archivator->AddFile(path + filename);
				else
					cout << "There is a file with the same name" << endl;
			}
			else
				cout << "This file was not found in current directory" << endl;
		}
		else
			cout << "You should give file name as argument" << endl;
	else
		cout << "First you should open an archive" << endl;
}

void ListHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	if (archivator) {
		vector<string> filesList = archivator->GetList();
		for (int i = 0; i < filesList.size(); ++i)
			cout << filesList[i] << endl;
	}
	else
		cout << "First you should open an archive" << endl;
}

void CloseHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	if (archivator) {
		delete archivator;
		archivator = 0;
	}
}

void RemoveHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	if (archivator) {
		string filename = parts[1];
		for (int i = 2; i < parts.size(); ++i)
			filename += " " + parts[i];
		archivator->RemoveFile(filename);
	}
	else
		cout << "First you should open an archive" << endl;
}

void ExtractHandler(const vector<string>& parts, string& path, Archivator* &archivator) {
	if (archivator) {
		string filename = parts[1];
		for (int i = 2; i < parts.size(); ++i)
			filename += " " + parts[i];
		vector<string> files = archivator->GetList();
		if (find(files.begin(), files.end(), filename) != files.end())
			archivator->ExtractFile(path, filename);
		else
			cout << "There is not such file" << endl;
	}
	else
		cout << "First you should open an archive" << endl;
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