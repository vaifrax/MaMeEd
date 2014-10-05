#ifndef FLTK13WORLDMAP_HEADER_INCLUDED
#define FLTK13WORLDMAP_HEADER_INCLUDED

#include "../tools/GLee.h"
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include "../tools/shader.h"

class MapTileZoomLevel;

class Fltk13WorldMap : public Fl_Gl_Window {
  public:
	Fltk13WorldMap(int x, int y, int w, int h, char* l=0);
	~Fltk13WorldMap();
	int handle(int event);
	void setFlag(double longitude, double latitude); // set a GPS marker at the specified coordinates
	void setFlag(); // remove flag

  protected:
	void initGL();
	void draw();
	double angle1, angle2; // globe rotation
	float zoom; // zoom = earth diameter in pixels

	void longLatToXYZ(double longitude, double latitude); // convert, save in cX, cY, cZ
	double cX, cY, cZ; // results from last computation

	MapTileZoomLevel* tileLevels[19];
	GLuint shaderProgram;

  private:
	int oldX, oldY;
	double flagLongitude, flagLatitude;
	bool showFlag;
	bool init1stTime;
};

#endif // FLTK13WORLDMAP_HEADER_INCLUDED
