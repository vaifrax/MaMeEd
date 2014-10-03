#include "MapTile.h"

#include <math.h>
#include <iomanip>
#include <sstream>

#include "../tools/MLOpenGLTex.h"
#include "../tools/ddsfile.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// TODO
char* mapCachePath = "E:\\coding\\2012\\MarcelsMetadataEditor\\map-cache\\";

std::string toZeroPaddedStr(int num, int len) {
	std::ostringstream ss;
	ss << std::setw(len) << std::setfill('0') << num;
	return ss.str();
}

MapTile::MapTile(int zoomLevel, int x, int y) {
	this->zoomLevel = zoomLevel;
	this->x = x;
	this->y = y;
	tex = NULL;
	fileName = std::string(mapCachePath) + toZeroPaddedStr(zoomLevel, 2) + "/" + toZeroPaddedStr(x, 6) + "/" + toZeroPaddedStr(y, 6) + ".dds";

	std::cout << fileName << std::endl;
}

MapTile::~MapTile() {
	if (tex) delete tex;
}

void MapTile::loadFromFile() {
	MLImage* img = loadDDS(fileName);
	tex = new MLOpenGLTex(img);

	delete img;
}

/////////////////////////////////////////////////////////////////////

/*static*/ int MapTile::long2tilex(double lon, int z) { 
	return (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z))); 
}
 
/*static*/ int MapTile::lat2tiley(double lat, int z) { 
	double latRad = lat * M_PI/180.0;
	return (int)(floor((1.0 - log( tan(latRad) + 1.0/cos(latRad)) / M_PI) / 2.0 * pow(2.0, z))); 
}
 
/*static*/ double MapTile::tilex2long(double x, int z) {
	return x / pow(2.0, z) * 360.0 - 180;
}
 
/*static*/ double MapTile::tiley2lat(double y, int z) {
	double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
	return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

