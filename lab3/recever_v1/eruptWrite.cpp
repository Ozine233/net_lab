#include "eruptWrite.h"

eruptWrite::eruptWrite(QObject *parent /* = nullptr */)
{
	quit = false;
}

void eruptWrite::push_packet(packet *rpacket)
{
	this->buffer.push(rpacket);
}

bool eruptWrite::is_finishsed()
{
	return this->buffer.empty();
}

void eruptWrite::set_quit()
{
	quit = true;
}

void eruptWrite::set_start()
{
	quit = false;
}

void eruptWrite::on_write(QString filename)
{
	// 建立文件
	file.setFileName(filename);
	file.open(QIODevice::WriteOnly);
	file_out.setDevice(&file);

	while (!quit)
	{
		if (!buffer.empty())
		{
			packet *r_packet = buffer.front();
			file_out.writeRawData(r_packet->data, r_packet->size);
			file.flush();
			buffer.pop();
			free(r_packet);
		}
	}

	file.close();
}