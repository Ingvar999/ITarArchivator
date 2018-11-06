#include "pch.h"
#include "ITarArchivator.h"
#include "SimpleFileSystem.h"

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

void ITarArchivator::AddFile(const string &filename) {
	ifstream file;
	file.open(filename, ifstream::in | ifstream::binary | ifstream::ate);
	archive->seekp(currentBlocksCount * blockSize);
	
	fill(begin(buffer), end(buffer), 0);
	Header *hed = (Header *)buffer;
	string shortname = filename.substr(filename.find_last_of('\\')+1);
	shortname.copy(hed->name, shortname.size());
	hed->isValid = true;
	hed->size = file.tellg();
	hed->nextOffset = (uint16_t)ceilf((float)hed->size / blockSize) + 1;
 	currentBlocksCount += hed->nextOffset;

	archive->write(buffer, blockSize);
	file.seekg(ios::beg);
	while (!file.eof()) {
		file.read(buffer, blockSize);
		archive->write(buffer, blockSize);
	}
	archive->flush();
}

vector<string> ITarArchivator::GetList() {
	vector<string> result;
	Header *hed = (Header *)buffer;
	int currentBlock = 0;
	while (currentBlock < currentBlocksCount) {
		archive->seekg(currentBlock * blockSize);
		archive->read(buffer, blockSize);
		if (hed->isValid)
			result.push_back(hed->name);
		currentBlock += hed->nextOffset;
	}
	return result;
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

void ITarArchivator::AddFolder(const string &foldername) {

}

void ITarArchivator::RemoveFolder(const string &foldername) {

}

void ITarArchivator::ExtractFolder(const string &directory, const string &foldername) {

}
