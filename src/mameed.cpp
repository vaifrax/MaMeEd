// mameed : MArcel's MEtadata EDitor
//
// edit metadata for files (or folders?), save in .ini file format per directory
// written especially for .jpg photos
//
// by Marcel Lancelle 2012

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Fltk13GUI.h"
#include "MCore.h"

int main(int argc, char* argv[]) {
	MCore* mCore = new MCore();
	MGUI* mGUI = (MGUI*) new Fltk13GUI(mCore);
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

/*

default fields and type definitions:

scene title
 description: describes content of the scene
event
 description: name of event, if applicable
photographer
 description: the person(s) taking the photo
camera owner
 description: if other from photographer, this person is most likely to have stored the original photo
date.year
 type: integer
date.month
 type: [1|12]
date.day
 type: [1|31]
date.hour
 type: [0|23]
date.minute
 type: [0|59]
date.second
 type: [0|59]
date.UTC-Offset
 type: [-12|12]

focal length (35mm eqiv.) / mm

*/