#include "F13KeyValueList.h"
#include "MDFile.h"
#include "MDProperty.h"

#include "FL/Fl_Box.H"
#include "FL/Fl_Input.H"

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
class KeyValueGroup : public Fl_Group {
	//Fl_Box *fixedBox;
	//Fl_Box *stretchBox;
	Fl_Input *fixedBox;
	Fl_Input *stretchBox;
public:
	std::string getKey() {return fixedBox->value();}
	std::string getValue() {return stretchBox->value();}

	KeyValueGroup(int X, int Y, int W, int H, const char* key, const char* value, const char* L=0) : Fl_Group(X,Y,W,H,L) {
		begin();
			// Fixed width box
			//fixedBox = new Fl_Box(X,Y,fixedWidth,25,key);
			fixedBox = new Fl_Input(X,Y,90,25,"");
			fixedBox->value(key);
			//fixedBox->box(FL_BORDER_FRAME);
			fixedBox->box(FL_FLAT_BOX);
			//fixedBox->color(0x99999900);
fixedBox->color(0xFFFFFF00);
			// Stretchy boxKeyValueScroll
			//stretchBox = new Fl_Box(X+fixedWidth,Y,W-fixedWidth,defaultHeight, value);
			stretchBox = new Fl_Input(X+90+1,Y,W-90-1,25);
			stretchBox->value(value);
			//stretchBox->box(FL_BORDER_FRAME);
			stretchBox->box(FL_FLAT_BOX);
			//stretchBox->color(0x99999900);
stretchBox->color(0xFFFFFF00);
			resizable(stretchBox);
		end();
	}
	int handle(int eventn) {
		switch (eventn) {
			//case FL_PUSH:
			//case FL_RELEASE:
			//case FL_DRAG:
			//case FL_MOVE:
				//return handle_mouse(eventn,Fl::event_button(), Fl::event_x(),Fl::event_y());
			case FL_FOCUS: {
				//label ("Gained focus");
				((F13KeyValueList*) parent())->setSelectedKeyValue(this);
				//selectedKeyValue = this;
//if (selectedFile) {
//	MDPropFile& pf = dirDB->getPropFile(selectedFile->getFileName()); // TODO
//	propFile = &pf; // TODO
//}
				//return Fl_Window::handle(eventn); //return 1;
				return Fl_Group::handle(eventn); }
			case FL_UNFOCUS: {
				//label ("Lost focus");
				//damage(1);
// TODO: also do this before saving, or at return; maybe add a new empty key value pair at the end
				KeyValueGroup* selectedKeyValue = ((F13KeyValueList*) parent())->getSelectedKeyValue();
//				propFile->addKeyValue(selectedKeyValue->fixedBox->value(), selectedKeyValue->stretchBox->value());
// TODO: how to remove?
				return 1; }
			default:
				//return Fl_Window::handle(eventn);
				return 0;
		};
	}
};

F13KeyValueList::F13KeyValueList(int X, int Y, int W, int H, MDFile* mdfile) : Fl_Scroll(X,Y,W,H,0), mdfile(mdfile), selectedKeyValue(NULL), itemNum(0) {
	if (mdfile) fillList();
}

void F13KeyValueList::addItem(std::string const& key, std::string const& value) {
	int X = x() + 1,
		Y = y() - yposition() + (itemNum*41) + 1,
		W = w() - 20,                           // -20: compensate for vscroll bar
		H = 41;
	add(new KeyValueGroup(X,Y,W,H, key.c_str(), value.c_str()));
	redraw();
	itemNum++;
}

void F13KeyValueList::fillList() {
	if (!mdfile) return;

	for (std::vector<MDProperty*>::iterator i=mdfile->properties.begin(); i!=mdfile->properties.end(); ++i) {
		addItem((*i)->key, (*i)->value);
	}
}

/*

// Custom scroll that tells children to follow scroll's width when resized
class KeyValueScroll : public Fl_Scroll {
	int nchild;
public:
	KeyValueScroll(int X, int Y, int W, int H, const char* L=0) : Fl_Scroll(X,Y,W,H,L) {
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
	void AddItem(std::string const& key, std::string const& value) {
		int X = x() + 1,
			Y = y() - yposition() + (nchild*26) + 1,
			W = w() - 20,                           // -20: compensate for vscroll bar
			H = 25;
		add(new KeyValueGroup(X,Y,W,H, key.c_str(), value.c_str()));
		redraw();
		nchild++;
	}
};
*/
