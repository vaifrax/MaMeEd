// MArcel's MEtadata EDitor

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <errno.h>	// errno
//#include <FL/fl_ask.H>

#include "Fltk13GUI.h"
#include "F13FileList.h"
#include "F13KeyValueList.h"
#include <FL/fl_ask.H>

#include "MCore.h"
#include "MDDir.h"
#include "MDFile.h"
#include "MDProperty.h"

#include <FL/Fl_Button.H>
#include <FL/Fl_Shared_Image.H>

#include <FL/x.H> // to set icon in windows
#include "../prj/resource.h" // for icon ID
#include "Fltk13WorldMap.h"

#include <curl/curl.h>

#include <iostream> // for cout, debugging only!?

/*static*/ Fltk13GUI* Fltk13GUI::fgui = NULL;

Fltk13GUI::Fltk13GUI(MCore* mCore) : MGUI(mCore), Fl_Double_Window(800,800,"Marcel's Metadata Editor") {
	fgui = this;
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
	Fl_Menu_Bar* menu = new Fl_Menu_Bar(0, 0, w(), MENU_HEIGHT);
	menu->copy(menuitems);

	// main group, containing all elements in the window
	const int MAIN_GROUP_PADDING = 0; // must be 0 otherwise tile doesn't work!
	mainGroup = new Fl_Tile(MAIN_GROUP_PADDING, MENU_HEIGHT+MAIN_GROUP_PADDING, w()-2*MAIN_GROUP_PADDING, h()-MENU_HEIGHT-2*MAIN_GROUP_PADDING); // put everything in a group for equal resizing
		int x0 = mainGroup->x(); // 0 [column 1] x1 [column 2] x2 [column 3] x3
		int x1 = 0.4*mainGroup->w() + x0;
		int x2 = 0.7*mainGroup->w() + x0;
		int x3 = mainGroup->w() + x0;
		int y0 = mainGroup->y(); // row positions
		int y1 = 0.3*mainGroup->h() + y0;
		int y2 = 0.6*mainGroup->h() + y0;
		int y9 = mainGroup->h() + y0;
		pathFilesGroup = new Fl_Group(x0, y0, x1-x0, mainGroup->h()); // left side
			const int PATHFILESGROUP_PADDING = 2;
			pathGroup = new Fl_Group(pathFilesGroup->x()+PATHFILESGROUP_PADDING, pathFilesGroup->y()+PATHFILESGROUP_PADDING, pathFilesGroup->w()-2*PATHFILESGROUP_PADDING, 29); // top bar of left side
				const int PATH_BUTTON_WIDTH = 35;
				pathTextEdit = new Fl_Input(pathGroup->x(), pathGroup->y(), pathGroup->w()-PATH_BUTTON_WIDTH-2, pathGroup->h()-4);
				//pathTextEdit->value("C:");
				pathTextEdit->callback(keyboardCallback, (void*) FROM_PATHTEXTEDIT);
				pathTextEdit->when(FL_WHEN_ENTER_KEY); // FL_WHEN_RELEASE

				Fl_Button* pathButton = new Fl_Button(pathGroup->x()+pathGroup->w()-PATH_BUTTON_WIDTH, pathGroup->y(), PATH_BUTTON_WIDTH, pathGroup->h()-4, "Path");
				pathButton->type(FL_NORMAL_BUTTON);
				pathButton->callback(buttonCallback, (void*) BUTTON_PATH);
			pathGroup->end();
			pathGroup->resizable(pathTextEdit);
		pathFilesGroup->box(FL_DOWN_BOX);
		pathFilesGroup->align(FL_ALIGN_CLIP);
		pathFilesGroup->end();

		keyValueList = new F13KeyValueList(x1, y0, x2-x1, mainGroup->h(), mCore->getConfig());
		keyValueList->box(FL_DOWN_BOX);
		keyValueList->align(FL_ALIGN_CLIP);
		keyValueList->end();

		//moduleColumn = new Fl_Scroll(x2, y0, x3-x2, mainGroup->h());
		// TODO: include larger preview or scale up thumb in file list?
		Fl_Box* largePreview = new Fl_Box(x2, y0, x3-x2, y1-y0, "large preview");
		largePreview->box(FL_DOWN_BOX);
		//b->image(

		// zoomable map
		// topo map: http://opentopomap.org/tiles/14/8640/5755.png
		worldMap = new Fltk13WorldMap(x2, y1, x3-x2, y2-y1, "world map");
		worldMap->box(FL_DOWN_BOX);
		//moduleColumn->box(FL_DOWN_BOX);
		//moduleColumn->align(FL_ALIGN_CLIP);
		//moduleColumn->resizable(largePreview);
		//moduleColumn->end();

		Fl_Box* empty = new Fl_Box(x2, y2, x3-x2, y9-y2, "empty");
		empty->box(FL_DOWN_BOX);

	mainGroup->end();
	resizable(mainGroup);
	end(); //////////////////////

	// dialogs
	fileChooser = NULL;
	aboutDialog = NULL;
	exitNoSavingDialog = NULL;

	// libcurl
	curl_global_init(CURL_GLOBAL_ALL);
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
	//Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
	//if (!fgui) return; // TODO: error

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
			//Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
			//if (!fgui) return; // TODO: error
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
			//Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
			//if (!fgui) return; // TODO: error
			fgui->openDir(fgui->pathTextEdit->value());
			break;
	}
}

