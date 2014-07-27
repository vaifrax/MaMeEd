#include "F13FileList.h"
#include "Fltk13GUI.h"
#include "MDDir.h"
#include "MDFile.h"
#include "FL/Fl_Box.H"
#include "FL/Fl_Button.H"

#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_JPEG_Image.H>

#include "ExifFile.h"

#include <iostream> // for cout, debugging only!?

#include <assert.h>


/*static*/ int F13FileList::thumbnailSize = 50;


////////////////////////////////////////////////////////////////////////////////////

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
class FileGroup : public Fl_Group {
	Fl_Button *thumbnailButton;
	Fl_Box *nameBox;
	Fl_Box *dateTimeBox;
public:
	const char* getFileName() const {return nameBox->label();}

	FileGroup(int X, int Y, int W, int H, const char* fileName, const char* dateStr, bool isDirectory, const char* L=0) : Fl_Group(X,Y,W,H,L) {
		begin();
			thumbnailButton = NULL;

// TODO: much of the logic should be done/cached in the metadata updating/importing metho

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
				std::string fileNameStr(fileName);
				int fileNameStrLen = fileNameStr.length();

				// extract extension and convert to upper case
				std::string ext;
				if (fileNameStrLen > 1) {
					std::string::size_type p = fileNameStr.rfind('.', fileNameStrLen-1);
					if (p != std::string::npos) {
						ext = fileNameStr.substr(p+1);
						for (unsigned int i=0; i<ext.length(); i++) ext[i] = toupper(ext[i]);
					}
				}

				// test for .jpg or .jpeg
				if ((ext == "JPG") || (ext == "JPEG")) {
					thumbnailButton = new Fl_Button(X, Y, F13FileList::thumbnailSize, F13FileList::thumbnailSize);
					thumbnailButton->callback(Fltk13GUI::launchViewerCallback, (void*) fileName);
					std::string path = ((F13FileList*) (parent()))->getMDDir()->getDirPath() + '/' + fileName;

					// try to load exif thumbnail and use as image
					// TODO: read position and length of thumbnail data in file from MDFile properties
					ExifFile ef(path.c_str());
					unsigned char* thumbData = ef.getThumbnailData();
					if (thumbData) {
						Fl_JPEG_Image jpgImgThumb(NULL, thumbData);
						//std::cout << "exif thumbnail: " << jpgImgThumb.w() << 'x' << jpgImgThumb.h() << std::endl;
						float scDiv = ((float) F13FileList::thumbnailSize) / max(jpgImgThumb.w(), jpgImgThumb.h());
						int newW = (int) (scDiv * jpgImgThumb.w() + 0.499);
						int newH = (int) (scDiv * jpgImgThumb.h() + 0.499);
						Fl_Image* jpgImg = jpgImgThumb.copy(newW, newH);
						if (jpgImg && (jpgImg->w() > 0))
							thumbnailButton->image(jpgImg);
					} else {
						// load full resolution and scale it down
						Fl_JPEG_Image jpgImgBig(path.c_str());
						float scDiv = ((float) F13FileList::thumbnailSize) / max(jpgImgBig.w(), jpgImgBig.h());
						int newW = (int) (scDiv * jpgImgBig.w() + 0.499);
						int newH = (int) (scDiv * jpgImgBig.h() + 0.499);
						Fl_Image* jpgImg = jpgImgBig.copy(newW, newH);
						if (jpgImg && (jpgImg->w() > 0))
							thumbnailButton->image(jpgImg);
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

	int handle(int eventn) {
//Fl_Group::handle(eventn);
		switch (eventn) {
			//case FL_PUSH:
			case FL_RELEASE:
			//case FL_DRAG:
			//case FL_MOVE:
				//return handle_mouse(eventn,Fl::event_button(), Fl::event_x(),Fl::event_y());
			//case FL_FOCUS:
				{
				//label ("Gained focus");
				assert(dynamic_cast<F13FileList*> (parent()));
				F13FileList* fileList = (F13FileList*) parent();
				FileGroup* prevSelectedFile = fileList->getSelectedFile(); // previously selected file
				//if (this == prevSelectedFile) return 1; // same file: do nothing (no FL_FOCUS event anyway, right?)
				if (prevSelectedFile) {
					//prevSelectedFile->color(0xDDDDDD00); // TODO
					prevSelectedFile->color(FL_BACKGROUND_COLOR); // TODO
					prevSelectedFile->box(FL_FLAT_BOX);
					prevSelectedFile->redraw();
					//selectedFile->box(FL_NO_BOX);
					//selectedFile->window()->redraw(); // redraw parent because this is transparent :-( -> performance not optimal
				}

				fileList->setSelectedFile(this);
				assert(dynamic_cast<Fltk13GUI*> (fileList->window()));
				Fltk13GUI* fgui = (Fltk13GUI*) fileList->window();
				fgui->showFileMetaData();

				this->color(0xDDEEFF00);
				this->box(FL_UP_BOX);
				redraw();
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
	}
};

////////////////////////////////////////////////////////////////////////////////////

F13FileList::F13FileList(int X, int Y, int W, int H, MDDir const* mddir/* = NULL*/) : Fl_Scroll(X,Y,W,H,0), mddir(mddir), selectedFile(NULL), itemNum(0) {
	if (mddir) fillList();
}

const char* F13FileList::getSelectedFileName() const {
	return selectedFile->getFileName();
}


void F13FileList::addItem(std::string const& fileName, std::string const& dateStr, bool isDirectory) {
	int X = x(),
		Y = y() - yposition() + (itemNum*(F13FileList::thumbnailSize+1)) + 1,
		W = w() - 18, // -18: compensate for vscroll bar
		H = F13FileList::thumbnailSize;
	add(new FileGroup(X,Y,W,H, fileName.c_str(), dateStr.c_str(), isDirectory));
	redraw();
	itemNum++;
}

void F13FileList::fillList() {
	if (!mddir) return;

	std::vector<MDFile*> const& files = mddir->getFiles();
	for (std::vector<MDFile*>::const_iterator i=files.begin(); i!=files.end(); ++i) {
		addItem((*i)->getName(), (*i)->getDateStr(), (*i)->isDirectory());
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
