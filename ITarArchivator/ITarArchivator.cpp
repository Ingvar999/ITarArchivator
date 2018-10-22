
#include "pch.h"

using namespace std;

vector<string> Dir(const string &path);

int main()
{
	string currentPath("C:\\*");
	string command;
	string item;
	while (1) {
		getline(cin, command);
		stringstream ss(command);
		vector<string> parts;
		while (getline(ss, item, ' ')) 
			parts.push_back(item);
		if (parts[0] == "dir") {
			vector<string> files = Dir(currentPath);
			for (int i = 0; i < files.size(); ++i)
				cout << files[i] << endl;
		}
		else if (parts[0] == "cd") {

		}

	}
}

vector<string> Dir(const string &path) {
	vector<string> result;
	WIN32_FIND_DATAA fi;
	ZeroMemory(&fi, sizeof(fi));
	HANDLE h = FindFirstFileA(path.c_str(), &fi);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			 result.push_back(fi.cFileName);
		} while (FindNextFileA(h, &fi));
		FindClose(h);
	}
	return result;
}