#include "F13FileList.h"
#include "Fltk13GUI.h"
#include "MDDir.h"
#include "MDFile.h"
#include "FL/Fl_Box.H"

#include <FL/Fl_Shared_Image.H>
#include <FL/Fl_JPEG_Image.H>

#include "ExifFile.h"

#include <iostream> // for cout, debugging only!?

////////////////////////////////////////////////////////////////////////////////////

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
class FileGroup : public Fl_Group {
	Fl_Box *fixedBox;
	Fl_Box *stretchBox;
public:
	const char* getFileName() const {return stretchBox->label();}

	FileGroup(int X, int Y, int W, int H, const char* fileName, const char* L=0) : Fl_Group(X,Y,W,H,L) {
		begin();
			// Fixed width box
			fixedBox = new Fl_Box(X,Y,40,40);
			//fixedBox->box(FL_UP_BOX);

			std::string fileNameStr(fileName);
			int fileNameStrLen = fileNameStr.length();

			// extract extension and convert to upper case
			std::string ext;
			if (fileNameStrLen > 1) {
				std::string::size_type p = fileNameStr.rfind('.', fileNameStrLen-1);
				if (p != std::string::npos) {
					ext = fileNameStr.substr(p+1);
					for (int i=0; i<ext.length(); i++) ext[i] = toupper(ext[i]);
				}
			}

			// test for .jpg or .jpeg
			if ((ext == "JPG") || (ext == "JPEG")) {
				std::string path = ((F13FileList*) (parent()))->getMDDir()->getDirPath() + '/' + fileName;

				// try to load exif thumbnail and use as image
				ExifFile ef(path.c_str());
				unsigned char* thumbData = ef.getThumbnailData();
				if (thumbData) {
					Fl_JPEG_Image jpgImgThumb(NULL, thumbData);
					//std::cout << "exif thumbnail: " << jpgImgThumb.w() << 'x' << jpgImgThumb.h() << std::endl;
					float scDiv = 40.f / max(jpgImgThumb.w(), jpgImgThumb.h());
					int newW = (int) (scDiv * jpgImgThumb.w() + 0.499);
					int newH = (int) (scDiv * jpgImgThumb.h() + 0.499);
					Fl_Image* jpgImg = jpgImgThumb.copy(newW, newH);
					if (jpgImg && (jpgImg->w() > 0))
						fixedBox->image(jpgImg);
				} else {
					// load full resolution and scale it down
					Fl_JPEG_Image jpgImgBig(path.c_str());
					float scDiv = 40.f / max(jpgImgBig.w(), jpgImgBig.h());
					int newW = (int) (scDiv * jpgImgBig.w() + 0.499);
					int newH = (int) (scDiv * jpgImgBig.h() + 0.499);
					Fl_Image* jpgImg = jpgImgBig.copy(newW, newH);
					if (jpgImg && (jpgImg->w() > 0))
						fixedBox->image(jpgImg);
				}
			}

			// Stretchy box
			stretchBox = new Fl_Box(X+50,Y,W-50,40, fileName);
			stretchBox->align(FL_ALIGN_CENTER | FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
			//stretchBox->box(FL_UP_BOX);
			resizable(stretchBox);
		end();
	}

	int handle(int eventn) {
		switch (eventn) {
			//case FL_PUSH:
			case FL_RELEASE:
			//case FL_DRAG:
			//case FL_MOVE:
				//return handle_mouse(eventn,Fl::event_button(), Fl::event_x(),Fl::event_y());
			//case FL_FOCUS:
			{
				//label ("Gained focus");
				F13FileList* fileList = (F13FileList*) parent();
				FileGroup* prevSelectedFile = fileList->getSelectedFile(); // previously selected file
				//if (this == prevSelectedFile) return 1; // same file: do nothing (no FL_FOCUS event anyway, right?)
				if (prevSelectedFile) {
					prevSelectedFile->color(0xDDDDDD00); // TODO
					prevSelectedFile->box(FL_FLAT_BOX);
					prevSelectedFile->redraw();
					//selectedFile->box(FL_NO_BOX);
					//selectedFile->window()->redraw(); // redraw parent because this is transparent :-( -> performance not optimal
				}

				fileList->setSelectedFile(this);
				Fltk13GUI* fgui = (Fltk13GUI*) fileList->parent();
				fgui->showFileMetaData();

				this->color(0xFF000000);
				this->box(FL_UP_BOX);
				redraw();
				return 1; }
			//case FL_UNFOCUS:
			//	//label ("Lost focus");
			//	//damage(1);
			//	return 1;
			default:
				//return Fl_Window::handle(eventn);
				return 0;
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


void F13FileList::addItem(std::string const& fileName) {
	int X = x() + 1,
		Y = y() - yposition() + (itemNum*41) + 1,
		W = w() - 20,                           // -20: compensate for vscroll bar
		H = 41;
	add(new FileGroup(X,Y,W,H, fileName.c_str()));
	redraw();
	itemNum++;
}

void F13FileList::fillList() {
	if (!mddir) return;

	std::vector<MDFile*> const& files = mddir->getFiles();
	for (std::vector<MDFile*>::const_iterator i=files.begin(); i!=files.end(); ++i) {
		addItem((*i)->getName());
	}
}