// set current map coordinates and radius as GPS position of selected photos
void Fltk13GUI::setGPSPosition(double lon, double lat, float radius) {
	bool overwriteExisting = false;
	bool alreadyAskedOverwrite = false;

	// for all selected files
	for (auto sp=fileList->getSelectedFiles().begin(); sp!=fileList->getSelectedFiles().end(); ++sp) {
std::cout << (*sp)->getFileName() << std::endl;
		bool dataExists = (*sp)->mdf->getPropertyByKey("longitude");
		if (dataExists && !alreadyAskedOverwrite) {
			switch(fl_choice("Overwrite all existing position data?", "Yes to all", "No to all", "Cancel")) {
				case 0: // first button
					overwriteExisting = true;
					break;
				case 1: // second button
					overwriteExisting = false;
					break;
				default:
					return;
			}
		}
		if (!dataExists || overwriteExisting) {
			(*sp)->mdf->setKeyValueSrc("longitude", lon, "manual");
			(*sp)->mdf->setKeyValueSrc("latitude", lat, "manual");
			(*sp)->mdf->setKeyValueSrc("PositionUncertaintyRadius", radius, "manual");
		}
	}
}

// callback for the button next to a directory in fileList
/*static*/ void Fltk13GUI::changeDirCallback(Fl_Widget* widget, void* userData) {
	const char* dirName = (const char*) userData;
	//Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
	//if (!fgui) return; // TODO: error

	fgui->openSubDir(dirName);
}

/*static*/ void Fltk13GUI::launchViewerCallback(Fl_Widget* widget, void* userData) {
	const char* fileName = (const char*) userData;
	//Fltk13GUI* fgui = dynamic_cast<Fltk13GUI*> (widget->window());
	//if (!fgui) return; // TODO: error

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

		fileList = new F13FileList(pathFilesGroup->x()+2, pathFilesGroup->y() + 29, pathFilesGroup->w()-4, pathFilesGroup->h()-31, mCore->getMDDir());
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
	MDFile* mdfile = mddir->getMDFile(fileList->getActiveFileName());
	if (!mdfile) return;

//	keyValueList->clear();
	if (keyValueList) {
		int x = keyValueList->x();
		int y = keyValueList->y();
		int w = keyValueList->w();
		int h = keyValueList->h();
		mainGroup->remove(keyValueList);
		Fl::delete_widget(keyValueList); // todo: instead of deleting, clearing and refilling it should be good, too?!?
		keyValueList = NULL;
		keyValueList = new F13KeyValueList(x, y, w, h, mCore->getConfig());
		keyValueList->box(FL_DOWN_BOX);
		mainGroup->add(keyValueList);
	}

	keyValueList->setMDFile(mdfile);
	keyValueList->redraw();
}

// update Flag list from world map according to selection
void Fltk13GUI::updateFlags() {
	worldMap->clearFlags();
	for (auto sp=fileList->getSelectedFiles().begin(); sp!=fileList->getSelectedFiles().end(); ++sp) {
		MDFile* mdfile = (*sp)->mdf;
		MDProperty* longProp = mdfile->getPropertyByKey("longitude");
		MDProperty* latProp = mdfile->getPropertyByKey("latitude");
		MDProperty* radProp = mdfile->getPropertyByKey("PositionUncertaintyRadius");

		if (longProp && latProp) {
			double longitude  = atof(longProp->value.c_str());
			double latitude  = atof(latProp->value.c_str());
			float radius = atof(radProp->value.c_str());
			worldMap->addFlag(longitude, latitude, radius);
		}
		worldMap->redraw();
	}
}
