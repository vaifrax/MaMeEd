#include "F13FileList.h"

/*
FileGroup* selectedFile = NULL;

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
*/
