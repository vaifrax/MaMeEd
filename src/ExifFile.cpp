// TODO: handle non-exif files
// TODO: include directories in list
// TODO: add event handler for changes in folder (update fileList)

// TODO: add dropdown helper for categories and rating of images
// TODO: add possibility to rotate image and/or thumbnail
// TODO: add GPS tags


#include "ExifFile.h"

ExifFile::ExifFile(const char* fileName) {
	this->fileName = string(fileName);
	userComment.position = 0;
	userComment.length = 0;
	userComment.data = NULL;
	thumbnail.position = 0;
	thumbnail.length = 0;
	thumbnail.data = NULL;
	exifFile = NULL;
	modified = false;

	// get file time
	HANDLE fHandle = CreateFile(fileName, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (!GetFileTime(fHandle, &creationTime, &lastAccessTime, &lastWriteTime)) return;
	CloseHandle(fHandle);

	//// open file (for read and write access)
	//exifFile = fopen(fileName, "r+b");
	// open file (for read access)
	exifFile = fopen(fileName, "rb");
	if (exifFile == NULL) return;

	if (!parseFile()) {
		fclose(exifFile);
		exifFile = NULL;
	}
}

ExifFile::~ExifFile() {
	if (thumbnail.data) delete[] (unsigned char*) thumbnail.data;
	if (userComment.data) delete[] (char*) userComment.data;
	if (exifFile) fclose(exifFile);

	if (modified) {
		// set original file time
		HANDLE fHandle = CreateFile(fileName.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		SetFileTime(fHandle, &creationTime, &lastAccessTime, &lastWriteTime);
		CloseHandle(fHandle);
	}
}

bool ExifFile::parseFile() {
	// if it's a jpg file 'start of image' (soi) should be 0xFFD8
	unsigned char soi[2] = {0, 0};
	fread(&soi, 1, 2, exifFile);
	if ((soi[0] != 0xFF) || (soi[1] != 0xD8)) return false;

	do {
		// search for start of APP1 tag containing EXIF data
		unsigned char appMarker[2] = {0, 0}, appLength[2] = {0, 0};
		fread(&appMarker, 1, 2, exifFile);
		fread(&appLength, 1, 2, exifFile);
		if ((appMarker[0] == 0xFF) && (appMarker[1] == 0xE1)) break;
		if ((appMarker[0] == 0xFF) && (appMarker[1] == 0xDA)) return false; // 'start of stream' marker
		fseek(exifFile, appLength[0]*256 + appLength[1] - 2, SEEK_CUR);
	} while (!feof(exifFile));
	if (feof(exifFile)) return false;
//TODO: skip wrong markers !!!
	
	char exifId[6];
	if (fread(&exifId, 1, 6, exifFile) != 6) return false;
	if (strcmp(exifId, "Exif")) return false;

//printf("is exif image\n");

	int exifStartPos = ftell(exifFile);
	char endianId[2];
	fread(&endianId, 1, 2, exifFile);
	bool bigEndian = (endianId[0] == 'I'); // endianId is 'II' for big endian (Intel) or 'MM' for little endian (Motorola)

	// read IFD offset
	fseek(exifFile, 2, SEEK_CUR);
	unsigned int ifdOffset = readNum(exifFile, bigEndian, 4);
	fseek(exifFile, exifStartPos + ifdOffset, SEEK_SET);
	int fieldCount = readNum(exifFile, bigEndian, 2);
	int ifdPointer = 0;

	do {
		// read tag
		int tag = readNum(exifFile, bigEndian, 2);
		int type = readNum(exifFile, bigEndian, 2);

		// parse tag
		switch (tag) {
			case 0x8769: {
				// read IFD pointer
				// read IFD offset
				fseek(exifFile, 4, SEEK_CUR);
				ifdPointer = readNum(exifFile, bigEndian, 4);
				break;}
			case 0x9286:{
				// read user comment id
				userComment.length = readNum(exifFile, bigEndian, 4);
				userComment.position = readNum(exifFile, bigEndian, 4) + exifStartPos;
				break;}
			case 0x201:
				fseek(exifFile, 4, SEEK_CUR);
				thumbnail.position = readNum(exifFile, bigEndian, 4) + exifStartPos;
				break;
			case 0x202:
				fseek(exifFile, 4, SEEK_CUR);
				thumbnail.length = readNum(exifFile, bigEndian, 4) + exifStartPos;
				break;
			default:
				// ignore tag: skip following bytes
				fseek(exifFile, 8, SEEK_CUR);
		}

		fieldCount--;
		if (fieldCount == 0) {
			int nextIfdOffset = readNum(exifFile, bigEndian, 4);
			if (nextIfdOffset) {
				fseek(exifFile, exifStartPos + nextIfdOffset, SEEK_SET);
				fieldCount = readNum(exifFile, bigEndian, 2);
			}
		}

		if ((fieldCount == 0) && ifdPointer) {
			// most of parsing is done but this ifd pointer thing is not read yet
			fseek(exifFile, exifStartPos + ifdPointer, SEEK_SET);
			fieldCount = readNum(exifFile, bigEndian, 2);
			ifdPointer = 0;
		}
//	} while ((!uc_position) || (!thumbnailPos) || (!thumbnailLength)); // TODO
	} while (fieldCount); // TODO
	
	//if (userComment.length < 8) {
	//	fclose(exifFile);
	//	return false;
	//}

	return true;
}

char* ExifFile::getUserCommentData() {
	if (userComment.data) return (char*) userComment.data;

	// file seek to user comment data
	fseek(exifFile, userComment.position, SEEK_SET);
	userComment.data = new char[userComment.length+1];
	fread(userComment.data, 1, userComment.length, exifFile);

	// add a delimiter so that it can be used as char* and delete spaces at end
	char* dPtr = &((char*) userComment.data)[userComment.length]; 
	do {*dPtr-- = '\0';} while ((*dPtr == ' ') && (dPtr >= (char*) userComment.data + 8));

	return (char*) userComment.data;
}

/*
// too long comments will be clipped automatically
bool ExifFile::setUserCommentData(char* newUserComment) {
	if (!hasUserComment()) return false;
	if (newUserComment == NULL) return true;

	modified = true;
	if (!userComment.data) userComment.data = new char[userComment.length+1];

	// set text tpye to ASCII
	strncpy((char*) userComment.data, "ASCII", 8); // filling with NULL chars at end
	strncpy((char*) userComment.data + 8, newUserComment, userComment.length-8);
	// fill up with spaces
	char* spaceFillPtr = &((char*) userComment.data)[userComment.length];
	while (!*spaceFillPtr) *spaceFillPtr-- = ' ';

	// file seek to user comment data
	fseek(exifFile, userComment.position, SEEK_SET);
	fwrite(userComment.data, 1, userComment.length, exifFile);
	return true;
}
*/

unsigned char* ExifFile::getThumbnailData() {
	if (thumbnail.data) return (unsigned char*) thumbnail.data;
	
	// file seek to thumbnail data
	fseek(exifFile, thumbnail.position, SEEK_SET);
	thumbnail.data = new char[thumbnail.length];
	fread(thumbnail.data, 1, thumbnail.length, exifFile);
//printf("th: %i %i\n", thumbnail.position, thumbnail.length);
	return (unsigned char*) thumbnail.data;
}

long ExifFile::readNum(FILE* file, bool bigEndian, int bytes) {
	unsigned char* buf = new unsigned char[bytes];
	fread(buf, 1, bytes, file);
	long num = 0;
	if (bigEndian) for (int i=bytes-1; i>=0; i--) {
		num += buf[i];
		if (i) num <<= 8;
	} else for (int i=0; i<bytes; i++) {
		num += buf[i];
		if (i < bytes-1) num <<= 8;
	}
	delete[] buf;
	return num;
}

