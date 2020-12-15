#include "binaryIO.h"

binaryIO::binaryIO() {}

void binaryIO::init(QString filepath, unsigned int len)
{
	this->file.setFileName(filepath);
	this->file.open(QIODevice::ReadOnly);
	this->file_in.setDevice(&(this->file));
	this->len = len;
}

binaryIO::~binaryIO()
{
	if (file.isOpen()) file.close();
}

int binaryIO::read(char *str, int len)
{
	return this->file_in.readRawData(str, len);
}

bool binaryIO::is_end()
{
	return this->file_in.atEnd();
}