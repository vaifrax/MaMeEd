// mameed : MArcel's MEtadata EDitor
//
// edit metadata for files (or folders?), save in .ini file format per directory
// written especially for .jpg photos
//
// by Marcel Lancelle 2012

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MFltk13GUI.h"
#include "MCore.h"

int main(int argc, char* argv[]) {
	MCore* mCore = new MCore();
	MGUI* mGUI = (MGUI*) new MFltk13GUI(mCore);
	mGUI->setCmdParams(argc, argv);

	// handle command line arguments : open file/folder that is passed on
	if (argc > 1) {
		mGUI->openDir(argv[1]);
	} else {
		//mGUI->openDir(""); // TODO: open most recently used or default
	}

	int retVal = mGUI->showWindow();

	// clean up
	delete mGUI;
	delete mCore;
	return retVal;
}
