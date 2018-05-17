#include "F13FileList.h"
#include "Fltk13GUI.h"
#include "MDDir.h"
#include "MDFile.h"
#include "MDProperty.h"

#include <FL/Fl_Shared_Image.H>
//#include <FL/Fl_JPEG_Image.H>
#include "Fl_JPEG_Image-Fast.h"

//#include "ExifFile.h"

#include <iostream> // for cout, debugging only!?

#include <algorithm> // for std::find

#include <assert.h>


/*static*/ int F13FileList::thumbnailSize = 50;


////////////////////////////////////////////////////////////////////////////////////

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
FileGroup::FileGroup(int X, int Y, int W, int H, const char* fileName, MDFile* mdf, const char* dateStr, bool isDirectory, int index, const char* L/*=0*/) : Fl_Group(X,Y,W,H,L) {
	this->index = index;

	begin();
		thumbnailButton = NULL;
		this->mdf = mdf;

// TODO: much of the logic should be done/cached in the metadata updating/importing method

		if (isDirectory) {
			// this is a directory
			thumbnailButton = new Fl_Button(X, Y, F13FileList::thumbnailSize, F13FileList::thumbnailSize);
			if (strcmp(fileName, "..") == 0) {
				thumbnailButton->label("@8->");
			} else {
				thumbnailButton->label("@->");
			}
			thumbnailButton->callback(Fltk13GUI::changeDirCallback, (void*) fileName);
		} else {
			// this is a file
			thumbnailButton = new Fl_Button(X, Y, F13FileList::thumbnailSize, F13FileList::thumbnailSize);
			thumbnailButton->callback(Fltk13GUI::launchViewerCallback, (void*) fileName);

			std::string fileNameStr(fileName);
			std::string path = ((F13FileList*) (parent()))->getMDDir()->getDirPath() + '/' + fileName;
			bool thumbLoaded = false;

			MDProperty* thumbPosProp = mdf->getPropertyByKey("thumbnailPosition");
			MDProperty* thumbSizeProp = mdf->getPropertyByKey("thumbnailSize");

			if (thumbPosProp && thumbSizeProp) {
				long thumbPos  = atoi(thumbPosProp->value.c_str());
				long thumbSize  = atoi(thumbSizeProp->value.c_str());

				if ((thumbSize > 0) && (thumbSize < 100000)) { // sanity check

					// TODO: move this to another file away from user interface!!
					FILE* tf = fopen(path.c_str(), "rb");
					fseek(tf, thumbPos, SEEK_SET);
					unsigned char* thumbData = new unsigned char[thumbSize];
					fread(thumbData, 1, thumbSize, tf);

					Fl_JPEG_ImageFast jpgImgThumb(NULL, thumbData, 64);
					//std::cout << "exif thumbnail: " << jpgImgThumb.w() << 'x' << jpgImgThumb.h() << std::endl;
					float scDiv = ((float) F13FileList::thumbnailSize) / max(jpgImgThumb.w(), jpgImgThumb.h());
					int newW = (int) (scDiv * jpgImgThumb.w() + 0.499);
					int newH = (int) (scDiv * jpgImgThumb.h() + 0.499);
					Fl_Image* jpgImg = jpgImgThumb.copy(newW, newH);
					if (jpgImg && (jpgImg->w() > 0)) {
						thumbnailButton->image(jpgImg);
						thumbLoaded = true;
					}

					delete[] thumbData;
					fclose(tf);
				}
			}
		}

		// box for name
		//nameBox = new Fl_Box(X+46,Y+4,W-46,16, fileName);
		nameBox = new Fl_Box(X+F13FileList::thumbnailSize+6, Y, W-F13FileList::thumbnailSize-6-100,F13FileList::thumbnailSize, fileName);
		nameBox->align(FL_ALIGN_CENTER | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

		// box for date and time
		//dateTimeBox = new Fl_Box(X+46,Y+20,W-46,16, dateStr);
		dateTimeBox = new Fl_Box(nameBox->x()+nameBox->w()+6,Y,100,F13FileList::thumbnailSize, dateStr);
		dateTimeBox->align(FL_ALIGN_CENTER | FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
		dateTimeBox->labelcolor(0x77777700);

		resizable(nameBox);
	end();
}

int FileGroup::handle(int eventn) {
//Fl_Group::handle(eventn);
//std::cout << eventn << std::endl;
	switch (eventn) {
		case FL_PUSH: {
			int r = Fl_Group::handle(eventn);
			if (r) return r;
			return 1;} // to enable FL_RELEASE
		case FL_RELEASE:
		//case FL_DRAG:
		//case FL_MOVE:
			//return handle_mouse(eventn,Fl::event_button(), Fl::event_x(),Fl::event_y());
		//case FL_FOCUS:
			{
			//label ("Gained focus");
			assert(dynamic_cast<F13FileList*> (parent()));
			F13FileList* fileList = (F13FileList*) parent();

			fileList->setActiveFile(this);
			//assert(dynamic_cast<Fltk13GUI*> (fileList->window()));
			//Fltk13GUI* fgui = (Fltk13GUI*) fileList->window();
			Fltk13GUI::fgui->showFileMetaData();
			return Fl_Group::handle(eventn);
			}
		//case FL_UNFOCUS:
		//	//label ("Lost focus");
		//	//damage(1);
		//	return 1;
		default:
			return Fl_Group::handle(eventn);
			//return 0;
	};
};

////////////////////////////////////////////////////////////////////////////////////

F13FileList::F13FileList(int X, int Y, int W, int H, MDDir const* mddir/* = NULL*/) : Fl_Scroll(X,Y,W,H,0), mddir(mddir), activeFile(NULL), itemNum(0) {
	if (mddir) {
		fillList();

		// set activeFile to first node that contains a FileGroup
		int i = 0;
		while (!dynamic_cast<FileGroup*>(child(i))) {
			i++;
			if (i >= children()) return; // no child is a FileGroup
		}
		activeFile = (FileGroup*) child(i);
	}
}

FileGroup* F13FileList::getActiveFile(int offset/* = 0*/) const {
	if (offset == 0) return activeFile;
	int newIndex = activeFile->index + offset;
	//if (newIndex < 0) newIndex = 0;
	if (newIndex < 0) return NULL;
	//if (newIndex >= this->itemNum) newIndex = this->itemNum-1;
	if (newIndex >= this->itemNum) return NULL;
	FileGroup* ret = dynamic_cast<FileGroup*>(Fltk13GUI::fgui->fileList->child(newIndex));
	if (ret && ret->mdf) return ret;
	return NULL;
}
/*
const char* F13FileList::getActiveFileName(int offset/ * = 0* /) const {
	//return activeFile->getFileName();
	return getActiveFile(offset)->getFileName();
}
*/

void F13FileList::addItem(std::string const& fileName, std::string const& dateStr, bool isDirectory, MDFile* mdf) {
	int X = x(),
		Y = y() - yposition() + (itemNum*(F13FileList::thumbnailSize+1)) + 1,
		W = w() - 18, // -18: compensate for vscroll bar
		H = F13FileList::thumbnailSize;
	add(new FileGroup(X,Y,W,H, fileName.c_str(), mdf, dateStr.c_str(), isDirectory, itemNum));
	redraw();
	itemNum++;
}

void F13FileList::fillList() {
	if (!mddir) return;

	std::vector<MDFile*> const& files = mddir->getFiles();
	for (std::vector<MDFile*>::const_iterator i=files.begin(); i!=files.end(); ++i) {
		addItem((*i)->getName(), (*i)->getDateStr(), (*i)->isDirectory(), *i);
	}
}

void F13FileList::resize(int X, int Y, int W, int H) {
	// Tell children to resize to our new width
	for ( int t=0; t<itemNum; t++ ) {
		Fl_Widget *w = child(t);
		w->resize(w->x(), w->y(), W-18, w->h());    // W-18: leave room for scrollbar
	}
	// Tell scroll children changed in size
	init_sizes();
	Fl_Scroll::resize(X,Y,W,H);
}

void F13FileList::setActiveFileNext() {
	setActiveFile(getActiveFile(1));
}

void F13FileList::setActiveFile(FileGroup* sel) {
	if (!sel) return;

	FileGroup* prevActiveFile = activeFile;
	activeFile = sel;

	if (Fl::event_ctrl()) {
		// control: toggle state
		auto sp = std::find(selectedFiles.begin(), selectedFiles.end(), activeFile);
		if (sp == selectedFiles.end()) { // not yet in list?
			// is not yet in list, add to list
			selectedFiles.push_back(activeFile);
			// mark as selected
			activeFile->color(0xDDEEFF00);
			activeFile->box(FL_UP_BOX);
		} else {
			// is already in list, remove from list
			selectedFiles.erase(sp);
			// mark as deselected
			activeFile->color(FL_BACKGROUND_COLOR);
			activeFile->box(FL_FLAT_BOX);
		}
		activeFile->redraw();
	} else {
		// TODO: the following method might redraw items twice, is not optimal!

		// deselect old selection
		for (auto sp=selectedFiles.begin(); sp!=selectedFiles.end(); ++sp) {
			(*sp)->color(FL_BACKGROUND_COLOR);
			(*sp)->box(FL_FLAT_BOX);
			(*sp)->redraw();
		}
		selectedFiles.clear();

		// make new selection
		if (Fl::event_shift()) {
			// shift
			bool act = false;
			bool lastAct = false;
			for (int i=0; i<this->children() && !lastAct; i++) {
				FileGroup* lePtr = (FileGroup*) this->child(i);
				//std::cout << lePtr->getFileName() << std::endl;
				if ((lePtr == prevActiveFile) || (lePtr == activeFile)) {
					if (!act) act = true; else lastAct = true;
				}
				if (act) {
					selectedFiles.push_back(lePtr);
					lePtr->color(0xDDEEFF00);
					lePtr->box(FL_UP_BOX);
					lePtr->redraw();
				}
				//lePtr++;
			}
		} else {
			// no keys pressed
			activeFile->color(0xDDEEFF00);
			activeFile->box(FL_UP_BOX);
			activeFile->redraw();
			selectedFiles.push_back(activeFile);
		}
	}

	// update flags from worldmap
	Fltk13GUI::fgui->updateFlags();

	// update star rating in widgets
	Fltk13GUI::fgui->updateStars();
}

/*
// to enable keyboard events?
int F13FileList::handle(int eventn) {
//std::cout << eventn << std::endl;
	switch (eventn) {
		case FL_KEYBOARD: {
			setActiveFileNext();
			Fltk13GUI::fgui->showFileMetaData();
			return 1;//Fl_Group::handle(eventn);
			}
		case FL_FOCUS:
		case FL_UNFOCUS:
			return 1;
		default:
			return Fl_Scroll::handle(eventn);
	};
};
*/