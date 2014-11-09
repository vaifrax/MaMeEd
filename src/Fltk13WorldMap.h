#ifndef FLTK13WORLDMAP_HEADER_INCLUDED
#define FLTK13WORLDMAP_HEADER_INCLUDED

#include "../tools/GLee.h"
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include "../tools/shader.h"
#include <vector>

class MapTileZoomLevel;

class Fltk13WorldMap : public Fl_Gl_Window {
  public:
	Fltk13WorldMap(int x, int y, int w, int h, char* l=0);
	~Fltk13WorldMap();
	int handle(int event);
	void addFlag(double longitude, double latitude, float radius); // set a GPS marker at the specified coordinates
	void clearFlags(); // remove flag

	static void longLatToXYZ(double longitude, double latitude); // convert, save in cX, cY, cZ
	static double cX, cY, cZ; // results from last computation

  protected:
	void initGL();
	void draw();
	static void Menu_CB(Fl_Widget*, void *data);

	double angle1, angle2; // globe rotation
	float zoom; // zoom = earth diameter in pixels
	float radius; // automatically computed; radius of visible map area
	void updateRadius();

	MapTileZoomLevel* tileLevels[19];
	GLuint shaderProgram;

  private:
	int oldX, oldY;

	struct Flag {
		double longitude, latitude;
		float radius;
	};
	std::vector<Fltk13WorldMap::Flag> mflags;

	bool init1stTime;
};

#endif // FLTK13WORLDMAP_HEADER_INCLUDED
