#include "MapTile.h"
#include "Fltk13WorldMap.h"

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
	int t = (int)(floor((lon + 180.0) / 360.0 * pow(2.0, z))); 
	if (t < 0) return t + (1<<z); // wrap around
	if (t >= 1<<z) return t - (1<<z); // wrap around
	return t;
}
 
/*static*/ int MapTile::lat2tiley(double lat, int z) {
	if (lat >= 90) return 0;
	double latRad = lat * M_PI/180.0;
	double la = tan(latRad) + 1.0/cos(latRad);
	if (la <= 0) return (1<<z) - 1; // clamp
	int t = (int)(floor((1.0 - log(la) / M_PI) / 2.0 * pow(2.0, z))); 
	if (t < 0) return 0; // clamp
	if (t >= 1<<z) return (1<<z) - 1; // clamp
	return t;
}
 
/*static*/ double MapTile::tilex2long(double x, int z) {
	return x / pow(2.0, z) * 360.0 - 180;
}
 
/*static*/ double MapTile::tiley2lat(double y, int z) {
	double n = M_PI - 2.0 * M_PI * y / pow(2.0, z);
	return 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
}

void MapTile::draw(double zoom) {
	tex->bind();
	if (zoomLevel > 3) {
		glBegin(GL_QUADS);
		glTexCoord2f(0, 1);
		Fltk13WorldMap::longLatToXYZ(getLongLeft(), getLatBottom());
		glVertex3f(zoom*Fltk13WorldMap::cX, zoom*Fltk13WorldMap::cY, zoom*Fltk13WorldMap::cZ);

		glTexCoord2f(1, 1);
		Fltk13WorldMap::longLatToXYZ(getLongRight(), getLatBottom());
		glVertex3f(zoom*Fltk13WorldMap::cX, zoom*Fltk13WorldMap::cY, zoom*Fltk13WorldMap::cZ);

		glTexCoord2f(1, 0);
		Fltk13WorldMap::longLatToXYZ(getLongRight(), getLatTop());
		glVertex3f(zoom*Fltk13WorldMap::cX, zoom*Fltk13WorldMap::cY, zoom*Fltk13WorldMap::cZ);

		glTexCoord2f(0, 0);
		Fltk13WorldMap::longLatToXYZ(getLongLeft(), getLatTop());
		glVertex3f(zoom*Fltk13WorldMap::cX, zoom*Fltk13WorldMap::cY, zoom*Fltk13WorldMap::cZ);
		glEnd();
	} else {
		int subDivNum = 9 - 2*zoomLevel; //4
		for (int sdy=0; sdy<subDivNum; sdy++) {
			float fsdy1 = sdy/(float) subDivNum;
			float fsdy2 = (sdy+1)/(float) subDivNum;
			float lat1 = tiley2lat(y+fsdy1, zoomLevel);
			float lat2 = tiley2lat(y+fsdy2, zoomLevel);
			for (int sdx=0; sdx<subDivNum; sdx++) {
				float fsdx1 = sdx/(float) subDivNum;
				float fsdx2 = (sdx+1)/(float) subDivNum;
				float long1 = tilex2long(x+fsdx1, zoomLevel);
				float long2 = tilex2long(x+fsdx2, zoomLevel);
				glBegin(GL_QUADS);

				glTexCoord2f(fsdx1, fsdy2);
				Fltk13WorldMap::longLatToXYZ(long1, lat2);
				glVertex3f(zoom*Fltk13WorldMap::cX, zoom*Fltk13WorldMap::cY, zoom*Fltk13WorldMap::cZ); // zoom = earth diameter in pixels

				glTexCoord2f(fsdx2, fsdy2);
				Fltk13WorldMap::longLatToXYZ(long2, lat2);
				glVertex3f(zoom*Fltk13WorldMap::cX, zoom*Fltk13WorldMap::cY, zoom*Fltk13WorldMap::cZ);

				glTexCoord2f(fsdx2, fsdy1);
				Fltk13WorldMap::longLatToXYZ(long2, lat1);
				glVertex3f(zoom*Fltk13WorldMap::cX, zoom*Fltk13WorldMap::cY, zoom*Fltk13WorldMap::cZ);

				glTexCoord2f(fsdx1, fsdy1);
				Fltk13WorldMap::longLatToXYZ(long1, lat1);
				glVertex3f(zoom*Fltk13WorldMap::cX, zoom*Fltk13WorldMap::cY, zoom*Fltk13WorldMap::cZ);

				glEnd();
			}
		}
	}
}