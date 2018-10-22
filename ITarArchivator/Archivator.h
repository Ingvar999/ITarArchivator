#pragma once

struct Header {
	char name[100];
	uint16_t nextOffset;
	uint32_t size;
};

class Archivator{
public:
	Archivator();
	~Archivator();
};

