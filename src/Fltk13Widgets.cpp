#include "Fltk13Widgets.h"
#include "Fltk13GUI.h"
#include "MDFile.h"
#include "MDProperty.h"
#include "FL/Fl_Pack.H"

#include <iostream> // for cout, debugging only!?

StarButton::StarButton(int X, int Y, int W, int H, const char*L=0) : Fl_Button(X,Y,W,H,L) {
	// set a style that fills background, so that when changing to draw less stars everything looks ok
	box(FL_FLAT_BOX);
	color(50);
}

void StarButton::starVertices() {
	float mx = w();
	float my = h();
	int tx = x();
	int ty = y();
	fl_vertex(0.47f*mx + tx, 0.94f*my + ty);
	fl_vertex(0.62f*mx + tx, 0.64f*my + ty);
	fl_vertex(0.96f*mx + tx, 0.66f*my + ty);
	fl_vertex(0.71f*mx + tx, 0.42f*my + ty);
	fl_vertex(0.83f*mx + tx, 0.11f*my + ty);
	fl_vertex(0.54f*mx + tx, 0.27f*my + ty);
	fl_vertex(0.28f*mx + tx, 0.05f*my + ty);
	fl_vertex(0.33f*mx + tx, 0.39f*my + ty);
	fl_vertex(0.05f*mx + tx, 0.57f*my + ty);
	fl_vertex(0.38f*mx + tx, 0.62f*my + ty);
}

void StarButton::draw() {
	fl_color(FL_BACKGROUND_COLOR);
	fl_rectf(x(), y(), x()+w()-1, y()+h()-1);

	if (!active_r()) {
		// inactive
		fl_color(210, 210, 210);
		fl_begin_loop();
		starVertices();
		fl_end_loop();
		return;
	}

	if (value() == 0) {
		// not set
		fl_color(127, 127, 127);
		fl_begin_loop();
		starVertices();
		fl_end_loop();
		return;
	}

	// set
	fl_color(255, 200, 0);
	fl_begin_complex_polygon();
	starVertices();
	fl_end_complex_polygon();
	fl_color(127, 127, 127);
	fl_begin_loop();
	starVertices();
	fl_end_loop();
}

/*static*/ const int StarRating::STAR_BUTTON_SIZE = 24;

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
StarRating::StarRating(int X, int Y, int W, int H, const char* L/*=0*/) : Fl_Group(X,Y,W,H,L) {
	this->mdf = NULL;

	begin();
	int posX = 6;
	int posY = 6;
	noStarButton = new Fl_Button(posX, posY, STAR_BUTTON_SIZE/2, STAR_BUTTON_SIZE);
noStarButton->label("0");
	noStarButton->callback(StarRating::buttonCallback, 0);
	posX += STAR_BUTTON_SIZE/2 +1;
	for (int i=0; i<5; i++) {
		//starButtons[i] = new Fl_Button(posX, posY, STAR_BUTTON_SIZE, STAR_BUTTON_SIZE);
		starButtons[i] = new StarButton(posX, posY, STAR_BUTTON_SIZE, STAR_BUTTON_SIZE);
		//starButtons[i]->label("*");
		starButtons[i]->callback(StarRating::buttonCallback, (void*) (i+1));
		posX += STAR_BUTTON_SIZE +1;
	}
	end();
}

/*static*/ void StarRating::buttonCallback(Fl_Widget* widget, void* userData) {
	Fltk13GUI::fgui->setStarRating((int) userData);
	((StarRating*) widget->parent())->value((int) userData);
}

// set value 0..5
void StarRating::value(int v) {
	stars = v;
	for (int i=0; i<5; i++) {
		starButtons[i]->value( i<v ? 1 : 0 );
	}
}

// get value 0..5
int StarRating::value() {
	return stars;
}

/*
int StarRating::handle(int eventn) {
//Fl_Group::handle(eventn);
//std::cout << eventn << std::endl;
	switch (eventn) {
		case FL_PUSH:
			return 1; // to enable FL_RELEASE
		case FL_RELEASE:
			{
			//label ("Gained focus");
			//Fltk13GUI::fgui->showFileMetaData();
			return Fl_Group::handle(eventn);
			}
		default:
			return Fl_Group::handle(eventn);
	};
};
*/
////////////////////////////////////////////////////////////////////////////////////

Fltk13Widgets::Fltk13Widgets(int X, int Y, int W, int H, const char* L/*=0*/) : Fl_Scroll(X,Y,W,H,L) {
	begin();

	Fl_Pack* pack = new Fl_Pack(X, Y, W-16, H-16);
	pack = pack;
	pack->box(FL_DOWN_FRAME);
	starRating = new StarRating(0, 0, 180, 40);
	starRating->deactivate(); // no active selection at start
	pack->end();

	//resizable(pack);

	end();
}
