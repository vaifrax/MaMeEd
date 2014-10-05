#include <FL/Fl.H>
#include "Fltk13WorldMap.h"
#include <math.h>

#include "MapTileZoomLevel.h"
#include "../tools/MLOpenGLTex.h"

#include <iostream> // purely for debugging

#ifndef M_PI
#define M_PI (3.1415926)
#endif

#include <assert.h>

//GL_INVALID_ENUM                   0x0500
//GL_INVALID_VALUE                  0x0501
//GL_INVALID_OPERATION              0x0502
//GL_STACK_OVERFLOW                 0x0503
//GL_STACK_UNDERFLOW                0x0504
//GL_OUT_OF_MEMORY                  0x0505
//GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
//#define CHECK_GL_STATE {int tmperr = glGetError(); if (tmperr != GL_NO_ERROR) {std::cout << "GL error: " << tmperr; std::cout.unsetf(std::ios::dec); std::cout.setf(std::ios::hex); std::cout << " (" << tmperr << " hex)" << std::endl; assert(0); exit(1);}}

// short version, stripped cout output
#ifndef CHECK_GL_STATE
#define CHECK_GL_STATE {int tmperr = glGetError(); if (tmperr != GL_NO_ERROR) {assert(0);}}
#endif // CHECK_GL_STATE

char* vertexShaderFileName = "../shader/Fltk13WorldMap.vertex";
char* fragmentShaderFileName = "../shader/Fltk13WorldMap.fragment";

Fltk13WorldMap::Fltk13WorldMap(int x, int y, int w, int h, char* l/*=0*/) : Fl_Gl_Window(x, y, w, h, l) {
	angle1 = 0;
	angle2 = 0;
	zoom = 200;
	showFlag = false;
	init1stTime = true;

	end();
}

Fltk13WorldMap::~Fltk13WorldMap() {
	for (int z=0; z<=18; z++) {
		delete tileLevels[z];
	}
}

// convert, save in cX, cY, cZ
void Fltk13WorldMap::longLatToXYZ(double longitude, double latitude) {
	cX = cos(latitude*M_PI/180)*cos(longitude*M_PI/180);
	cY = cos(latitude*M_PI/180)*sin(longitude*M_PI/180);
	cZ = sin(latitude*M_PI/180);
}

void Fltk13WorldMap::initGL() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w(), h());
	glOrtho(-w(), w(), -h(), h(), -400, 400);
	glRotatef(-90, 1, 0, 0);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_DEPTH_TEST);
	CHECK_GL_STATE

	if (init1stTime) {
		init1stTime = false;

		for (int z=0; z<=18; z++) {
			tileLevels[z] = new MapTileZoomLevel(z);
		}
		CHECK_GL_STATE

		// load shader
		shaderProgram = loadShader(vertexShaderFileName, fragmentShaderFileName);

		CHECK_GL_STATE
	}
}

// set a GPS marker at the specified coordinates
void Fltk13WorldMap::setFlag(double longitude, double latitude) {
	flagLongitude = longitude;
	flagLatitude = latitude;
	showFlag = true;
}
void Fltk13WorldMap::setFlag() {
	showFlag=false;
}

void Fltk13WorldMap::draw() {
	CHECK_GL_STATE

	if (!valid()) {
		// First time? init viewport, etc.
		valid(1);
		initGL();
	}

	CHECK_GL_STATE

	// Clear screen
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0, zoom, 0);
	glRotatef(angle2, 1, 0, 0);
	glRotatef(angle1, 0, 0, 1);

	// Draw white 'X'
	glColor3f(0.2, 0.2, 0.2);
	glBegin(GL_LINE_STRIP); glVertex2f(w(),  h()); glVertex2f(-w(), -h()); glEnd();
	glBegin(GL_LINE_STRIP); glVertex2f(w(), -h()); glVertex2f(-w(),  h()); glEnd();

	CHECK_GL_STATE
/*
	// Draw sphere
	glColor3f(1.0, 1.0, 1.0);
	for (int x=-180; x<180; x+=20) {
		for (int y=-180; y<180; y+=20) {
			int x2 = x+19;
			int y2 = y+19;
			glBegin(GL_QUADS);
				glVertex3f(300*cos(y *M_PI/180)*cos(x *M_PI/180), 300*cos(y *M_PI/180)*sin(x *M_PI/180), 300*sin(y *M_PI/180));
				glVertex3f(300*cos(y2*M_PI/180)*cos(x *M_PI/180), 300*cos(y2*M_PI/180)*sin(x *M_PI/180), 300*sin(y2*M_PI/180));
				glVertex3f(300*cos(y2*M_PI/180)*cos(x2*M_PI/180), 300*cos(y2*M_PI/180)*sin(x2*M_PI/180), 300*sin(y2*M_PI/180));
				glVertex3f(300*cos(y *M_PI/180)*cos(x2*M_PI/180), 300*cos(y *M_PI/180)*sin(x2*M_PI/180), 300*sin(y *M_PI/180));
			glEnd();
		}
	}
*/
//	int tileLevel = 3;
//	int tileLevel = (int) ((w()+h())/2800.0 * std::log(zoom)/std::log(2.0));
	float tileLevelF = std::log(0.03 * zoom)/std::log(2.0); // zoom is earth diameter in pixels, independent of window size
