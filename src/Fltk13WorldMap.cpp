#include <FL/Fl.H>
#include "Fltk13WorldMap.h"
#include <math.h>
#include <FL/Fl_Menu_Button.H>
#include "Fltk13GUI.h"

#include "MapTileZoomLevel.h"
#include "../tools/MLOpenGLTex.h"

#include <iostream> // purely for debugging with std::cout

#define MAX_LEVEL 18

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define RADSIZE (0.45)

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

/*static*/ double Fltk13WorldMap::cX, Fltk13WorldMap::cY, Fltk13WorldMap::cZ; // last result of conversion from longLatToXYZ

Fltk13WorldMap::Fltk13WorldMap(int x, int y, int w, int h, char* l/*=0*/) : Fl_Gl_Window(x, y, w, h, l) {
	angle1 = 0;
	angle2 = 0;
	zoom = 200;
	updateRadius();
	init1stTime = true;

	MapTile::initCurl();

	end();
}

Fltk13WorldMap::~Fltk13WorldMap() {
	MapTile::deinitCurl();

	for (int z=0; z<=MAX_LEVEL; z++) {
		delete tileLevels[z];
	}
}

// convert, save in cX, cY, cZ
/*static*/ void Fltk13WorldMap::longLatToXYZ(double longitude, double latitude) {
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
	updateRadius();

	if (init1stTime) {
		init1stTime = false;

		for (int z=0; z<=MAX_LEVEL; z++) {
			tileLevels[z] = new MapTileZoomLevel(z);
		}
		CHECK_GL_STATE

		// load shader
		shaderProgram = loadShader(vertexShaderFileName, fragmentShaderFileName);

		CHECK_GL_STATE
	}
}

// set a GPS marker at the specified coordinates
void Fltk13WorldMap::addFlag(double longitude, double latitude, float radius) {
	Flag f;
	f.longitude = longitude;
	f.latitude = latitude;
	f.radius = radius;
	mflags.push_back(f);
}
void Fltk13WorldMap::clearFlags() {
	mflags.clear();
}

void Fltk13WorldMap::draw() {
	MapTile::updateCurl(); // todo: move this to some other place??

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
	glRotatef((float) angle2, 1, 0, 0);
	glRotatef((float) angle1 + 90.f, 0, 0, -1);

	// Draw white 'X'
	glColor3f(0.2f, 0.2f, 0.2f);
	//glBegin(GL_LINE_STRIP); glVertex2f(w(),  h()); glVertex2f(-w(), -h()); glEnd();
	//glBegin(GL_LINE_STRIP); glVertex2f(w(), -h()); glVertex2f(-w(),  h()); glEnd();
	//glBegin(GL_LINE_STRIP); glVertex2f(-w(), 0); glVertex2f(w(), 0); glEnd();

	CHECK_GL_STATE

//	int tileLevel = 3;
//	int tileLevel = (int) ((w()+h())/2800.0 * std::log(zoom)/std::log(2.0));
//	float tileLevelF = std::log(0.03 * zoom)/std::log(2.0) * cos(angle2*M_PI/180); // zoom is earth diameter in pixels, independent of window size
//	float tileLevelF = std::log(0.1 * zoom -100)/std::log(2.0) * cos(angle2*M_PI/180); // zoom is earth diameter in pixels, independent of window size
	float tileLevelF = 1.37f*std::log(zoom) -5.8f;
	if (tileLevelF < 0.1f) tileLevelF = 0.1f;
	if (tileLevelF > MAX_LEVEL+0.5f) tileLevelF = MAX_LEVEL+0.5f;
	int tileLevel = (int) tileLevelF;
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.f, 1.f, 1.f);
	glDisable(GL_LIGHTING);
	CHECK_GL_STATE
	glUseProgram(shaderProgram);
	CHECK_GL_STATE
