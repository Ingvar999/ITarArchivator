#include "pch.h"
#include "Archivator.h"

Archivator::Archivator(const string &filename)
{
	archivname = filename;
	archiv = new fstream();
	archiv->open(filename, fstream::out | fstream::in | fstream::binary | fstream::app | fstream::ate);
	currentBlocksCount = archiv->tellg() / blockSize;
}

Archivator::~Archivator()
{
	if (archiv->is_open())
		archiv->close();
	delete archiv;
}

const string &Archivator::getName() {
	return archivname;
}

void Archivator::AddFile(const string &filename) {
	ifstream file;
	file.open(filename, ifstream::in | ifstream::binary | ifstream::ate);
	archiv->seekp(currentBlocksCount * blockSize);
	
	fill(begin(buffer), end(buffer), 0);
	Header *hed = (Header *)buffer;
	string shortname = filename.substr(filename.find_last_of('\\')+1);
	shortname.copy(hed->name, shortname.size());
	hed->isValid = true;
	hed->size = file.tellg();
	hed->nextOffset = (uint16_t)ceilf((float)hed->size / blockSize) + 1;
 	currentBlocksCount += hed->nextOffset;

	archiv->write(buffer, blockSize);
	file.seekg(ios::beg);
	while (!file.eof()) {
		file.read(buffer, blockSize);
		archiv->write(buffer, blockSize);
	}
	archiv->flush();
}

vector<string> Archivator::GetList() {
	vector<string> result;
	Header *hed = (Header *)buffer;
	int currentBlock = 0;
	while (currentBlock < currentBlocksCount) {
		archiv->seekg(currentBlock * blockSize);
		archiv->read(buffer, blockSize);
		if (hed->isValid)
			result.push_back(hed->name);
		currentBlock += hed->nextOffset;
	}
	return result;
}

void Archivator::RemoveFile(const string &filename) {
	Header *hed = (Header *)buffer;
	int currentBlock = 0;
	while (currentBlock < currentBlocksCount) {
		archiv->seekg(currentBlock * blockSize);
		archiv->read(buffer, blockSize);
		if (filename.compare(hed->name) == 0) {
			hed->isValid = false;
			archiv->seekp(currentBlock * blockSize);
			archiv->write(buffer, blockSize);
			archiv->flush();
			return;
		}
		currentBlock += hed->nextOffset;
	}
}

void Archivator::Exctract(const string &directory) {

}