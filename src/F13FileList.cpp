#include "F13FileList.h"
#include "Fltk13GUI.h"
#include "MDDir.h"
#include "MDFile.h"
#include "FL/Fl_Box.H"

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
			fixedBox = new Fl_Box(X,Y,60,40,"Fixed");
			fixedBox->box(FL_UP_BOX);
			// Stretchy box
			stretchBox = new Fl_Box(X+60,Y,W-60,25, fileName);
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
