// MArcel's MEtadata EDitor

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <errno.h>	// errno
//#include <FL/fl_ask.H>

#include "Fltk13GUI.h"
#include "F13FileList.h"
#include "F13KeyValueList.h"

#include "MCore.h"
//#include "F13FileList.h"

#include <FL/Fl_Button.H>

#include <FL/x.H> // to set icon in windows
#include "../prj/resource.h" // for icon ID

Fltk13GUI::Fltk13GUI(MCore* mCore) : MGUI(mCore), Fl_Window(800,800,"Marcel's Metadata Editor") {
	Fl::scheme("gtk+");
	//Fl::scheme("plastic");

	fileList = NULL;
	keyValueList = NULL;

	Fl_Menu_Item menuitems[] = {
		{"&File", 0, 0, 0, FL_SUBMENU},
			{"&Open", FL_COMMAND + 'o', menuCallback, (void*) FILE_OPEN},
			{"&Save", FL_COMMAND + 's', menuCallback, (void*) FILE_SAVE},
			{"E&xit", FL_COMMAND + 'q', menuCallback, (void*) FILE_EXIT},
			{0},
		{"&Edit", 0, 0, 0, FL_SUBMENU},
			{"&Undo", FL_COMMAND + 'z', menuCallback, (void*) EDIT_UNDO, FL_MENU_DIVIDER},
			{0},
		{"&Help", 0, 0, 0, FL_SUBMENU},
			{"&About...", FL_COMMAND + 'a', menuCallback, (void*) HELP_ABOUT},
			{0},
		{0}
	};
	Fl_Menu_Bar *menu = new Fl_Menu_Bar(0, 0, 400, 25);
	menu->copy(menuitems);

	pathTextEdit = new Fl_Input(20, 50, 270, 24, "");
	//pathTextEdit->value("C:");
	pathTextEdit->callback(keyboardCallback, (void*) FROM_PATHTEXTEDIT);
	pathTextEdit->when(FL_WHEN_ENTER_KEY); // FL_WHEN_RELEASE

	Fl_Button* pathButton = new Fl_Button(300, 50, 34, 24, "Path"); // "@#menu"
	pathButton->type(FL_NORMAL_BUTTON);
	pathButton->callback(buttonCallback, (void*) BUTTON_PATH);

/*
	fileList = new FileList();
	keyValueList = new KeyValueList();

	scroll = new FileScroll(10,90,w()/3-20,h()-110);
	scroll->box(FL_BORDER_BOX);
	scroll->end();

	scroll2 = new KeyValueScroll(w()/3+3,90,w()/3-20,h()-110);
	scroll2->box(FL_BORDER_BOX);
	scroll2->end();
*/
	// Initialize the file chooser
	fileChooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	//fc->filter("Text\t*.txt\n");
	//fileChooser->preset_file(untitled_default());

	end();

	// about dialog
	aboutDialog = new Fl_Window(300, 105, "About");
	Fl_Box *aboutInfo = new Fl_Box(70, 10, 200, 25, "MaMeEd 2012-11-12"); // TODO: update date automatically
	Fl_Button *aboutOk = new Fl_Button(230, 70, 60, 25, "Ok");
	aboutOk->callback(buttonCallback, (void*) BUTTON_ABOUT_OK);
}

Fltk13GUI::~Fltk13GUI() {
}

/*static*/ void Fltk13GUI::menuCallback(Fl_Widget* widget, void* userData) {
	Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
	if (!fgui) return; // TODO: error

	switch ((int) userData) {
		case FILE_OPEN:
			fgui->showFileChooser();
			break;
		case FILE_SAVE:
			break;
		case FILE_EXIT:
			break;
		case HELP_ABOUT:
			fgui->aboutDialog->show();
			break;
	}
}

/*static*/ void Fltk13GUI::buttonCallback(Fl_Widget* widget, void* userData) {
	switch ((int) userData) {
		case BUTTON_PATH: {
			Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
			if (!fgui) return; // TODO: error
			//fgui->fileChooser->preset_file(fgui->pathTextEdit->value());
			fgui->fileChooser->directory(fgui->pathTextEdit->value());
			fgui->showFileChooser();
			} break;
		case BUTTON_ABOUT_OK:
			widget->window()->hide();
			break;
	}
}

/*static*/ void Fltk13GUI::keyboardCallback(Fl_Widget* widget, void* userData) {
	switch ((int) userData) {
		case FROM_PATHTEXTEDIT:
			Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
			if (!fgui) return; // TODO: error
			fgui->openDir(fgui->pathTextEdit->value());
			break;
	}
}

void Fltk13GUI::showFileChooser() {
	fileChooser->title("Open");
	fileChooser->type(Fl_Native_File_Chooser::BROWSE_DIRECTORY); // already existing directories
	switch (fileChooser->show()) {
	  case -1: break; // Error
	  case  1: break; // Cancel
	  default: // Choice
		//fileChooser->preset_file(fileChooser->filename());
		fileChooser->directory(fileChooser->filename());
		openDir(fileChooser->filename());
	break;
	}
}

void Fltk13GUI::openDir(std::string path) {
	pathTextEdit->value(path.c_str());
	//pathTextEdit->red

	if (mCore->openDir(path)) {
		//if (fileList) delete fileList;
		//fileList = new F13FileList(mCore->getMDDir());
	}
}

