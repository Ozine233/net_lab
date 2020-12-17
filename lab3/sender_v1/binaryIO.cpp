#include "binaryIO.h"

binaryIO::binaryIO() {}

void binaryIO::init(QString filepath, unsigned int MSS)
{
	this->file.setFileName(filepath);
	this->file.open(QIODevice::ReadOnly);
	this->file_in.setDevice(&(this->file));
	this->MSS = MSS;
	this->filename = QFileInfo(filepath).fileName();
	this->remain = this->filesize = QFileInfo(filepath).size();
}

binaryIO::~binaryIO()
{
	if (file.isOpen()) file.close();
}

int binaryIO::read(char *str)
{
	unsigned int len = remain >= MSS ? MSS : remain;
	this->file_in.readRawData(str, len);
	remain -= len;
	return len;
}

bool binaryIO::is_end()
{
	return this->file_in.atEnd();
}

QString binaryIO::get_filename()
{
	return filename;
}

unsigned int binaryIO::get_size()
{
	return filesize;
}