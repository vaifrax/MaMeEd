#ifndef FLTK13GUI_HEADER_INCLUDED
#define FLTK13GUI_HEADER_INCLUDED

#include "MGUI.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>

#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Box.H>
//#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
//#include <FL/Fl_Scroll.H>

class F13FileList;
class F13KeyValueList;

class Fltk13GUI : public MGUI, public Fl_Window {
  public:
	Fltk13GUI(MCore* mCore);
	~Fltk13GUI();

	void openDir(std::string path);
	int showWindow();

	static void menuCallback(Fl_Widget* widget, void* userData);
	static void buttonCallback(Fl_Widget* widget, void* userData);
	static void keyboardCallback(Fl_Widget* widget, void* userData);

  protected:
	F13FileList* fileList;
	F13KeyValueList* keyValueList;

	Fl_Input* pathTextEdit;
	Fl_Native_File_Chooser *fileChooser;
	void showFileChooser();

	Fl_Window* aboutDialog;

	static enum {FILE_OPEN, FILE_SAVE, FILE_EXIT, EDIT_UNDO, HELP_ABOUT} MenuItemEnum;
	static enum {BUTTON_PATH, BUTTON_ABOUT_OK} ButtonEnum;
	static enum {FROM_PATHTEXTEDIT} KeyboardEnum;
};

#endif // FLTK13GUI_HEADER_INCLUDED
