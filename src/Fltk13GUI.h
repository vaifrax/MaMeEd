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
class MDFile;

class Fltk13GUI : public MGUI, public Fl_Window {
  public:
	Fltk13GUI(MCore* mCore);
	~Fltk13GUI();

	void openDir(std::string path); // absolute path
	void openSubDir(std::string subDirName); // relative to currently opened path
	void showFileMetaData(); // update/show new file meta data window (of selected file)
	int showWindow();
	void applyChangesOfSelectedKeyValue();
	void saveDataBase();

	//int Fltk13GUI::handle(int e);
	static void closeWindowCallback(Fl_Widget* widget, void* userData);
	static void menuCallback(Fl_Widget* widget, void* userData);
	static void buttonCallback(Fl_Widget* widget, void* userData);
	static void keyboardCallback(Fl_Widget* widget, void* userData);
	static void changeDirCallback(Fl_Widget* widget, void* userData);
	static void launchViewerCallback(Fl_Widget* widget, void* userData);
	static void resizeDragCallback(Fl_Widget* widget, void* userData);

	float widthLeftColumn, widthRightColumn; // in percent; middle column width depends on these two
	Fl_Group* mainGroup; // put everything in a group for equal resizing
	Fl_Group* pathFilesGroup; // left side

  protected:
	F13FileList* fileList;
	F13KeyValueList* keyValueList;

	Fl_Input* pathTextEdit;
	Fl_Native_File_Chooser *fileChooser;
	void showFileChooser(const char* initPath = NULL);

	Fl_Window* aboutDialog;
	void showAboutDialog();
	Fl_Window* exitNoSavingDialog;
	void showExitNoSavingDialog();

	static enum {FILE_OPEN, FILE_SAVE, FILE_EXIT_NO_SAVING, FILE_EXIT, EDIT_UNDO, HELP_ABOUT} MenuItemEnum;
	static enum {BUTTON_PATH, BUTTON_ABOUT_OK, BUTTON_NO_SAVING_OK, BUTTON_NO_SAVING_CANCEL} ButtonEnum;
	static enum {FROM_PATHTEXTEDIT} KeyboardEnum;

	Fl_Group* pathGroup; // top bar of left side

	//char* dragDropEventText; // required for drag and drop
	//static void dragDropCallbackDeferred(void* v);
};

#endif // FLTK13GUI_HEADER_INCLUDED
