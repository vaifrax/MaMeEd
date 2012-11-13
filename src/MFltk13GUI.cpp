// MArcel's MEtadata EDitor

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>	// errno
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Scroll.H>
#include <FL/fl_ask.H>

#include "MFltk13GUI.h"


MFltk13GUI::MFltk13GUI(MCore* mCore) : MGUI(mCore), Fl_Window(800,800,"MaMeEd") {
	Fl::scheme("gtk+");
	//Fl::scheme("plastic");
}

MFltk13GUI::~MFltk13GUI() {
}

void MFltk13GUI::openDir(std::string) {
}

int MFltk13GUI::showWindow() {
	show(1, argv); // do not pass on command line parameters (except exec path)
	return Fl::run();
}












#include "MDDir.h"
#include "MDFile.h"
#include "MDDirDataBase.h"

#include "MDPropFile.h"



const int fixedWidth = 50;
const int defaultHeight = 25;
const std::string emptyStr("");

class FileGroup;
class KeyValueGroup;
FileGroup* selectedFile = NULL;
KeyValueGroup* selectedKeyValue = NULL;
MDPropFile* propFile; // currently displayed file
MDDirDataBase* dirDB;

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
class FileGroup : public Fl_Group {
	Fl_Box *fixedBox;
	Fl_Box *stretchBox;
public:
	std::string getFileName() {return stretchBox->label();}

	FileGroup(int X, int Y, int W, int H, const char* fileName, const char* L=0) : Fl_Group(X,Y,W,H,L) {
		begin();
			// Fixed width box
			fixedBox = new Fl_Box(X,Y,fixedWidth,40,"Fixed");
			fixedBox->box(FL_UP_BOX);
			// Stretchy box
			stretchBox = new Fl_Box(X+fixedWidth,Y,W-fixedWidth,defaultHeight, fileName);
			//stretchBox->box(FL_UP_BOX);
			resizable(stretchBox);
		end();
	}

	int handle(int eventn) {
		switch (eventn) {
			case FL_PUSH:
			case FL_RELEASE:
			case FL_DRAG:
			case FL_MOVE:
				//return handle_mouse(eventn,Fl::event_button(), Fl::event_x(),Fl::event_y());
			case FL_FOCUS:
				//label ("Gained focus");
				if (this != selectedFile) {
					if (selectedFile) { // previously selected file
						selectedFile->color(0xDDDDDD00); // TODO
						selectedFile->box(FL_FLAT_BOX);
						selectedFile->redraw();
						//selectedFile->box(FL_NO_BOX);
						//selectedFile->window()->redraw(); // redraw parent because this is transparent :-( -> performance not optimal
					}
					this->color(0xFF000000);
					this->box(FL_UP_BOX);
					redraw();
					selectedFile = this;
				}
				return 1;
			case FL_UNFOCUS:
				//label ("Lost focus");
				//damage(1);
				return 1;
			default:
				//return Fl_Window::handle(eventn);
				return 0;
		};
	}
};

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
class KeyValueGroup : public Fl_Group {
	//Fl_Box *fixedBox;
	//Fl_Box *stretchBox;
	Fl_Input *fixedBox;
	Fl_Input *stretchBox;
public:
	std::string getKey() {return fixedBox->value();}
	std::string getValue() {return stretchBox->value();}

	KeyValueGroup(int X, int Y, int W, int H, const char* key, const char* value, const char* L=0) : Fl_Group(X,Y,W,H,L) {
		begin();
			// Fixed width box
			//fixedBox = new Fl_Box(X,Y,fixedWidth,25,key);
			fixedBox = new Fl_Input(X,Y,fixedWidth,25,"");
			fixedBox->value(key);
			//fixedBox->box(FL_BORDER_FRAME);
			fixedBox->box(FL_FLAT_BOX);
			//fixedBox->color(0x99999900);
fixedBox->color(0xFFFFFF00);
			// Stretchy boxKeyValueScroll
			//stretchBox = new Fl_Box(X+fixedWidth,Y,W-fixedWidth,defaultHeight, value);
			stretchBox = new Fl_Input(X+fixedWidth+1,Y,W-fixedWidth-1,defaultHeight);
			stretchBox->value(value);
			//stretchBox->box(FL_BORDER_FRAME);
			stretchBox->box(FL_FLAT_BOX);
			//stretchBox->color(0x99999900);
stretchBox->color(0xFFFFFF00);
			resizable(stretchBox);
		end();
	}
	int handle(int eventn) {
		switch (eventn) {
			//case FL_PUSH:
			//case FL_RELEASE:
			//case FL_DRAG:
			//case FL_MOVE:
				//return handle_mouse(eventn,Fl::event_button(), Fl::event_x(),Fl::event_y());
			case FL_FOCUS:
				//label ("Gained focus");
				selectedKeyValue = this;
if (selectedFile) {
	MDPropFile& pf = dirDB->getPropFile(selectedFile->getFileName()); // TODO
	propFile = &pf; // TODO
}
				//return Fl_Window::handle(eventn); //return 1;
				return Fl_Group::handle(eventn);
			case FL_UNFOCUS:
				//label ("Lost focus");
				//damage(1);
// TODO: also do this before saving, or at return; maybe add a new empty key value pair at the end
				propFile->addKeyValue(selectedKeyValue->fixedBox->value(), selectedKeyValue->stretchBox->value());
// TODO: how to remove?
				return 1;
			default:
				//return Fl_Window::handle(eventn);
				return 0;
		};
	}
};

