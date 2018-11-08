#include "pch.h"
#include "ITarArchivator.h"
#include "SimpleFileSystem.h"

struct Header {
	static const byte file_md = 1;
	static const byte folder_md = 2;

	char name[100];
	uint16_t nextOffset;
	uint32_t size;
	bool isValid;
	byte mode;
};

ITarArchivator::ITarArchivator(const string &filename)
{
	archivename = filename;
	archive = new fstream();
	archive->open(filename, fstream::out | fstream::in | fstream::binary | fstream::ate);
	if (!archive->is_open()) {
		archive->open(filename, fstream::out | fstream::app);
		archive->close();
		archive->open(filename, fstream::out | fstream::in | fstream::binary | fstream::ate);
	}
	currentBlocksCount = archive->tellg() / blockSize;
	filesystem = new SimpleFileSystem();
}

ITarArchivator::~ITarArchivator()
{
	if (archive->is_open()) {
		Compact();
		archive->close();
	}
	delete archive;
	delete filesystem;
}

const string &ITarArchivator::getArchiveName() {
	return archivename;
}

int ITarArchivator::AddFile(const string &filename) {
	ifstream file;
	file.open(filename, ifstream::in | ifstream::binary | ifstream::ate);
	int size = file.tellg();
	archive->seekp(currentBlocksCount * blockSize);
	
	fill(begin(buffer), end(buffer), 0);
	Header *hed = (Header *)buffer;
	string shortname = filename.substr(filename.find_last_of('\\')+1);
	shortname.copy(hed->name, shortname.size());
	hed->isValid = true;
	hed->size = size;
	hed->nextOffset = (uint16_t)ceilf((float)hed->size / blockSize) + 1;
	hed->mode = Header::file_md;
 	currentBlocksCount += hed->nextOffset;

	archive->write(buffer, blockSize);
	file.seekg(ios::beg);
	while (!file.eof()) {
		file.read(buffer, blockSize);
		archive->write(buffer, blockSize);
	}
	archive->flush();
	return size;
}

vector<string> ITarArchivator::GetList() {
	vector<string> result;
	Header *hed = (Header *)buffer;
	int currentBlock = 0;
	while (currentBlock < currentBlocksCount) {
		archive->seekg(currentBlock * blockSize);
		archive->read(buffer, blockSize);
		if (hed->isValid) {
			result.push_back(hed->name);
			switch (hed->mode)
			{
			case Header::file_md:
				currentBlock += hed->nextOffset;
				break;
			case Header::folder_md:
				ReadFolder(result, currentBlock, string(hed->name) + "\\");
				break;
			}
		}
	}
	return result;
}

void ITarArchivator::ReadFolder(vector<string> &result, int &currentBlock, const string &path) {
	Header *hed = (Header *)buffer;
	archive->seekg(currentBlock * blockSize);
	archive->read(buffer, blockSize);
	int endFolder = hed->nextOffset + currentBlock;
	currentBlock++;
	while (currentBlock != endFolder) {
		archive->seekg(currentBlock * blockSize);
		archive->read(buffer, blockSize);
		if (hed->isValid) {
			result.push_back(path + hed->name);
			switch (hed->mode)
			{
			case Header::file_md:
				currentBlock += hed->nextOffset;
				break;
			case Header::folder_md:
				ReadFolder(result, currentBlock, path + hed->name + "\\");
				break;
			}
		}
	}
}

void ITarArchivator::RemoveFile(const string &filename) {
	Header *hed = (Header *)buffer;
	int currentBlock = 0;
	while (currentBlock < currentBlocksCount) {
		archive->seekg(currentBlock * blockSize);
		archive->read(buffer, blockSize);
		if (hed->isValid && filename.compare(hed->name) == 0) {
			hed->isValid = false;
			archive->seekp(currentBlock * blockSize);
			archive->write(buffer, blockSize);
			archive->flush();
			return;
		}
		currentBlock += hed->nextOffset;
	}
}

void ITarArchivator::ExtractFile(const string &directory, const string &filename) {
	Header *hed = (Header *)buffer;
	int currentBlock = 0;
	while (currentBlock < currentBlocksCount) {
		archive->seekg(currentBlock * blockSize);
		archive->read(buffer, blockSize);
		if (filename.compare(hed->name) == 0) {
			ofstream file;
			file.open(directory + filename, ofstream::out | ofstream::binary | ofstream::beg);
			int remain = hed->size;
			if (remain > 0) {
				while (remain > blockSize) {
					archive->read(buffer, blockSize);
					file.write(buffer, blockSize);
					remain -= blockSize;
				}
				archive->read(buffer, blockSize);
				file.write(buffer, remain);
			}
			file.close();
			return;
		}
		currentBlock += hed->nextOffset;
	}
}

void ITarArchivator::Clear() {
	filesystem->ResizeFile(archivename, 0);
	currentBlocksCount = 0;
}

void ITarArchivator::Compact() {
	bool haveFreeSpace = false;
	int wPos, rPos;
	Header *hed = (Header *)buffer;
	int currentBlock = 0;
	while (currentBlock < currentBlocksCount) {
		archive->seekg(currentBlock * blockSize);
		archive->read(buffer, blockSize);
		int n = hed->nextOffset;
		if (hed->isValid) {
			if (haveFreeSpace) {
				rPos = archive->tellg();
				archive->seekp(wPos);
				archive->write(buffer, blockSize);
				wPos += blockSize;
				for (int i = 1; i < n; ++i) {
					archive->seekg(rPos);
					archive->read(buffer, blockSize);
					archive->seekp(wPos);
					archive->write(buffer, blockSize);
					rPos += blockSize;
					wPos += blockSize;
				}
			}
		}
		else {
			if (!haveFreeSpace) {
				haveFreeSpace = true;
				wPos = currentBlock * blockSize;
			}
		}
		currentBlock += n;
	}
	if (haveFreeSpace) {
		filesystem->ResizeFile(archivename, wPos);
		currentBlocksCount = wPos / blockSize;
		archive->flush();
	}
}

int ITarArchivator::AddFolder(const string &foldername) {
	int size = 0;
	int headerPosition = currentBlocksCount;
	currentBlocksCount++;
	vector<string> folders = filesystem->GetContentList(foldername, false, true);
	for(int i = 0; i < folders.size(); ++i) {
		if (folders[i] != "." && folders[i] != "..")
			size += AddFolder(foldername + folders[i] + "\\");
	}
	vector<string> files = filesystem->GetContentList(foldername, true, false);
	for (int i = 0; i < files.size(); ++i) {
		size += AddFile(foldername + files[i]);
	}
	fill(begin(buffer), end(buffer), 0);
	Header *hed = (Header *)buffer;
	int pos = foldername.find_last_of('\\', foldername.size() - 2) + 1;
	string shortname = foldername.substr(pos, foldername.size() - pos - 1);
	shortname.copy(hed->name, shortname.size());
	hed->mode = Header::folder_md;
	hed->isValid = true;
	hed->size = size;
	hed->nextOffset = currentBlocksCount - headerPosition;
	archive->seekp(headerPosition * blockSize);
	archive->write(buffer, blockSize);
	archive->flush();
	return size;
}

void ITarArchivator::RemoveFolder(const string &foldername) {

}

void ITarArchivator::ExtractFolder(const string &directory, const string &foldername) {

}
