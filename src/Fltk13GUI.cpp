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
#include "MDDir.h"

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
		if (fileList) {
			remove(fileList); // remove from window
			delete fileList;
		}
		if (keyValueList) {
			remove(keyValueList);
			delete keyValueList;
			keyValueList = NULL;
		}

		fileList = new F13FileList(10,80,200,500,mCore->getMDDir()); // TODO
		add(fileList); // add to window

		//std::string const& selFileName = mCore->getMDDir()->getInitiallySelectedFileName();
		//if (selFileName.size()) {
		//	keyValueList = new F13KeyValueList(250, 30, 200, 600, selFileName);
		//}
	}
}

int Fltk13GUI::showWindow() {
	#ifdef WIN32
	icon((char*)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));
	#endif

	show(1, argv); // do not pass on command line parameters (except exec path)
	return Fl::run();
}

void Fltk13GUI::showFileMetaData() {
	if (keyValueList) {
		remove(keyValueList);
		Fl::delete_widget(keyValueList); // todo: instead of deleting, clearing and refilling it should be good, too?!?
		keyValueList = NULL;
	}

	if (!fileList) return;

	MDDir const* mddir = mCore->getMDDir();
	if (!mddir) return;
	MDFile* mdfile = mddir->getMDFile(fileList->getSelectedFileName());
	if (!mdfile) return;

	keyValueList = new F13KeyValueList(350, 30, 200, 600, mdfile);
	add(keyValueList);
	keyValueList->redraw();
}
