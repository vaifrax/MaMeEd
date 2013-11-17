#ifndef MAPTILEZOOMLEVEL_HEADER_INCLUDED
#define MAPTILEZOOMLEVEL_HEADER_INCLUDED

#include "MapTile.h"
//#include <vector>

class MapTileZoomLevel {
  public:
	MapTileZoomLevel(int level);
	~MapTileZoomLevel();

	int level; // 0 .. 18
	MapTile*** tilesArray; // 2D array with MapTile pointers
	int tilesArraySize; // in both x and y directions

	// std::vector<
};

#endif // MAPTILEZOOMLEVEL_HEADER_INCLUDED
