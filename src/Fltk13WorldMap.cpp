#include <FL/Fl.H>
#include "Fltk13WorldMap.h"
#include <math.h>

#include "MapTileZoomLevel.h"
#include "../tools/MLOpenGLTex.h"

#ifndef M_PI
#define M_PI (3.1415926)
#endif

Fltk13WorldMap::Fltk13WorldMap(int x, int y, int w, int h, char* l/*=0*/) : Fl_Gl_Window(x, y, w, h, l) {
	angle1 = 0;
	angle2 = 0;
	zoom = 200;

	end();
}

Fltk13WorldMap::~Fltk13WorldMap() {
	for (int z=0; z<=18; z++) {
		delete tileLevels[z];
	}
}

void Fltk13WorldMap::draw() {
	if (!valid()) {
		// First time? init viewport, etc.
		valid(1);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, w(), h());
		glOrtho(-w(), w(), -h(), h(), -400, 400);
		glRotatef(-90, 1, 0, 0);
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_DEPTH_TEST);

		for (int z=0; z<=18; z++) {
			tileLevels[z] = new MapTileZoomLevel(z);
		}
	}
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
	int rLevel = 2;
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	glDisable(GL_LIGHTING);
	MapTileZoomLevel* mtzl = tileLevels[rLevel];
	for (int y=0; y<mtzl->tilesArraySize; y++) {
		for (int x=0; x<mtzl->tilesArraySize; x++) {
			MapTile* mt = mtzl->tilesArray[x][y];
			mt->tex->bind();
			glBegin(GL_QUADS);
				glTexCoord2f(0, 1);
				glVertex3f(zoom*cos(mt->getLatBottom()*M_PI/180)*cos(mt->getLongLeft()*M_PI/180), zoom*cos(mt->getLatBottom()*M_PI/180)*sin(mt->getLongLeft()*M_PI/180), zoom*sin(mt->getLatBottom()*M_PI/180));
				glTexCoord2f(1, 1);
				glVertex3f(zoom*cos(mt->getLatBottom()*M_PI/180)*cos(mt->getLongRight()*M_PI/180), zoom*cos(mt->getLatBottom()*M_PI/180)*sin(mt->getLongRight()*M_PI/180), zoom*sin(mt->getLatBottom()*M_PI/180));
				glTexCoord2f(1, 0);
				glVertex3f(zoom*cos(mt->getLatTop()*M_PI/180)*cos(mt->getLongRight()*M_PI/180), zoom*cos(mt->getLatTop()*M_PI/180)*sin(mt->getLongRight()*M_PI/180), zoom*sin(mt->getLatTop()*M_PI/180));
				glTexCoord2f(0, 0);
				glVertex3f(zoom*cos(mt->getLatTop()*M_PI/180)*cos(mt->getLongLeft()*M_PI/180), zoom*cos(mt->getLatTop()*M_PI/180)*sin(mt->getLongLeft()*M_PI/180), zoom*sin(mt->getLatTop()*M_PI/180));
			glEnd();
		}
	}
	glDisable(GL_TEXTURE_2D);

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

			angle1 += dx;
			angle2 += dy;

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
			zoom *= pow(1.1, Fl::event_dy());
			redraw();
			break;
		default:
			return Fl_Gl_Window::handle(event);
	}
}