//	setUniform1f(shaderProgram, "zoom", zoom);
//	CHECK_GL_STATE
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(5, 1); push depth values of map tile polygons back a little so that flags will appear on top without z-fighting: didn't work yet
	tileLevels[tileLevel]->draw(w(), h(), zoom, angle1, angle2);
	//glDisable(GL_POLYGON_OFFSET_FILL);
	CHECK_GL_STATE
	glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	CHECK_GL_STATE

	if (mflags.size()) {
		float zoomRatio = zoom / min(w(), h());

		glPointSize(3.0);
		glColor4f(1, 0, 0, 2.f/mflags.size() + 0.3f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBegin(GL_POINTS);
		for (auto f=mflags.begin(); f!=mflags.end(); ++f) {
			longLatToXYZ(f->longitude, f->latitude);
/*
			if (f->radius < 10*zoomRatio) {
				// show a sphere
//				drawSolidSphere(zoom*cX, zoom*cY, zoom*cZ, zoom*f->radius);
				drawSolidSphere(zoom*cX, zoom*cY, zoom*cZ, zoom*f->radius/radius);
				//std::cout << zoom*f->radius/radius << std::endl;
//				drawSolidSphere(zoom*cX, zoom*cY, zoom*cZ, 10);
			} else {
				// show just a dot
			}
*/
			glVertex3f(zoom*cX, zoom*cY, zoom*cZ);
		}
		glEnd();
		glDisable(GL_BLEND);
	}

	glLoadIdentity();
	// draw crosshair
	//glColor3f(0.2, 0.2, 0.2);
	//glBegin(GL_LINE_STRIP); glVertex3f(0, 0, 0.1*h()); glVertex3f(0, 0, -0.1*h()); glEnd();
	//glBegin(GL_LINE_STRIP); glVertex3f(0.1*w(), 0, 0); glVertex3f(-0.1*w(), 0, 0); glEnd();

	// show radius when there are no flags to display
	if (mflags.size() == 0) {
		glColor3f(0.2, 0.2, 0.2);
		glBegin(GL_LINES);
		for (int i=0; i<16; i++) {
			float fi = i/16.f * 2.f*M_PI;
			float r = RADSIZE * min(w(), h())/2.f;
			glVertex3f(r*cos(fi), 0, r*sin(fi));
		}
		glEnd();
	}
}

void Fltk13WorldMap::drawSolidSphere(double x, double y, double z, float radius) {
	const float X = 0.525731112119133606f;
	const float Z = 0.850650808352039932f;

	glPushMatrix();
	//glTranslated(-x, -y, -z);
	glTranslated(x, y, z);
	glScalef(radius, radius, radius);
glColor4f(0, 1, 0, 1);
glBegin(GL_POINTS);
glVertex3f(0,0,0);
glEnd();
	// from OpenGLEAN code:
	glBegin(GL_POLYGON);
		glNormal3d (0, 0.525731112119, 0.850650808354);
		glVertex3d (0, 1.61803398875, 0.61803398875);
		glVertex3d (-1, 1, 1);
		glVertex3d (-0.61803398875, 0, 1.61803398875);
		glVertex3d (0.61803398875, 0, 1.61803398875);
		glVertex3d (1, 1, 1);
	glEnd();
	glBegin (GL_POLYGON);
		glNormal3d (0, 0.525731112119, -0.850650808354);
		glVertex3d (0, 1.61803398875, -0.61803398875);
		glVertex3d (1, 1, -1);
		glVertex3d (0.61803398875, 0, -1.61803398875);
		glVertex3d (-0.61803398875, 0, -1.61803398875);
		glVertex3d (-1, 1, -1);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3d (0, -0.525731112119, 0.850650808354);
		glVertex3d (0, -1.61803398875, 0.61803398875);
		glVertex3d (1, -1, 1);
		glVertex3d (0.61803398875, 0, 1.61803398875);
		glVertex3d (-0.61803398875, 0, 1.61803398875);
		glVertex3d (-1, -1, 1);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3d (0, -0.525731112119, -0.850650808354);
		glVertex3d (0, -1.61803398875, -0.61803398875);
		glVertex3d (-1, -1, -1);
		glVertex3d (-0.61803398875, 0, -1.61803398875);
		glVertex3d (0.61803398875, 0, -1.61803398875);
		glVertex3d (1, -1, -1);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3d (0.850650808354, 0, 0.525731112119);
		glVertex3d (0.61803398875, 0, 1.61803398875);
		glVertex3d (1, -1, 1);
		glVertex3d (1.61803398875, -0.61803398875, 0);
		glVertex3d (1.61803398875, 0.61803398875, 0);
		glVertex3d (1, 1, 1);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3d (-0.850650808354, 0, 0.525731112119);
		glVertex3d (-0.61803398875, 0, 1.61803398875);
		glVertex3d (-1, 1, 1);
		glVertex3d (-1.61803398875, 0.61803398875, 0);
		glVertex3d (-1.61803398875, -0.61803398875, 0);
		glVertex3d (-1, -1, 1);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3d (0.850650808354, 0, -0.525731112119);
		glVertex3d (0.61803398875, 0, -1.61803398875);
		glVertex3d (1, 1, -1);
		glVertex3d (1.61803398875, 0.61803398875, 0);
		glVertex3d (1.61803398875, -0.61803398875, 0);
		glVertex3d (1, -1, -1);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3d (-0.850650808354, 0, -0.525731112119);
		glVertex3d (-0.61803398875, 0, -1.61803398875);
		glVertex3d (-1, -1, -1);
		glVertex3d (-1.61803398875, -0.61803398875, 0);
		glVertex3d (-1.61803398875, 0.61803398875, 0);
		glVertex3d (-1, 1, -1);
	glEnd();

	glBegin(GL_POLYGON);
		glNormal3d (0.525731112119, 0.850650808354, 0);
		glVertex3d (1.61803398875, 0.61803398875, 0);
		glVertex3d (1, 1, -1);
		glVertex3d (0, 1.61803398875, -0.61803398875);
		glVertex3d (0, 1.61803398875, 0.61803398875);
		glVertex3d (1, 1, 1);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3d (0.525731112119, -0.850650808354, 0);
		glVertex3d (1.61803398875, -0.61803398875, 0);
		glVertex3d (1, -1, 1);
		glVertex3d (0, -1.61803398875, 0.61803398875);
		glVertex3d (0, -1.61803398875, -0.61803398875);
		glVertex3d (1, -1, -1);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3d (-0.525731112119, 0.850650808354, 0);
		glVertex3d (-1.61803398875, 0.61803398875, 0);
		glVertex3d (-1, 1, 1);
		glVertex3d (0, 1.61803398875, 0.61803398875);
		glVertex3d (0, 1.61803398875, -0.61803398875);
		glVertex3d (-1, 1, -1);
	glEnd();
	glBegin(GL_POLYGON);
		glNormal3d (-0.525731112119, -0.850650808354, 0);
		glVertex3d (-1.61803398875, -0.61803398875, 0);
		glVertex3d (-1, -1, -1);
		glVertex3d (0, -1.61803398875, -0.61803398875);
		glVertex3d (0, -1.61803398875, 0.61803398875);
		glVertex3d (-1, -1, 1);
	glEnd();

	glPopMatrix();
}

