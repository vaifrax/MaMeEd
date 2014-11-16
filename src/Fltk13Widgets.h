#ifndef F13WIDGETS_HEADER_INCLUDED
#define F13WIDGETS_HEADER_INCLUDED

#include "FL/Fl_Scroll.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Button.H"
#include <list>

class MDFile;

class StarRating : public Fl_Group {
  public:
	StarRating(int X, int Y, int W, int H, const char* L=0);
	//int handle(int eventn);
	MDFile* mdf;

	static void buttonCallback(Fl_Widget* widget, void* userData);

  protected:
	Fl_Button* noStarButton;
	Fl_Button* starButtons[5];

	static const int STAR_BUTTON_SIZE;
};

class Fltk13Widgets : public Fl_Scroll {
  public:
	Fltk13Widgets(int X, int Y, int W, int H, const char* L=0);

  protected:
	StarRating* starRating;
};

#endif // F13WIDGETS_HEADER_INCLUDED
