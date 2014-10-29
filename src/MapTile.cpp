#include "MapTile.h"
#include "Fltk13WorldMap.h"
#include "MDDir.h" // for isDirectory()

#include <math.h>
#include <iomanip>
#include <sstream>
#include <vector>

#include "../tools/MLOpenGLTex.h"
#include "../tools/ddsfile.h"

#include <Winsock2.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// TODO
char* mapCachePath = "E:\\coding\\2012\\MarcelsMetadataEditor\\map-cache\\";


///////////////////////////////////
struct OngoingDownload {
	int x, y, level;
	CURL* httpHandle;
	FILE* outFile;
};
static std::vector<OngoingDownload> ongoingDownloads;
static CURLM* multiHandle;
///////////////////////////////////


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

bool MapTile::loadFromFile() {
	MLImage* img = loadDDS(fileName);
	if (!img) return false;

	tex = new MLOpenGLTex(img);
	delete img;
	return true;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
if (stream == NULL) return 0; // does this work?

	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void MapTile::loadFromInternet() {
	OngoingDownload od;
	od.x = x;
	od.y = y;
	od.level = zoomLevel;
	od.httpHandle = curl_easy_init();

	std::string url = std::string("http://otile4.mqcdn.com/tiles/1.0.0/osm/") + toZeroPaddedStr(zoomLevel, 2) + "/" + toZeroPaddedStr(x, 6) + "/" + toZeroPaddedStr(y, 6) + ".png";
	std::cout << url << std::endl;
	curl_easy_setopt(od.httpHandle, CURLOPT_URL, url.c_str());

	std::string outpath1 = std::string("http://otile4.mqcdn.com/tiles/1.0.0/osm/") + toZeroPaddedStr(zoomLevel, 2);

if (!MDDir::isDirectory(outpath1.c_str())) {
	CreateDirectory(outpath1.c_str(), NULL);

TODO: doesn't work?

}

	std::string outpath2 = outpath1 + "/" + toZeroPaddedStr(x, 6);

if (!MDDir::isDirectory(outpath2.c_str())) {
	CreateDirectory(outpath2.c_str(), NULL);
}

	std::string outfilename = outpath2 + "/" + toZeroPaddedStr(y, 6) + ".png"; //std::string("http://otile4.mqcdn.com/tiles/1.0.0/osm/") + toZeroPaddedStr(zoomLevel, 2) + "/" + toZeroPaddedStr(x, 6) + "/" + toZeroPaddedStr(y, 6) + ".png";
	std::cout << url << std::endl;
	od.outFile = fopen(outfilename.c_str(), "wb");
	curl_easy_setopt(od.httpHandle, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(od.httpHandle, CURLOPT_WRITEDATA, od.outFile);

	curl_multi_add_handle(multiHandle, od.httpHandle);

	ongoingDownloads.push_back(od);
	int stillRunning;
	curl_multi_perform(multiHandle, &stillRunning);
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

void MapTile::drawFromParent(double zoom) {
}

void MapTile::draw(double zoom) {
	if (!tex) {
		drawFromParent(zoom);
		return;
	}

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


////////////////////////////////////////////////////////////////////////

/*static*/ void MapTile::initCurl() {
	multiHandle = curl_multi_init();
}

/*static*/ void MapTile::deinitCurl() {
	// stop all ongoing downloads
	curl_multi_cleanup(multiHandle);
	for (auto i=ongoingDownloads.begin(); i!=ongoingDownloads.end(); ++i) {
		//curl_multi_remove_handle(multiHandle, i->httpHandle);
		curl_easy_cleanup(i->httpHandle);
		fclose(i->outFile);
	}
	ongoingDownloads.clear();
}

/*static*/ void MapTile::updateCurl() {
	struct timeval timeout;
	int rc; // select() return code

	fd_set fdread;
	fd_set fdwrite;
	fd_set fdexcep;
	int maxfd = -1;

	long curl_timeo = -1;

	FD_ZERO(&fdread);
	FD_ZERO(&fdwrite);
	FD_ZERO(&fdexcep);

	// set a suitable timeout to play around with
	timeout.tv_sec = 0;
	timeout.tv_usec = 10000;

	curl_multi_timeout(multiHandle, &curl_timeo);
	if ((curl_timeo >= 0) && (curl_timeo < 10)) {
		timeout.tv_usec = curl_timeo * 1000;
	}

	// get file descriptors from the transfers
	curl_multi_fdset(multiHandle, &fdread, &fdwrite, &fdexcep, &maxfd);

	/* In a real-world program you OF COURSE check the return code of the
	function calls.  On success, the value of maxfd is guaranteed to be
	greater or equal than -1.  We call select(maxfd + 1, ...), specially in
	case of (maxfd == -1), we call select(0, ...), which is basically equal
	to sleep. */ 
	if (maxfd < 0) return;

/*
	rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);

	switch(rc) {
	case -1:
		// select error
		return;
		break; // TODO
	case 0:
	default:
		// timeout or readable/writable sockets
		int stillRunning;
		curl_multi_perform(multiHandle, &stillRunning);
		break;
	}
*/
int stillRunning;
curl_multi_perform(multiHandle, &stillRunning);

//// transfer ready?
//curl_multi_info_read
//todo: now remove with  curl_multi_remove_handle, curl_easy_cleanup?
//todo: load texture
}