// Custom scroll that tells children to follow scroll's width when resized
class FileScroll : public Fl_Scroll {
	int nchild;
public:
	FileScroll(int X, int Y, int W, int H, const char* L=0) : Fl_Scroll(X,Y,W,H,L) {
		nchild = 0;
	}
	void resize(int X, int Y, int W, int H) {
		// Tell children to resize to our new width
		for ( int t=0; t<nchild; t++ ) {
			Fl_Widget *w = child(t);
			w->resize(w->x(), w->y(), W-20, w->h());    // W-20: leave room for scrollbar
		}
		// Tell scroll children changed in size
		init_sizes();
		Fl_Scroll::resize(X,Y,W,H);
	}
	
	// Append new FileGroup to bottom
	//     Note: An Fl_Pack would be a good way to do this, too
	//
	void AddItem(std::string const& fileName) {
		int X = x() + 1,
			Y = y() - yposition() + (nchild*41) + 1,
			W = w() - 20,                           // -20: compensate for vscroll bar
			H = 41;
		add(new FileGroup(X,Y,W,H, fileName.c_str()));
		redraw();
		nchild++;
	}
};

// Custom scroll that tells children to follow scroll's width when resized
class KeyValueScroll : public Fl_Scroll {
	int nchild;
public:
	KeyValueScroll(int X, int Y, int W, int H, const char* L=0) : Fl_Scroll(X,Y,W,H,L) {
		nchild = 0;
	}
	void resize(int X, int Y, int W, int H) {
		// Tell children to resize to our new width
		for ( int t=0; t<nchild; t++ ) {
			Fl_Widget *w = child(t);
			w->resize(w->x(), w->y(), W-20, w->h());    // W-20: leave room for scrollbar
		}
		// Tell scroll children changed in size
		init_sizes();
		Fl_Scroll::resize(X,Y,W,H);
	}
	
	// Append new FileGroup to bottom
	//     Note: An Fl_Pack would be a good way to do this, too
	//
	void AddItem(std::string const& key, std::string const& value) {
		int X = x() + 1,
			Y = y() - yposition() + (nchild*26) + 1,
			W = w() - 20,                           // -20: compensate for vscroll bar
			H = 25;
		add(new KeyValueGroup(X,Y,W,H, key.c_str(), value.c_str()));
		redraw();
		nchild++;
	}
};




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


Fl_Input* pathTextEdit;
public:
FileScroll *scroll;
KeyValueScroll *scroll2;

public:
	MWin() : Fl_Window(800,800,"Native File Chooser Example") {
		dir = NULL;
		dirDB = NULL;
		propFile = NULL;

		Fl_Menu_Item menuitems[] = {
			{ "&File", 0, 0, 0, FL_SUBMENU },
				{ "&Options", 0, (Fl_Callback *)open_cb },
				{ "&Save", FL_COMMAND + 'o', (Fl_Callback *)save_cb },
				{ "E&xit", FL_COMMAND + 'q', (Fl_Callback *)quit_cb, 0 },
				{ 0 },
			{ "&Edit", 0, 0, 0, FL_SUBMENU },
				{ "&Undo", FL_COMMAND + 'z', (Fl_Callback *)quit_cb, 0, FL_MENU_DIVIDER },
				{ 0 },
			{ "&Help", 0, 0, 0, FL_SUBMENU },
				{ "&About...", FL_COMMAND + 'a', (Fl_Callback *)about_cb },
				{ 0 },
			{ 0 }
		};
		Fl_Menu_Bar *menu = new Fl_Menu_Bar(0,0,400,25);
		menu->copy(menuitems, (void*)this);

		pathTextEdit = new Fl_Input(20, 50, 270, 24, "");
		//pathTextEdit->value("C:");

		Fl_Button* pathButton = new Fl_Button(300, 50, 34, 24, "Path"); // "@#menu"
		pathButton->type(FL_NORMAL_BUTTON);
	    pathButton->callback(open_cb, (void*)this);

		scroll = new FileScroll(10,90,w()/3-20,h()-110);
		scroll->box(FL_BORDER_BOX);
		scroll->end();

		scroll2 = new KeyValueScroll(w()/3+3,90,w()/3-20,h()-110);
		scroll2->box(FL_BORDER_BOX);
		scroll2->end();

		// Initialize the file chooser
		fc = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
		//fc->filter("Text\t*.txt\n");
		fc->preset_file(untitled_default());
		end();

		// about dialog
		about_dlg = new Fl_Window(300, 105, "About");
		Fl_Box *about_info = new Fl_Box(70, 10, 200, 25, "MaMeEd 2012-11-12");
		Fl_Button *about_ok = new Fl_Button(230, 70, 60, 25, "Ok");
		about_ok->callback(about_ok_cb);
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
