#ifndef EXIFFILE_HEADER_INCLUDED
#define EXIFFILE_HEADER_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <string>
using namespace std;

class MDFile;

class ExifFile {
  public:
	ExifFile(const char* fileName, MDFile* mdf = NULL);
	~ExifFile();

	int getThumbnailLength() {return thumbnail.length;};
	unsigned char* getThumbnailData();
	bool fileOk() {return (exifFile!=NULL);};

	//FILETIME getLastModifiedTime() {return lastWriteTime;};

  protected:
	bool parseFile();
	long readNum(FILE* file, int bytes);

	string fileName;
	FILE* exifFile; // is NULL if exif data couldn't be read

	struct ExifDataInfo {
		long position, length; // in bytes from file start
		void* data;
	} thumbnail;

	bool modified;

	void readGPS(long giLength, long giPosition);
	char* getUserCommentData();
	bool hasUserComment() {return (userComment.length > 8);};

	bool bigEndian;
	MDFile* mdf;

	//FILETIME creationTime, lastAccessTime, lastWriteTime;
};

#endif //  EXIFFILE_HEADER_INCLUDED
