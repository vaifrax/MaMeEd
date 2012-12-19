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

#include <FL/Fl_Shared_Image.H>

#include <FL/x.H> // to set icon in windows
#include "../prj/resource.h" // for icon ID


Fltk13GUI::Fltk13GUI(MCore* mCore) : MGUI(mCore), Fl_Window(800,800,"Marcel's Metadata Editor") {
	Fl::scheme("gtk+");
	//Fl::scheme("plastic");

	fileList = NULL;
	keyValueList = NULL;

	callback(closeWindowCallback, this);

	fl_register_images(); // init image lib

	Fl_Menu_Item menuitems[] = {
		{"&File", 0, 0, 0, FL_SUBMENU},
			{"&Open", FL_COMMAND + 'o', menuCallback, (void*) FILE_OPEN},
			{"&Save", FL_COMMAND + 's', menuCallback, (void*) FILE_SAVE},
			{"Exit without saving", FL_COMMAND + 'q', menuCallback, (void*) FILE_EXIT_NO_SAVING},
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

	const int MENU_HEIGHT = 25;
	Fl_Menu_Bar *menu = new Fl_Menu_Bar(0, 0, w(), MENU_HEIGHT);
	menu->copy(menuitems);

	// main group, containing all elements in the window
	const int MAIN_GROUP_PADDING = 2;
	mainGroup = new Fl_Tile(MAIN_GROUP_PADDING, MENU_HEIGHT+MAIN_GROUP_PADDING, w()-2*MAIN_GROUP_PADDING, h()-MENU_HEIGHT-2*MAIN_GROUP_PADDING); // put everything in a group for equal resizing
		pathFilesGroup = new Fl_Group(mainGroup->x(), mainGroup->y(), 0.4*mainGroup->w(), mainGroup->h()); // left side
			box(FL_DOWN_BOX);
			pathGroup = new Fl_Group(pathFilesGroup->x(), pathFilesGroup->y(), pathFilesGroup->w(), 25); // top bar of left side
				const int PATH_BUTTON_WIDTH = 35;
				pathTextEdit = new Fl_Input(pathGroup->x(), pathGroup->y(), pathGroup->w()-PATH_BUTTON_WIDTH-2, pathGroup->h());
				//pathTextEdit->value("C:");
				pathTextEdit->callback(keyboardCallback, (void*) FROM_PATHTEXTEDIT);
				pathTextEdit->when(FL_WHEN_ENTER_KEY); // FL_WHEN_RELEASE

				Fl_Button* pathButton = new Fl_Button(pathGroup->x()+pathGroup->w()-PATH_BUTTON_WIDTH, pathGroup->y(), PATH_BUTTON_WIDTH, pathGroup->h(), "Path");
				pathButton->type(FL_NORMAL_BUTTON);
				pathButton->callback(buttonCallback, (void*) BUTTON_PATH);
			pathGroup->end();
			pathGroup->resizable(pathTextEdit);
		pathFilesGroup->end();
		keyValueList = new F13KeyValueList(mainGroup->x() + 0.4*mainGroup->w(), mainGroup->y(), 0.3*mainGroup->w(), mainGroup->h(), mCore->getConfig());
			box(FL_DOWN_BOX);
		keyValueList->end();
		Fl_Scroll* fls = new Fl_Scroll(mainGroup->x() + 0.7*mainGroup->w(), mainGroup->y(), 0.3*mainGroup->w(), mainGroup->h(), "test");
			box(FL_DOWN_BOX);
		fls->end();
//pathFilesGroup->resizable(fileList);
	mainGroup->end();
	resizable(mainGroup);
	end(); //////////////////////

	// dialogs
	fileChooser = NULL;
	aboutDialog = NULL;
	exitNoSavingDialog = NULL;
}

Fltk13GUI::~Fltk13GUI() {
	applyChangesOfSelectedKeyValue();
}

void Fltk13GUI::applyChangesOfSelectedKeyValue() {
	if (keyValueList) keyValueList->applyChangesOfSelectedKeyValue();
}

void Fltk13GUI::saveDataBase() {
	if (!keyValueList) return;
	keyValueList->applyChangesOfSelectedKeyValue();
	mCore->getMDDir()->writeToFile();
}


//int Fltk13GUI::handle(int e) {
//	switch (e) {
//		case FL_PASTE:
//			// make a copy of the DND payload
//			int evtLen = Fl::event_length();
//			if (dragDropEventText) delete[] dragDropEventText;
//			dragDropEventText = new char[evtLen];
//			strcpy(dragDropEventText, Fl::event_text());
//			// If there is a callback registered, call it.
//			// The callback must access Fl::event_text() to
//			// get the string or file path that was dropped.
//			// Note that do_callback() is not called directly.
//			// Instead it will be executed by the FLTK main-loop
//			// once we have finished handling the DND event.
//			// This allows caller to popup a window or change widget focus.
//			if(callback() && ((when() & FL_WHEN_RELEASE) || (when() & FL_WHEN_CHANGED)))
//				Fl::add_timeout(0.0, dragDropCallbackDeferred, (void*)this);
//			return 1;
//	}
//}
//
///*static*/ void dragDropCallbackDeferred(void* v) {
//	assert(dynamic_cast<Fltk13GUI*> v);
//	Fltk13GUI* fgui = (Fltk13GUI*) v;
//	fgui->openDir(dragDropEventText);
//	delete[] dragDropEventText;
//}

// called when pressing 'Esc' or closing window via 'X' button
/*static*/ void Fltk13GUI::closeWindowCallback(Fl_Widget* widget, void* userData) {
	Fltk13GUI* fgui = (Fltk13GUI*) userData;

	// always save before exiting
	fgui->applyChangesOfSelectedKeyValue();
	fgui->saveDataBase(); // auto-save on exit
	exit(0);
}

/*static*/ void Fltk13GUI::menuCallback(Fl_Widget* widget, void* userData) {
	Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
	if (!fgui) return; // TODO: error

	switch ((int) userData) {
		case FILE_OPEN:
			fgui->applyChangesOfSelectedKeyValue();
			fgui->showFileChooser();
			break;
		case FILE_SAVE:
			//fgui->applyChangesOfSelectedKeyValue();
			fgui->saveDataBase();
			break;
		case FILE_EXIT_NO_SAVING:
			fgui->showExitNoSavingDialog();
			break;
		case FILE_EXIT:
			fgui->applyChangesOfSelectedKeyValue();
			fgui->saveDataBase(); // auto-save on exit
			exit(0);
			//break;
		case HELP_ABOUT:
			fgui->showAboutDialog();
			break;
	}
}

/*static*/ void Fltk13GUI::buttonCallback(Fl_Widget* widget, void* userData) {
	switch ((int) userData) {
		case BUTTON_PATH: {
			Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
			if (!fgui) return; // TODO: error
			//fgui->fileChooser->preset_file(fgui->pathTextEdit->value());
			fgui->showFileChooser(fgui->pathTextEdit->value());
			} break;
		case BUTTON_ABOUT_OK:
			widget->window()->hide();
			break;

		case BUTTON_NO_SAVING_OK:
			exit(0);
			break;
		case BUTTON_NO_SAVING_CANCEL:
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

// callback for the button next to a directory in fileList
/*static*/ void Fltk13GUI::changeDirCallback(Fl_Widget* widget, void* userData) {
	const char* dirName = (const char*) userData;
	Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
	if (!fgui) return; // TODO: error

	fgui->openSubDir(dirName);
}

/*static*/ void Fltk13GUI::launchViewerCallback(Fl_Widget* widget, void* userData) {
	const char* fileName = (const char*) userData;
	Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
	if (!fgui) return; // TODO: error

	std::string currentPath(fgui->mCore->getMDDir()->getDirPath());
	std::string fullPath = currentPath + '/' + fileName;

	#ifdef WIN32
	ShellExecute(0, 0, fullPath.c_str(), 0, 0, SW_SHOW);
	#endif // WIN32
}

void Fltk13GUI::showFileChooser(const char* initPath/* = NULL*/) {
	if (!fileChooser) {
		fileChooser = new Fl_Native_File_Chooser(Fl_Native_File_Chooser::BROWSE_DIRECTORY);
	}

	if (initPath) fileChooser->directory(initPath);

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

void Fltk13GUI::showAboutDialog() {
	if (!aboutDialog) {
		aboutDialog = new Fl_Window(300, 105, "About");
		Fl_Box *aboutInfo = new Fl_Box(70, 10, 200, 25, "MaMeEd 2012-11-12"); // TODO: update date automatically
		Fl_Button *aboutOk = new Fl_Button(230, 70, 60, 25, "Ok");
		aboutOk->callback(buttonCallback, (void*) BUTTON_ABOUT_OK);
		aboutDialog->end();
	}

	aboutDialog->show();
}

void Fltk13GUI::showExitNoSavingDialog() {
	if (!exitNoSavingDialog) {
	exitNoSavingDialog = new Fl_Window(390, 100, "Exit without saving?");
	Fl_Box *exitNoSavingInfo = new Fl_Box(70, 10, 200, 25, "Discard all changes?"); // TODO: update date automatically
	Fl_Button *exitNoSavingOk = new Fl_Button(230, 70, 60, 25, "Ok");
	exitNoSavingOk->callback(buttonCallback, (void*) BUTTON_NO_SAVING_OK);
	Fl_Button *exitNoSavingCancel = new Fl_Button(320, 70, 60, 25, "Cancel");
	exitNoSavingCancel->callback(buttonCallback, (void*) BUTTON_NO_SAVING_CANCEL);
	exitNoSavingDialog->end();
	}

	exitNoSavingDialog->show();
}

void Fltk13GUI::openDir(std::string path) {
	pathTextEdit->value(path.c_str());
	//pathTextEdit->red

	if (mCore->openDir(path)) {
		if (fileList) {
			pathFilesGroup->remove(fileList); // remove from window
			Fl::delete_widget(fileList);
		}
		if (keyValueList) {
			// remove what is displayed now; TODO: could this be somehow more efficient?
			keyValueList->clear();
			keyValueList->redraw();

//			remove(keyValueList);
//			Fl::delete_widget(keyValueList);
//			keyValueList = NULL;
		}

		fileList = new F13FileList(pathFilesGroup->x(), pathFilesGroup->y() + 26, pathFilesGroup->w(), pathFilesGroup->h()-26, mCore->getMDDir());
		pathFilesGroup->add(fileList); // add to window
		pathFilesGroup->resizable(fileList); // must be here, doesn't work otherwise

		//std::string const& selFileName = mCore->getMDDir()->getInitiallySelectedFileName();
		//if (selFileName.size()) {
		//	keyValueList = new F13KeyValueList(250, 30, 200, 600, selFileName);
		//}
		fileList->redraw();
	}
}

void Fltk13GUI::openSubDir(std::string subDirName) {
	std::string currentPath(mCore->getMDDir()->getDirPath());

	// todo: this code could be put into the core?
	if (subDirName == "..") {
		// go up one directory

		size_t p = currentPath.find_last_of("/\\");
		if (p == std::string::npos) {
			return; // do nothing: already uppermost directory
		}

		std::string newPath = currentPath.substr(0, p);
		openDir(newPath);
	} else {
		openDir(currentPath + '/' + subDirName);
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
	if (!fileList) return;

	MDDir const* mddir = mCore->getMDDir();
	if (!mddir) return;
	MDFile* mdfile = mddir->getMDFile(fileList->getSelectedFileName());
	if (!mdfile) return;

//	keyValueList->clear();
	if (keyValueList) {
		mainGroup->remove(keyValueList);
		Fl::delete_widget(keyValueList); // todo: instead of deleting, clearing and refilling it should be good, too?!?
		keyValueList = NULL;
		keyValueList = new F13KeyValueList(mainGroup->x() + 0.4*mainGroup->w(), mainGroup->y(), 0.3*mainGroup->w(), mainGroup->h(), mCore->getConfig());
		keyValueList->box(FL_DOWN_BOX);
		mainGroup->add(keyValueList);
	}

	keyValueList->setMDFile(mdfile);
	keyValueList->redraw();
}
