#include "MapTileZoomLevel.h"
#include "../tools/MLOpenGLTex.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

MapTileZoomLevel::MapTileZoomLevel(int level) {
	this->level = level;
	tilesArraySize = 0;

	if (level <= 4) {
		tilesArraySize = 1 << level;

		tilesArray = new MapTile**[tilesArraySize];
		for (int x=0; x<tilesArraySize; x++) {
			tilesArray[x] = new MapTile*[tilesArraySize];
			for (int y=0; y<tilesArraySize; y++) {
				if (level < 3) { // preload all tiles up to level 2
					tilesArray[x][y] = new MapTile(level, x, y);
					tilesArray[x][y]->loadFromFile();
				} else {
					tilesArray[x][y] = NULL;
				}
			}
		}
	}
}

MapTileZoomLevel::~MapTileZoomLevel() {
	if (tilesArraySize) {
		for (int x=0; x<tilesArraySize; x++) {
			for (int y=0; y<tilesArraySize; y++) {
				delete tilesArray[x][y];
			}
			delete tilesArray[x];
		}		
	}
}

void MapTileZoomLevel::draw(int w, int h, double zoom, double angle1, double angle2) {
	//int minY = 0;//MapTile::lat2tiley(angle2, level);
	//int maxY = tilesArraySize-1;//MapTile::lat2tiley(angle2, level);
	double wy = (h>=zoom) ? 90 : asin(h/zoom)*180/M_PI;
	int minY = MapTile::lat2tiley(angle2 + wy, level);
	int maxY = MapTile::lat2tiley(angle2 - wy, level);

	int minX = 0;
	int maxX = tilesArraySize-1;
	if (level > 1) { // todo: set to 3,4,5 ?
		double wx = (w>=zoom) ? 90 : asin(w/zoom)*180/M_PI / cos(angle2*M_PI/180);
		if (wx < 160) {
			minX = MapTile::long2tilex(angle1 - wx, level);
			maxX = MapTile::long2tilex(angle1 + wx, level);
		}
	}

	for (int y=minY; y<=maxY; y++) {
		int x = minX;
		while (true) {
			MapTile* mt = tilesArray[x][y];
//todo: if not available, use previous level's texture
			if (!mt) {
				// load
				mt = new MapTile(level, x, y);
				tilesArray[x][y] = mt;
				mt->loadFromFile();
			}
			if (mt) {
				mt->draw(zoom);
			} else {
				// TODO: draw with texture from previous level
			}
			if (x == maxX) break;
			x = (x+1) % tilesArraySize;
		}
	}
}