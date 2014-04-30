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

  protected:
	void initGL();
	void draw();
	float angle1, angle2; // globe rotation
	float zoom;

	MapTileZoomLevel* tileLevels[19];
	GLuint shaderProgram;

  private:
	int oldX, oldY;
};

#endif // FLTK13WORLDMAP_HEADER_INCLUDED
