#ifndef EXIFFILE_HEADER_INCLUDED
#define EXIFFILE_HEADER_INCLUDED

#include <windows.h>
#include <stdio.h>
#include <string>
using namespace std;

class ExifFile {
  public:
	ExifFile(const char* fileName);
	~ExifFile();

	char* getUserCommentData();
	bool hasUserComment() {return (userComment.length > 8);};
	int getUserCommentLength() {return userComment.length;};
	int getThumbnailLength() {return thumbnail.length;};
	//bool setUserCommentData(char* newUserComment); // too long comments will be clipped automatically
	unsigned char* getThumbnailData();
	bool fileOk() {return exifFile;};
	
	FILETIME getLastModifiedTime() {return lastWriteTime;};
	
  protected:
	bool parseFile();
	long readNum(FILE* file, bool bigEndian, int bytes);

	string fileName;
	FILE* exifFile; // is NULL if exif data couldn't be read

	struct ExifDataInfo {
		long position, length; // in bytes from file start
		void* data;
	} userComment, thumbnail;

	bool modified;

	FILETIME creationTime, lastAccessTime, lastWriteTime;
};

#endif //  EXIFFILE_HEADER_INCLUDED
