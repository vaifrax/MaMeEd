#include "F13KeyValueList.h"
#include "MDFile.h"
#include "MDProperty.h"

#include "FL/Fl_Box.H"
#include "FL/Fl_Input.H"

//#include <FL/names.h> // defines fl_eventnames[]

/*static*/ const std::string F13KeyValueList::emptyString;


// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
class KeyValueGroup : public Fl_Group {
	Fl_Input *keyInput;
	Fl_Input *valueInput;
public:
	std::string /*const&*/ getKey() const {return keyInput->value();}
	std::string /*const&*/ getValue() const {return valueInput->value();}

	KeyValueGroup(int X, int Y, int W, int H, const char* key, const char* value, const char* L=0) : Fl_Group(X,Y,W,H,L) {
		begin();
			keyInput = new Fl_Input(X,Y,90,25,"");
			keyInput->value(key);
			keyInput->box(FL_FLAT_BOX);
			keyInput->color(0xFFFFFF00);
			keyInput->callback(F13KeyValueList::keyCallback, this->parent());
			keyInput->when(FL_WHEN_CHANGED |  FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);

			valueInput = new Fl_Input(X+90+1,Y,W-90-1,25);
			valueInput->value(value);
			valueInput->box(FL_FLAT_BOX);
			valueInput->color(0xFFFFFF00);
			//resizable(valueInput);
		end();
	}

	int handle(int eventn) {
		//printf("Event was %s (%d)\n", fl_eventnames[eventn], eventn); // e.g. "Event was FL_PUSH (1)"

		switch (eventn) {
			case FL_PUSH:
			//case FL_RELEASE:
			//case FL_DRAG:
			//case FL_MOVE:
				//return handle_mouse(eventn,Fl::event_button(), Fl::event_x(),Fl::event_y());
			case FL_FOCUS:
				{
				if (this == ((F13KeyValueList*) parent())->getSelectedKeyValue()) return Fl_Group::handle(eventn);
				int groupHandleReturn = Fl_Group::handle(eventn); // order is important here: first pass on event to trigger unfocus,
				((F13KeyValueList*) parent())->setSelectedKeyValue(this); // then set newly selected key/value pair
				//return Fl_Group::handle(eventn);
				return groupHandleReturn;
				//return 1;
				 }
			case FL_UNFOCUS: {
// TODO: this is called 2x? and also when group is the same (changing from key to value)?
				//label ("Lost focus");
				//damage(1);
// TODO: also do this before saving, or at return; maybe add a new empty key value pair at the end
				((F13KeyValueList*) parent())->applyChangesOfSelectedKeyValue();
//				propFile->addKeyValue(selectedKeyValue->fixedBox->value(), selectedKeyValue->stretchBox->value());
// TODO: how to remove?
				//return 1; 
				return Fl_Group::handle(eventn);
				}
			default:
				return Fl_Group::handle(eventn);
		};
	}
};

F13KeyValueList::F13KeyValueList(int X, int Y, int W, int H, MDFile* mdfile) : Fl_Scroll(X,Y,W,H,0), mdfile(mdfile), selectedKeyValue(NULL), itemNum(0) {
	if (mdfile) fillList();
}

/*static*/ void F13KeyValueList::keyCallback(Fl_Widget *w, void *data) {
	F13KeyValueList* kvl = (F13KeyValueList*) data;
	//std::string s = ((KeyValueGroup*) (kvl->child(kvl->itemNum-1)))->getKey();
	if ((kvl->itemNum==0) || ((KeyValueGroup*) (kvl->child(kvl->itemNum-1)))->getKey().size()) {
		kvl->addItem(emptyString, emptyString);
	}
}

void F13KeyValueList::setSelectedKeyValue(KeyValueGroup* selectedKeyValue) {
	this->selectedKeyValue = selectedKeyValue;
	origSelectedKey = selectedKeyValue->getKey();
	origSelectedValue = selectedKeyValue->getValue();
}

void F13KeyValueList::applyChangesOfSelectedKeyValue() {
	if (!selectedKeyValue) return;

	std::string newKey = selectedKeyValue->getKey();
	std::string newValue = selectedKeyValue->getValue();

	if ((origSelectedKey != newKey) || (origSelectedValue != newValue)) {
		mdfile->changeKeyValue(origSelectedKey, newKey, newValue);
		origSelectedKey = selectedKeyValue->getKey();
		origSelectedValue = selectedKeyValue->getValue();
	}
}

void F13KeyValueList::addItem(std::string const& key, std::string const& value) {
	int X = x() + 1,
		Y = y() - yposition() + (itemNum*26) + 1,
		W = w() - 20,                           // -20: compensate for vscroll bar
		H = 26;
	add(new KeyValueGroup(X,Y,W,H, key.c_str(), value.c_str()));
	redraw();
	itemNum++;
}

void F13KeyValueList::fillList() {
	if (!mdfile) return;

	for (std::vector<MDProperty*>::iterator i=mdfile->properties.begin(); i!=mdfile->properties.end(); ++i) {
		addItem((*i)->key, (*i)->value);
	}
	addItem(emptyString, emptyString);
}

void F13KeyValueList::resize(int X, int Y, int W, int H) {
	// Tell children to resize to our new width
	for ( int t=0; t<itemNum; t++ ) {
		Fl_Widget *w = child(t);
		w->resize(w->x(), w->y(), W-20, w->h());    // W-20: leave room for scrollbar
	}
	// Tell scroll children changed in size
	init_sizes();
	Fl_Scroll::resize(X,Y,W,H);
}
