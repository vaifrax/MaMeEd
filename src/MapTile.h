#ifndef MAPTILE_HEADER_INCLUDED
#define MAPTILE_HEADER_INCLUDED

#include <string>

class MLOpenGLTex;

class MapTile {
  public:
	MapTile(int zoomLevel, int x, int y); // x and y: tile coordinates
	~MapTile();

	void loadFromFile();

	double getLongLeft() {return tilex2long(x, zoomLevel);}
	double getLongRight() {return tilex2long(x+1, zoomLevel);}
	double getLatTop() {return tiley2lat(y, zoomLevel);}
	double getLatBottom() {return tiley2lat(y+1, zoomLevel);}

	// transformations provided by openstreetmap wiki
	static int MapTile::long2tilex(double lon, int z);
	static int MapTile::lat2tiley(double lat, int z);
	static double MapTile::tilex2long(double x, int z);
	static double MapTile::tiley2lat(double y, int z);

	int zoomLevel;
	int x, y; // tile coordinates
	int cacheState; // not loaded, downloading, loading, available, unloading???
	MLOpenGLTex* tex;
	std::string fileName;
};

#endif // MAPTILE_HEADER_INCLUDED
