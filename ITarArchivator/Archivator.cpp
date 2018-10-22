#include "pch.h"
#include "Archivator.h"

Archivator::Archivator(const string &filename)
{
	this->filename = filename;
	archiv = new ofstream();
	archiv->open(filename, fstream::out | fstream::binary);
}

Archivator::~Archivator()
{
	if (archiv->is_open())
		archiv->close();
	delete archiv;
}

const string &Archivator::getName() {
	return filename;
}

void Archivator::AddFile(const string &filename) {
	ifstream file;
	file.open(filename, ifstream::in | ifstream::binary);
}

vector<string> Archivator::GetList() {

}

void Archivator::RemoveFile(const string &filename) {

}

void Archivator::Exctract(const string &directory) {

}