int Fltk13WorldMap::handle(int event) {
	switch(event) {
		case FL_PUSH:
			if (Fl::event_button() == FL_LEFT_MOUSE) {
				//		Fl::event_x() and Fl::event_y()
				//		redraw();
				oldX = Fl::event_x();
				oldY = Fl::event_y();
				return 1;
			} else if (Fl::event_button() == FL_RIGHT_MOUSE) {
char tmp[80];
time_t t = time(NULL);
sprintf(tmp, "Time is %s %i %i", ctime(&t), Fl::event_x_root(), Fl::event_y_root());
// Dynamically create menu, pop it up
//Fl_Menu_Button menu(Fl::event_x_root(), Fl::event_y_root(), 80, 1);
Fl_Menu_Button menu(0, 0, 80, 1);
menu.add(tmp);      // dynamic -- changes each time popup opens..
menu.add("Apply Position to Selection",  0, Fltk13WorldMap::Menu_CB, (void*)&menu);
menu.add("Cancel",       0, Fltk13WorldMap::Menu_CB, (void*)&menu);
menu.popup();
				return 1;
			}
			return 0;
		case FL_DRAG: {
			int dx = Fl::event_x() - oldX;
			int dy = Fl::event_y() - oldY;

			double df = 200.0 / zoom;//std::log(zoom)/std::log(2.0);
//std::cout << zoom << "   " << df << std::endl;
			angle1 -= dx * df;
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
			zoom *= pow(1.1f, -Fl::event_dy()); // zoom = earth diameter in pixels
			updateRadius();
			redraw();
			return 1; // event consumed
		default:
			return Fl_Gl_Window::handle(event);
	}
}

void Fltk13WorldMap::updateRadius() {
	radius = 6371000.f * RADSIZE * 0.5f*min(w(), h()) / zoom;
}

/*static*/ void Fltk13WorldMap::Menu_CB(Fl_Widget* widget, void *data) {
	char name[80];
	((Fl_Menu_Button*)data)->item_pathname(name, sizeof(name)-1);
	fprintf(stderr, "Menu Callback: %s\n", name);

	//Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->top_window());
	//if (!fgui) return; // TODO: error
	if (std::string(name) == "/Apply Position to Selection") {
		Fltk13GUI::fgui->setGPSPosition(Fltk13GUI::fgui->worldMap->angle1, Fltk13GUI::fgui->worldMap->angle2, Fltk13GUI::fgui->worldMap->radius);
	}
}