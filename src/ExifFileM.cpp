// TODO: handle non-exif files
// TODO: include directories in list
// TODO: add event handler for changes in folder (update fileList)

// TODO: add dropdown helper for categories and rating of images
// TODO: add possibility to rotate image and/or thumbnail
// TODO: add GPS tags


#include "ExifFileM.h"
#include "MDFile.h"

#include <iostream> // for cout only

ExifFileM::ExifFileM(const char* fileName, MDFile* mdf) {
	this->fileName = string(fileName);
	this->mdf = mdf;
	exifFile = NULL;

	// open file (for read access)
	exifFile = fopen(fileName, "rb");
}

ExifFileM::~ExifFileM() {
	if (exifFile) fclose(exifFile);
}

bool ExifFileM::parseFile() {
	if (exifFile == NULL) return false;

	int ifdNum = 0;

	// if it's a jpg file 'start of image' (soi) should be 0xFFD8
	unsigned char soi[2];
	if (fread(&soi, 1, 2, exifFile) < 2) return false;
	if ((soi[0] != 0xFF) || (soi[1] != 0xD8)) return false;

	int len = 0;
	int cnt = 0;
	unsigned char appMarker[2], appLength[2];
	while(true) {
		// search for start of APP1 tag containing EXIF data
		if (fread(&appMarker, 1, 2, exifFile) < 2) return false;
		if (fread(&appLength, 1, 2, exifFile) < 2) return false;
		len = appLength[0]*256 + appLength[1];

		if ((appMarker[0] == 0xFF) && (appMarker[1] == 0xE1)) break; // found exif data
		if ((appMarker[0] == 0xFF) && (appMarker[1] == 0xDA)) return false; // 'start of stream' marker

		// go to next marker
		fseek(exifFile, len - 2, SEEK_CUR);
		cnt++;

		if (cnt > 10) return false; // likely, this file is corrupt   TODO: is this a good value/way to do this?
		if (feof(exifFile)) return false;
	};

	if (len < 16) return false; // must be at least that long

	char exifId[6];
	if (fread(&exifId, 1, 6, exifFile) != 6) return false;
	if (strcmp(exifId, "Exif")) return false;

	exifStartPos = ftell(exifFile);
	char endianId[2];
	fread(&endianId, 1, 2, exifFile);
	bigEndian = (endianId[0] == 'I'); // endianId is 'II' for big endian (Intel) or 'MM' for little endian (Motorola)

	// read IFD offset
	fseek(exifFile, 2, SEEK_CUR);
	unsigned int ifdOffset = readNum(4);

	fseek(exifFile, exifStartPos + ifdOffset, SEEK_SET);
	int fieldCount = readNum(2);
	int ifdPointer = 0;

	long gpsInfoPosition = 0;
	long exifIFDPosition = 0;

	do {
		// read tag
		int tag = readNum(2);
		int type = readNum(2);
		int count = readNum(4);
		long data = readNum(4);

		// parse tag
		switch (tag) {
			case 0x8769:
				// read IFD pointer
				exifIFDPosition = exifStartPos + data;
				break;
			case 0x8825:
				// GPS info
				gpsInfoPosition = exifStartPos + data;
				break;
/*
			case 0x0100: // image width
				mdf->setKeyValueSrc("width", (long) data, "EXIF");
				break;
			case 0x0101: // image height
				mdf->setKeyValueSrc("height", (long) data, "EXIF");
				break;
*/

			case 0x0201:
				mdf->setKeyValueSrc("thumbnailPosition", exifStartPos + data, "EXIF");
//todo: is this correct: exifStartPos + data ?
				break;
			case 0x0202:
				mdf->setKeyValueSrc("thumbnailSize", data, "EXIF");
				break;

			case 0x0112: // orientation
				mdf->setKeyValueSrc("storageOrientation", data, "EXIF");
				break;
			case 0x132:
				mdf->setKeyValueSrc("dateTime", readString(data + exifStartPos, count), "EXIF");
				break;
			case 0x10E:
				mdf->setKeyValueSrc("imgageTitle", readString(data + exifStartPos, count), "EXIF");
				break;
			case 0x10F:
				mdf->setKeyValueSrc("deviceManufacturer", readString(data + exifStartPos, count), "EXIF");
				break;
			case 0x110:
				mdf->setKeyValueSrc("deviceModel", readString(data + exifStartPos, count), "EXIF");
				break;
			case 0x13B:
				mdf->setKeyValueSrc("artist", readString(data + exifStartPos, count), "EXIF");
				break;
			case 0x8298:
				mdf->setKeyValueSrc("copyright", readString(data + exifStartPos, count), "EXIF");
				break;
		}

		fieldCount--;

		// next idf
		if (fieldCount == 0) {
			ifdNum++;
			int nextIfdOffset = readNum(4);
			if (nextIfdOffset) {
				fseek(exifFile, exifStartPos + nextIfdOffset, SEEK_SET);
				fieldCount = readNum(2);
			}

			if (ifdPointer) {
				// most of parsing is done but this ifd pointer thing is not read yet
				fseek(exifFile, exifStartPos + ifdPointer, SEEK_SET);
				fieldCount = readNum(2);
				ifdPointer = 0;
			}
		}

//	} while ((!uc_position) || (!thumbnailPos) || (!thumbnailLength)); // TODO
	} while (fieldCount && (ifdNum < 2)); // TODO

	if (exifIFDPosition) {
		if (!readExifIFD(exifIFDPosition)) return false;
	}

	if (gpsInfoPosition) {
		if (!readGPS(gpsInfoPosition)) return false;
	}

	return true;
}