int Fltk13GUI::showWindow() {
	#ifdef WIN32
	icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));
	#endif

	show(1, argv); // do not pass on command line parameters (except exec path)
	return Fl::run();
}





/*






#include "MDDir.h"
#include "MDFile.h"
#include "MDDirDataBase.h"

#include "MDPropFile.h"



const int fixedWidth = 50;
const int defaultHeight = 25;
const std::string emptyStr("");

class FileGroup;
class KeyValueGroup;
MDPropFile* propFile; // currently displayed file
MDDirDataBase* dirDB;






class MWin : public Fl_Window {
	Fl_Native_File_Chooser *fc;
	MDDir* dir;
	Fl_Window* about_dlg;

  // Does file exist?
  int exist(const char *filename) {
	FILE *fp = fopen(filename, "r");
	if (fp) { fclose(fp); return(1); }
	else    { return(0); }
  }
  // 'Open' the file
  void open(const char *filename) {
	printf("Open '%s'\n", filename);
  }
  // 'Save' the file
  //    Create the file if it doesn't exist
  //    and save something in it.
  //
  void save(const char *filename) {
	//printf("Saving '%s'\n", filename);
	//if ( !exist(filename) ) {
	// // FILE *fp = fopen(filename, "w");				// create file if it doesn't exist
	// // if ( fp ) {
	//	//// A real app would do something useful here.
	//	//fprintf(fp, "Hello world.\n");
	//	//fclose(fp);
	// // } else {
	//	//fl_message("Error: %s: %s", filename, strerror(errno));
	// // }
	//} else {
	//  // A real app would do something useful here.
	//}
  }
  // Handle an 'Open' request from the menu
  static void open_cb(Fl_Widget *w, void *v) {
	MWin *mwin = (MWin*)v;
	mwin->fc->title("Open");
	mwin->fc->type(Fl_Native_File_Chooser::BROWSE_DIRECTORY);		// only picks directories that exist
	switch ( mwin->fc->show() ) {
	  case -1: break;	// Error
	  case  1: break; 	// Cancel
	  default:		// Choice
		mwin->fc->preset_file(mwin->fc->filename());
		mwin->openDir(mwin->fc->filename());
	break;
	}
  }
  // Handle a 'Save as' request from the menu
  //static void saveas_cb(Fl_Widget *w, void *v) {
	//MWin *mwin = (MWin*)v;
	//mwin->fc->title("Save As");
	//mwin->fc->type(Fl_Native_File_Chooser::BROWSE_SAVE_FILE);	// need this if file doesn't exist yet
	//switch ( mwin->fc->show() ) {
	//  case -1: break;	// Error
	//  case  1: break; 	// Cancel
	//  default:		// Choice
	//	mwin->fc->preset_file(mwin->fc->filename());
	//	mwin->save(mwin->fc->filename());
	//break;
	//}
  //}
  // Handle a 'Save' request from the menu
  static void save_cb(Fl_Widget *w, void *v) {
	MWin *mwin = (MWin*)v;
	if (dirDB) dirDB->writeToFile();
	//if ( strlen(mwin->fc->filename()) == 0 ) {
	//  saveas_cb(w,v);
	//} else {
	//  mwin->save(mwin->fc->filename());
	//}
  }
  static void quit_cb(Fl_Widget *w, void *v) {
	exit(0);
  }

	static void about_cb(Fl_Widget*, void* v) {
		MWin *mwin = (MWin*)v;
		mwin->about_dlg->show();
	}

	static void about_ok_cb(Fl_Widget*, void* v) {
		MWin *mwin = (MWin*)v;
		mwin->about_dlg->hide();
	}


  // Return an 'untitled' default pathname
  const char* untitled_default() {
	static char *filename = 0;
	if ( !filename ) {
	  const char *home =
		getenv("HOME") ? getenv("HOME") :		 // unix
	getenv("HOME_PATH") ? getenv("HOME_PATH") :	 // windows
	".";						 // other
	  filename = (char*)malloc(strlen(home)+20);
	  sprintf(filename, "%s/untitled.txt", home);
	}
	return(filename);
  }



public:
	MWin() : Fl_Window(800,800,"Native File Chooser Example") {
		dir = NULL;
		dirDB = NULL;
		propFile = NULL;

	}

	void openDir(const char* path) {
		if (MDDir::isDirectory(path) || MDDir::isFile(path) || (strlen(path) <=3)) { // TODO: better check for root directory
			if (dir) delete dir; // TODO: save before doing that?!
			dir = new MDDir(path);
			// todo: trigger redraw?
			if (dir) {
				pathTextEdit->value(dir->getPath().c_str());
				// fill list
				for (std::vector<MDFile*>::iterator i=dir->files.begin(); i!=dir->files.end(); ++i) {
					scroll->AddItem((*i)->getName());
				}

				dirDB = new MDDirDataBase(*dir);
			}

			if (dirDB) {
				MDPropFile& pf = dirDB->getPropFile("DSCF1182.JPG"); // TODO
				propFile = &pf; // TODO
				for (std::map<std::string, std::string>::iterator i=pf.getProperties().begin(); i!=pf.getProperties().end(); ++i) {
					scroll2->AddItem(i->first, i->second);
				}
				scroll2->AddItem(emptyStr, emptyStr);
			}
		}
	}
};
*/