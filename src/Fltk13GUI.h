#ifndef FLTK13GUI_HEADER_INCLUDED
#define FLTK13GUI_HEADER_INCLUDED

#include "MGUI.h"
#include <FL/Fl.H>
//#include <FL/Fl_Window.H>
#include <FL/Fl_Double_Window.H>

#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tile.H>
//#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Scroll.H>

class F13FileList;
class F13KeyValueList;
class MDFile;
class Fltk13WorldMap;
class Fltk13Widgets;
class Fltk13Preview;

class Fltk13GUI : public MGUI, public Fl_Double_Window {
  public:
	Fltk13GUI(MCore* mCore);
	~Fltk13GUI();

	void openDir(std::string path); // absolute path
void removeFilesWithLessThan4Stars();
	void openSubDir(std::string subDirName); // relative to currently opened path
	void showFileMetaData(); // update/show new file meta data window (of selected file)
	void updateFlags(); // update Flag list from world map according to selection
	void updateStars(); // update star rating according to selection
	int showWindow();
	void applyChangesOfSelectedKeyValue();
	void saveDataBase();
	void rescan(); // scan all files in directory, extracting meta data (even when .metadata.mmd already exists)

	int handle(int e);
	static void closeWindowCallback(Fl_Widget* widget, void* userData);
	static void menuCallback(Fl_Widget* widget, void* userData);
	static void buttonCallback(Fl_Widget* widget, void* userData);
	static void keyboardCallback(Fl_Widget* widget, void* userData);
	static void changeDirCallback(Fl_Widget* widget, void* userData);
	static void launchViewerCallback(Fl_Widget* widget, void* userData);
	void setGPSPosition(double lon, double lat, float radius);
	void setStarRating(int stars); // 0 to 5, to process click on stars

	void setShowThumbs(bool s) { show_thumbs_ = s; }
	bool getShowThumbs() { return show_thumbs_; }

	Fltk13WorldMap* worldMap;
	Fltk13Widgets* widgets;
	Fltk13Preview* largePreview;

	static Fltk13GUI* fgui;

	F13FileList* fileList;

  protected:
	F13KeyValueList* keyValueList;
	//Fl_Scroll* moduleColumn;

	Fl_Input* pathTextEdit;
	Fl_Native_File_Chooser *fileChooser;
	void showFileChooser(const char* initPath = NULL);

	Fl_Window* aboutDialog;
	void showAboutDialog();
	Fl_Window* exitNoSavingDialog;
	void showExitNoSavingDialog();

	static enum {FILE_OPEN, FILE_SAVE, FILE_RESCAN, FILE_EXIT_NO_SAVING, FILE_EXIT, EDIT_UNDO, VIEW_SHOW_THUMBS, VIEW_REM_STARS, HELP_ABOUT} MenuItemEnum;
	static enum {BUTTON_PATH, BUTTON_ABOUT_OK, BUTTON_NO_SAVING_OK, BUTTON_NO_SAVING_CANCEL} ButtonEnum;
	static enum {FROM_PATHTEXTEDIT} KeyboardEnum;

	Fl_Group* pathGroup; // top bar of left side
	Fl_Tile* mainGroup; // put everything in a group for equal resizing/resizable columns
	Fl_Group* pathFilesGroup; // left side

	//char* dragDropEventText; // required for drag and drop
	//static void dragDropCallbackDeferred(void* v);

	int eventRecursion;
	bool show_thumbs_;
	bool auto_save_ = true;
	int ratings_since_last_save = 0;
};

#endif // FLTK13GUI_HEADER_INCLUDED