bool ExifFileM::readExifIFD(long position) {
	fseek(exifFile, position, SEEK_SET);
	int fieldCount = readNum(2);

	while (fieldCount > 0) {
		int tag = readNum(2);
		int type = readNum(2);
		int count = readNum(4);
		long data = 0;
		switch (type) {
			case 1:
				data = readNum(1);
				fseek(exifFile, 3, SEEK_CUR);
				break;
			case 3:
				data = readNum(2);
				fseek(exifFile, 2, SEEK_CUR);
				break;
			default:
				data = readNum(4);
		}

		// parse tag
		switch (tag) {
			case 0x829A:
				mdf->setKeyValueSrc("ExposureTime", readRational(data + exifStartPos), "EXIF");
				break;
			case 0x829D:
				mdf->setKeyValueSrc("FNumber", readRational(data + exifStartPos), "EXIF");
				break;
			case 0x8827:
				mdf->setKeyValueSrc("ISOSpeed", data, "EXIF");
				break;

			case 0x9204:
				mdf->setKeyValueSrc("ExposureBias", readRational(data + exifStartPos), "EXIF");
				break;
			case 0x9206:
				mdf->setKeyValueSrc("SubjectDistance", readRational(data + exifStartPos), "EXIF");
				break;
			case 0x9207:
				mdf->setKeyValueSrc("MeteringMode", data, "EXIF");
				break;
			case 0x9286:
				if ((count > 8) && (count < 300)) {
					std::string format = readString(exifStartPos + data, 8);
					if (format == "ASCII") {
						mdf->setKeyValueSrc("userComment", readString(exifStartPos + data + 8, count-8), "EXIF");
					}
				break;}

			case 0x9209:
				mdf->setKeyValueSrc("FlashFired", (data&0x01) ? "yes" : "no", "EXIF");
				break;
			case 0xA405:
				mdf->setKeyValueSrc("FocalLengthIn35mm", data, "EXIF");
				break;
			case 0xA002: // image width
				mdf->setKeyValueSrc("Width", data, "EXIF");
				break;
			case 0xA003: // image height
				mdf->setKeyValueSrc("Height", data, "EXIF");
				break;
		}

		fieldCount--;
	}

	return true;
}

bool ExifFileM::readGPS(long position) {
	fseek(exifFile, position, SEEK_SET);
	int fieldCount = readNum(2);

	double longitude = 0; // west (negative), east (positive) in -180..180 degrees
	bool longitudeEast = true;

	double latitude = 0; // south (negative), north (positive) in -90..90 degrees
	bool latitudeNorth = true;

	double altitude = 0;
	bool altitudeAboveSeaLevel = true;
	bool hasAltitudeTag = false;
	bool measureMode3D = true;

	while (fieldCount > 0) {
		int tag = readNum(2);
		int type = readNum(2);
		int count = readNum(4);
		long data = 0;
		switch (type) {
			case 1:
				data = readNum(1);
				fseek(exifFile, 3, SEEK_CUR);
				break;
			case 3:
				data = readNum(2);
				fseek(exifFile, 2, SEEK_CUR);
				break;
			default:
				data = readNum(4);
		}

		// parse tag
		switch (tag) {
			case 0x0001:
				latitudeNorth = (data != 'S');
				break;
			case 0x0002: {
				double deg = readRational(data + exifStartPos);
				double min = readRational(data + exifStartPos + 8);
				double sec = readRational(data + exifStartPos + 16);
				latitude = deg + min/60.0 + sec/3600.0;
				break;}
			case 0x0003:
				latitudeNorth = (data != 'W');
				break;
			case 0x0004: {
				double deg = readRational(data + exifStartPos);
				double min = readRational(data + exifStartPos + 8);
				double sec = readRational(data + exifStartPos + 16);
				longitude = deg + min/60.0 + sec/3600.0;
				break;}
			case 0x0005:
				altitudeAboveSeaLevel = (data != 1);
				break;
			case 0x0006:
				altitude = readRational(data + exifStartPos);
				hasAltitudeTag = true;
				break;
			case 0x000A: // '2' or '3'
				if (data == '2') measureMode3D = false;
				break;
			case 0x000B: // GPS degree of precision  (using some const, hopefully good)
				mdf->setKeyValueSrc("PositionUncertaintyRadius", (long) (12.3456 * readRational(data + exifStartPos)), "EXIF");
				break;
		}

		fieldCount--;
	}

	if (!longitudeEast) longitude *= -1;
	if (!latitudeNorth) latitude *= -1;
	if (!altitudeAboveSeaLevel) altitude *= -1;

	mdf->setKeyValueSrc("Longitude", longitude, "EXIF");
	mdf->setKeyValueSrc("Latitude", latitude, "EXIF");
	if (hasAltitudeTag && measureMode3D) mdf->setKeyValueSrc("Altitude", altitude, "EXIF");

	return true;
}

long ExifFileM::readNum(int bytes) {
	unsigned char* buf = new unsigned char[bytes];
	fread(buf, 1, bytes, exifFile);
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

std::string ExifFileM::readString(long position, long length) {
	// sanity check
	if (length > 256) return "";

	long prevpos = ftell(exifFile);

	fseek(exifFile, position, SEEK_SET);
	char* tmp = new char[length+1];
	fread(tmp, 1, length, exifFile);
	tmp[length] = 0;
	std::string res = std::string(tmp);
	delete[] tmp;

	fseek(exifFile, prevpos, SEEK_SET);
	return res;
}

double ExifFileM::readRational(long position) {
	long prevpos = ftell(exifFile);

	fseek(exifFile, position, SEEK_SET);
	int numerator = readNum(4);
	int denominator = readNum(4);
	fseek(exifFile, prevpos, SEEK_SET);
	if (denominator == 0) return 0; // error?

	return numerator/(double) denominator;
}