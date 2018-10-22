#include "pch.h"
#include "Archivator.h"

Archivator::Archivator(const string &filename)
{
	archiv = new fstream();
	archiv->open(filename);
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
