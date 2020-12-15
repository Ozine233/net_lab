#pragma once
#include <QString>
#include <QDataStream>
#include <QFile>

class binaryIO
{
public:
	binaryIO();
	~binaryIO();
	void init(QString filepath, unsigned int len);
	int read(char *str, int len);
	bool is_end();

private:
	QFile file;
	QDataStream file_in;
	unsigned int len;
};
