#include "Fltk13Widgets.h"
#include "Fltk13GUI.h"
#include "MDFile.h"
#include "MDProperty.h"
#include "FL/Fl_Pack.H"

#include <iostream> // for cout, debugging only!?

/*static*/ const int StarRating::STAR_BUTTON_SIZE = 24;

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
StarRating::StarRating(int X, int Y, int W, int H, const char* L/*=0*/) : Fl_Group(X,Y,W,H,L) {
	this->mdf = NULL;

	begin();
	int posX = 6;
	int posY = 6;
	noStarButton = new Fl_Button(posX, posY, STAR_BUTTON_SIZE/2, STAR_BUTTON_SIZE);
	noStarButton->callback(StarRating::buttonCallback, 0);
	posX += STAR_BUTTON_SIZE/2 +1;
	for (int i=0; i<5; i++) {
		starButtons[i] = new Fl_Button(posX, posY, STAR_BUTTON_SIZE, STAR_BUTTON_SIZE);
		starButtons[i]->label("*");
		starButtons[i]->callback(StarRating::buttonCallback, (void*) (i+1));
		posX += STAR_BUTTON_SIZE +1;
	}
	end();
}

/*static*/ void StarRating::buttonCallback(Fl_Widget* widget, void* userData) {
	Fltk13GUI::fgui->setStarRating((int) userData);
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
	pack->end();

	//resizable(pack);

	end();
}
