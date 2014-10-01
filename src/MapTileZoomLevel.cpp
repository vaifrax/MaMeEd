#include "MapTileZoomLevel.h"

MapTileZoomLevel::MapTileZoomLevel(int level) {
	this->level = level;
	tilesArraySize = 0;

	if (level <= 5) {
		tilesArraySize = 1 << level;

		tilesArray = new MapTile**[tilesArraySize];
		for (int x=0; x<tilesArraySize; x++) {
			tilesArray[x] = new MapTile*[tilesArraySize];
			for (int y=0; y<tilesArraySize; y++) {
				if (level <= 3) {
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