std::cout << tileLevelF << std::endl;
	int tileLevel = (int) tileLevelF;
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glDisable(GL_LIGHTING);
	CHECK_GL_STATE
	glUseProgram(shaderProgram);
	CHECK_GL_STATE
	setUniform1f(shaderProgram, "zoom", zoom);
	CHECK_GL_STATE
	MapTileZoomLevel* mtzl = tileLevels[tileLevel];
	for (int y=0; y<mtzl->tilesArraySize; y++) {
		for (int x=0; x<mtzl->tilesArraySize; x++) {
			MapTile* mt = mtzl->tilesArray[x][y];
//todo: if not available, use previous level's texture
			if (mt) {
				mt->tex->bind();
				if (tileLevel > 3) {
					glBegin(GL_QUADS);
					glTexCoord2f(0, 1);
					longLatToXYZ(mt->getLongLeft(), mt->getLatBottom());
					glVertex3f(zoom*cX, zoom*cY, zoom*cZ);

					glTexCoord2f(1, 1);
					longLatToXYZ(mt->getLongRight(), mt->getLatBottom());
					glVertex3f(zoom*cX, zoom*cY, zoom*cZ);

					glTexCoord2f(1, 0);
					longLatToXYZ(mt->getLongRight(), mt->getLatTop());
					glVertex3f(zoom*cX, zoom*cY, zoom*cZ);

					glTexCoord2f(0, 0);
					longLatToXYZ(mt->getLongLeft(), mt->getLatTop());
					glVertex3f(zoom*cX, zoom*cY, zoom*cZ);
					glEnd();
				} else {
					int subDivNum = 9 - 2*tileLevel; //4
					for (int sdy=0; sdy<subDivNum; sdy++) {
						float fsdy1 = sdy/(float) subDivNum;
						float fsdy2 = (sdy+1)/(float) subDivNum;
						float lat1 = mt->tiley2lat(mt->y+fsdy1, mt->zoomLevel);
						float lat2 = mt->tiley2lat(mt->y+fsdy2, mt->zoomLevel);
						for (int sdx=0; sdx<subDivNum; sdx++) {
							float fsdx1 = sdx/(float) subDivNum;
							float fsdx2 = (sdx+1)/(float) subDivNum;
							float long1 = mt->tilex2long(mt->x+fsdx1, mt->zoomLevel);
							float long2 = mt->tilex2long(mt->x+fsdx2, mt->zoomLevel);
							glBegin(GL_QUADS);

							glTexCoord2f(fsdx1, fsdy2);
							longLatToXYZ(long1, lat2);
							glVertex3f(zoom*cX, zoom*cY, zoom*cZ); // zoom = earth diameter in pixels

							glTexCoord2f(fsdx2, fsdy2);
							longLatToXYZ(long2, lat2);
							glVertex3f(zoom*cX, zoom*cY, zoom*cZ);

							glTexCoord2f(fsdx2, fsdy1);
							longLatToXYZ(long2, lat1);
							glVertex3f(zoom*cX, zoom*cY, zoom*cZ);

							glTexCoord2f(fsdx1, fsdy1);
							longLatToXYZ(long1, lat1);
							glVertex3f(zoom*cX, zoom*cY, zoom*cZ);

							glEnd();
						}
					}
				}
			}
		}
	}
	CHECK_GL_STATE
	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	CHECK_GL_STATE

	if (showFlag) {
		longLatToXYZ(flagLongitude, flagLatitude);
		glColor3f(1, 0, 0);
		glPointSize(3.0);
		glBegin(GL_POINTS);
		glVertex3f(zoom*cX, zoom*cY, zoom*cZ);
		glEnd();
	}

	glLoadIdentity();
	// draw crosshair
	glColor3f(0.2, 0.2, 0.2);
	glBegin(GL_LINE_STRIP); glVertex3f(0, 0, 0.1*h()); glVertex3f(0, 0, -0.1*h()); glEnd();
	glBegin(GL_LINE_STRIP); glVertex3f(0.1*w(), 0, 0); glVertex3f(-0.1*w(), 0, 0); glEnd();

}


int Fltk13WorldMap::handle(int event) {
	switch(event) {
		case FL_PUSH:
			//	if (FL::event_button() == FL_LEFT_MOUSE) ...
			//		Fl::event_x() and Fl::event_y()
			//		redraw();
			oldX = Fl::event_x();
			oldY = Fl::event_y();
			return 1;
			return 0;
		case FL_DRAG: {
			//int t = Fl::event_inside(this);
			//if (t != highlight) {
			//	highlight = t;
			//	redraw();
			//}
			int dx = Fl::event_x() - oldX;
			int dy = Fl::event_y() - oldY;

			//double df = std::log(0.01 * zoom)/std::log(2.0);
			double df = 0.07 * std::log(zoom)/std::log(2.0);
			angle1 += dx * df;
			angle2 += dy * df;

			if (angle2 < -90) angle2 = -90;
			if (angle2 > 90) angle2 = 90;
			if (angle1 < -180) angle1 += 360;
			if (angle1 > 180) angle1 -= 360;


			oldX = Fl::event_x();
			oldY = Fl::event_y();
			redraw();
			return 1; }
		//case FL_RELEASE:
		//	if (highlight) {
		//	highlight = 0;
		//	redraw();
		//	do_callback();
		//	// never do anything after a callback, as the callback
		//	// may delete the widget!
		//	}
		//	return 1;
		//case FL_SHORTCUT:
		//	if (Fl::event_key() == 'x') {
		//	do_callback();
		//	return 1;
		//	}
		//	return 0;
		case FL_MOUSEWHEEL:
			zoom *= pow(1.1, -Fl::event_dy()); // zoom = earth diameter in pixels
			redraw();
			break;
		default:
			return Fl_Gl_Window::handle(event);
	}
}
