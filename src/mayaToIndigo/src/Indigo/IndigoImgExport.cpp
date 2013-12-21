#include "IndigoImgExport.h"
#include <iostream>
#include <fstream>


void BitMap::writeFloatBufferToBMP(const Indigo::String& path, float* buffer, size_t w, size_t h)
{
	// Convert buffer data to 8-bit first:
	std::vector<uint8> uint8_buf(w * h * 4);

	for(size_t i=0; i<w * h * 4; ++i)
		uint8_buf[i] = (uint8)(buffer[i] * 255.9f);

	writeUInt8BufferToBMP(path, &uint8_buf[0], w, h);
}


void BitMap::writeUInt8BufferToBMP(const Indigo::String& path, uint8* buffer, size_t w, size_t h)
{
	int bytesPerLine = (int)w * 3;
	// round up to a dword boundary
	if(bytesPerLine & 0x0003) 
	{
		bytesPerLine |= 0x0003;
		++bytesPerLine;
	}

	BITMAP_HEADER header;
	memset(&header, 0, sizeof(BITMAP_HEADER));
	header.type = BITMAP_ID;
	header.size = sizeof(BITMAP_HEADER) + sizeof(BITMAP_INFOHEADER) + bytesPerLine * (int)h;
	header.offset = sizeof(BITMAP_HEADER) + sizeof(BITMAP_INFOHEADER);

	BITMAP_INFOHEADER infoheader;
	memset(&infoheader, 0, sizeof(BITMAP_INFOHEADER));
	infoheader.size = sizeof(infoheader);
	infoheader.width = (int)w;
	infoheader.height = (int)h;
	infoheader.planes = 1;
	infoheader.bits = 24;
	infoheader.compression = 0L;

	Indigo::Vector<char> buf = path.getNullTerminatedBuffer();
	std::ofstream file(&buf[0], std::ios::out | std::ios::binary);
	if(!file)
	{
		std::cerr << "Could not open file for writing." << std::endl;
		exit(1);
	}

	// Write headers
	file.write((const char*)&header, sizeof(header));
	file.write((const char*)&infoheader, sizeof(infoheader));

	// Write image data
	char* line = new char[bytesPerLine];
	for(int y=(int)h-1; y >= 0; --y)
	{
		for(unsigned int x=0; x<w; ++x)
		{
			line[x*3] = ((const char*)buffer)[(y*w + x)*4 + 2]; // b
			line[x*3+1] = ((const char*)buffer)[(y*w + x)*4 + 1]; // g
			line[x*3+2] = ((const char*)buffer)[(y*w + x)*4]; // r
		}

		file.write(line, bytesPerLine);
	}
	delete[] line;
}

