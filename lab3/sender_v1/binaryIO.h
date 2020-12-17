#pragma once
#include <QString>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>

class binaryIO
{
public:
	binaryIO();
	~binaryIO();
	void init(QString filepath, unsigned int MSS);
	int read(char *str);
	bool is_end();
	QString get_filename();
	unsigned int get_size();

private:
	QFile file;
	QDataStream file_in;

	QString filename;
	unsigned int filesize;
	unsigned int remain;
	unsigned int MSS;
};
