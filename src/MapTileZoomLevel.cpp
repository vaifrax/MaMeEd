#include "MapTileZoomLevel.h"
#include "../tools/MLOpenGLTex.h"

#include <iostream> // purely for debugging with std::cout

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

MapTileZoomLevel::MapTileZoomLevel(int level) {
	this->level = level;
	tileNum = 1 << level;

	if (level <= 4) {

		for (int x=0; x<tileNum; x++) {
			for (int y=0; y<tileNum; y++) {
				if (level < 3) { // preload all tiles up to a certain level
					tilesMap[x + y*tileNum] = new MapTile(level, x, y);
					tilesMap[x + y*tileNum]->loadFromFile();
				}
			}
		}
	}
}

MapTileZoomLevel::~MapTileZoomLevel() {
}

void MapTileZoomLevel::draw(int w, int h, double zoom, double angle1, double angle2) {
	//int minY = 0;//MapTile::lat2tiley(angle2, level);
	//int maxY = tileNum-1;//MapTile::lat2tiley(angle2, level);

	int minY = 0;
	int maxY = tileNum-1;
	int minX = 0;
	int maxX = tileNum-1;

	float zoomRatio = zoom / min(w, h);

	if (level > 2) { // for low levels always show complete globe, for higher levels compute which tiles are visible
		double wy = (h>=zoom) ? 90 : asin(h/zoom)*180/M_PI;
		double wx = (w>=zoom) ? 90 : asin(w/zoom)*180/M_PI / cos(angle2*M_PI/180);

		// approximation is a bit wrong when zoomed out much, fix this
		if (zoomRatio < 2) {
			wx *= 1.6;
			wy *= 1.6;
		} else if (zoomRatio < 4) {
			wx *= 1.45;
			wy *= 1.45;
		} else if (zoomRatio < 6) {
			wx *= 1.2;
			wy *= 1.2;
		}

		minY = MapTile::lat2tiley(angle2 + wy, level);
		maxY = MapTile::lat2tiley(angle2 - wy, level);

		if (wx < 160) {
			minX = MapTile::long2tilex(angle1 - wx, level);
			maxX = MapTile::long2tilex(angle1 + wx, level);
		}
	}

	for (int y=minY; y<=maxY; y++) {
		int x = minX;
		while (true) {
			auto mt = tilesMap.find(x + y*tileNum);
			if (mt == tilesMap.end()) { // not found
				mt = tilesMap.insert(std::pair<int,MapTile*>(x + y*tileNum, new MapTile(level, x, y))).first;
//todo: if not available, use previous level's texture
				// load
				if (!mt->second->loadFromFile()) {
					mt->second->loadFromInternet();
				}
			}
			mt->second->draw(zoom);

			if (x == maxX) break;
			x = (x+1) % tileNum;
		}
	}
}