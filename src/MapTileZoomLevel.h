#ifndef MAPTILEZOOMLEVEL_HEADER_INCLUDED
#define MAPTILEZOOMLEVEL_HEADER_INCLUDED

#include "MapTile.h"
//#include <vector>
#include <map>

class MapTileZoomLevel {
  public:
	MapTileZoomLevel(int level);
	~MapTileZoomLevel();

	int level; // 0 .. 18
	//MapTile*** tilesArray; // 2D array with MapTile pointers
	int tileNum; // in both x and y directions; equals 2^level
	std::map<int, MapTile*> tilesMap; // use x+y*tileNum to access


	void draw(int w, int h, double zoom, double angle1, double angle2);
};

#endif // MAPTILEZOOMLEVEL_HEADER_INCLUDED
