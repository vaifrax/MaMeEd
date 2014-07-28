#ifndef EXIFFILEM_HEADER_INCLUDED
#define EXIFFILEM_HEADER_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <string>
using namespace std;

class MDFile;

class ExifFileM {
  public:
	ExifFileM(const char* fileName, MDFile* mdf);
	~ExifFileM();
	bool parseFile();

  protected:
	string fileName;
	FILE* exifFile; // is NULL if exif data couldn't be read
	bool bigEndian;
	MDFile* mdf;
	long exifStartPos;

	bool readGPS(long position);
	bool readExifIFD(long position);

	long readNum(int bytes);
	std::string readString(long position, long length);
	double readRational(long position);
};

#endif //  EXIFFILEM_HEADER_INCLUDED
