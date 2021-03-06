
#include "pch.h"
#include "ITarArchivator.h"
#include "SimpleFileSystem.h"

using namespace std;

typedef void(*Handler)(const vector<string> &parts, string& path, ITarArchivator* &archivator);

bool Endorsement();
void HelpHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void DirHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void CdHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void OpenHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void CloseHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void AddHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void ListHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void RemoveHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void ExtractHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);
void ClearHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator);


const string helpMessage(
"dir\n\
cd <folder_name>\n\
open <archive_name>\n\
close\n\
add <file_name>\n\
list\n\
remove <file_name>\n\
extract <file_name> | all\n\
clear");

IFileSystem *filesystem;

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
	{"extract", ExtractHandler},
	{"clear", ClearHandler}
	};
	filesystem = new SimpleFileSystem();
	ITarArchivator *archivator = 0;
	string currentPath("D:\\git\\ITarArchivator\\Example\\");
	string command;
	string item;
	while (1) {
		if (archivator)
			cout << '[' << archivator->getArchiveName() << "] ";
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

void HelpHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	cout << helpMessage << endl;
}

void DirHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	vector<string> files = filesystem->GetContentList(path, true, true);
	for (int i = 0; i < files.size(); ++i)
		cout << files[i] << endl;
}

void CdHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	if (parts.size() > 1) {
		if (parts[1] != ".") {
			if (parts[1] == "..") {
				string::reverse_iterator pos;
				if ((pos = find(++path.rbegin(), path.rend(), '\\')) != path.rend())
					path = string(path.begin(), pos.base());
			}
			else {
				vector<string> directories = filesystem->GetContentList(path, false, true);
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

void OpenHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	if (parts.size() > 1) {
		string filename = parts[1];
		for (int i = 2; i < parts.size(); ++i)
			filename += " " + parts[i];
		if (filename.substr(filename.find_last_of('.')) == ".itar") {
			if (archivator != 0)
				delete archivator;
			archivator = new ITarArchivator(path + filename);
		}
		else
			cout << "File must have .itar extension" << endl;
	}
	else
		cout << "You should give archive name as argument" << endl;
}

void AddHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	if (archivator)
		if (parts.size() > 1) {
			string sourcename = parts[1];
			for (int i = 2; i < parts.size(); ++i)
				sourcename += " " + parts[i];
			vector<string> files = filesystem->GetContentList(path, true, false);
			vector<string> list = archivator->GetList();
			if (find(files.begin(), files.end(), sourcename) != files.end()) {
				if (find(list.begin(), list.end(), sourcename) == list.end())
					archivator->AddFile(path + sourcename);
				else
					cout << "There is a file with the same name" << endl;
			}
			else {
				vector<string> folders = filesystem->GetContentList(path, false, true);
				if (find(folders.begin(), folders.end(), sourcename) != folders.end()) {
					if (find(list.begin(), list.end(), sourcename) == list.end())
						archivator->AddFolder(path + sourcename + "\\");
					else
						cout << "There is a folder with the same name" << endl;
				}
				else
					cout << "This file or folder was not found in current directory" << endl;
			}
		}
		else
			cout << "You should give file name as argument" << endl;
	else
		cout << "First you should open an archive" << endl;
}

void ListHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	if (archivator) {
		vector<string> filesList = archivator->GetList();
		for (int i = 0; i < filesList.size(); ++i)
			cout << filesList[i] << endl;
	}
	else
		cout << "First you should open an archive" << endl;
}

void CloseHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	if (archivator) {
		delete archivator;
		archivator = 0;
	}
}

void RemoveHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	if (archivator) {
		if (parts.size() > 1) {
			string filename = parts[1];
			for (int i = 2; i < parts.size(); ++i)
				filename += " " + parts[i];
			archivator->RemoveItem(filename);
		}
		else
			cout << "You should give file name as argument" << endl;
	}
	else
		cout << "First you should open an archive" << endl;
}

void ExtractHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	if (archivator) {
		if (parts.size() > 1) {
			vector<string> archiveFiles = archivator->GetList();
			vector<string> folderFiles = filesystem->GetContentList(path, true, false);
			if (parts[1] != "all") {
				string filename = parts[1];
				for (int i = 2; i < parts.size(); ++i)
					filename += " " + parts[i];
				if (find(archiveFiles.begin(), archiveFiles.end(), filename) != archiveFiles.end()) {
					if (find(folderFiles.begin(), folderFiles.end(), filename) != folderFiles.end()) {
						cout << "This file already exists in current folder. Do you want rewrite it? [y/n]" << endl;
						if (Endorsement())
							archivator->ExtractFile(path, filename);
					}
					else
						archivator->ExtractFile(path, filename);
				}
				else
					cout << "There is not such file" << endl;
			}
			else {
				for (int i = 0; i < archiveFiles.size(); ++i) {
					if (find(folderFiles.begin(), folderFiles.end(), archiveFiles[i]) != folderFiles.end()) {
						cout << "File \"" << archiveFiles[i] << "\" already exists in current folder. Do you want rewrite it? [y/n]" << endl;
						if (Endorsement())
							archivator->ExtractFile(path, archiveFiles[i]);
					}
					else
						archivator->ExtractFile(path, archiveFiles[i]);
				}
			}
		}
		else
			cout << "You should give file name as argument" << endl;
	}
	else
		cout << "First you should open an archive" << endl;
}

void ClearHandler(const vector<string>& parts, string& path, ITarArchivator* &archivator) {
	if (archivator) {
		archivator->Clear();
	}
	else
		cout << "First you should open an archive" << endl;
}

bool Endorsement() {
	char answer;
	answer = _getch();
	while (answer != 'y' && answer != 'n') {
		answer = _getch();
	}
	return (answer == 'y' ? true : false);
}