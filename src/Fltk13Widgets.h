#ifndef F13WIDGETS_HEADER_INCLUDED
#define F13WIDGETS_HEADER_INCLUDED

#include "FL/Fl_Scroll.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Button.H"
#include <list>

#include <FL/fl_draw.H> // for fl_color and fl_line

class MDFile;

class StarButton : public Fl_Button {
  public:
	StarButton(int X, int Y, int W, int H, const char*L/*=0*/);
	void draw();
  protected:
	void starVertices();
};

class StarRating : public Fl_Group {
  public:
	StarRating(int X, int Y, int W, int H, const char* L=0);
	//int handle(int eventn);
	MDFile* mdf;
	void value(int v); // set value 0..5
	int value(); // get value 0..5

	static void buttonCallback(Fl_Widget* widget, void* userData);

  protected:
	int stars; // the value 0..5
	//Fl_Button* noStarButton;
	//Fl_Button* starButtons[5];
	Fl_Button* noStarButton;
	Fl_Button* starButtons[5];

	static const int STAR_BUTTON_SIZE;
};

class Fltk13Widgets : public Fl_Scroll {
  public:
	Fltk13Widgets(int X, int Y, int W, int H, const char* L=0);
	StarRating* starRating;

  protected:
};

#endif // F13WIDGETS_HEADER_INCLUDED
