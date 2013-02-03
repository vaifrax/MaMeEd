#include "F13KeyValueList.h"
#include "MDFile.h"
#include "MDProperty.h"
#include "MConfig.h"

#include "FL/Fl_Box.H"
#include "FL/Fl_Input.H"
#include "FL/Fl_Tooltip.H"

//#include <FL/names.h> // defines fl_eventnames[]

/*static*/ const std::string F13KeyValueList::emptyString;


// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
class KeyValueGroup : public Fl_Group {
	Fl_Input *keyInput;
	Fl_Input *valueInput;
public:
	std::string /*const&*/ getKey() const {return keyInput->value();}
	std::string /*const&*/ getValue() const {return valueInput->value();}
	void setValue(const char* newValue) {valueInput->value(newValue);}

	KeyValueGroup(int X, int Y, int W, int H, const char* key, const char* value, std::string const& keyDescription, F13KeyValueList::KeyType keyType = F13KeyValueList::USER_KEY) : Fl_Group(X,Y,W,H,0) {
		begin();
			keyInput = new Fl_Input(X+2,Y+2,90,25,"");
			keyInput->value(key);
			keyInput->box(FL_FLAT_BOX);
			keyInput->color(0xFFFFFF00);
			if (keyType == F13KeyValueList::DEFAULT_KEY) {
				keyInput->deactivate();
				keyInput->textcolor(0x00007700);
			}
			keyInput->callback(F13KeyValueList::keyCallback, this->parent());
			keyInput->when(FL_WHEN_CHANGED | FL_WHEN_ENTER_KEY | FL_WHEN_NOT_CHANGED);
			if (keyDescription.size()) {
				keyInput->tooltip(keyDescription.c_str());
			}

			valueInput = new Fl_Input(X+90+3,Y+2,W-90-3-5,25);
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
}; // KeyValueGroup

//////////////////////////////////////////////////////////////////////////////////

F13KeyValueList::F13KeyValueList(int X, int Y, int W, int H, MConfig const* config) : Fl_Scroll(X,Y,W,H,0), selectedKeyValue(NULL), itemNum(0), config(config) {
}

F13KeyValueList::~F13KeyValueList() {
}

void F13KeyValueList::setMDFile(MDFile* mdfile) {
	//clear();
	this->mdfile = mdfile;
	if (mdfile) fillList();
}

/*static*/ void F13KeyValueList::keyCallback(Fl_Widget *w, void *data) {
	F13KeyValueList* kvl = (F13KeyValueList*) data;
	//std::string s = ((KeyValueGroup*) (kvl->child(kvl->itemNum-1)))->getKey();

	// add new row if necessary
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

void F13KeyValueList::addItem(std::string const& key, std::string const& value, std::string const& keyDescription/* = emptyString*/, F13KeyValueList::KeyType keyType/* = USER_KEY*/) {
	int X = x() + 1,
		Y = y() - yposition() + (itemNum*26) + 1,
		W = w() - 20,                           // -20: compensate for vscroll bar
		H = 26;
	add(new KeyValueGroup(X,Y,W,H, key.c_str(), value.c_str(), keyDescription, keyType));
	redraw();
	itemNum++;
}

void F13KeyValueList::setValueOrAddItem(std::string const& key, std::string const& value) {
	for (int t=0; t<itemNum; t++) {
		KeyValueGroup* kvg = dynamic_cast<KeyValueGroup*> (child(t));
		if (kvg && (kvg->getKey()==key)) {
			kvg->setValue(value.c_str());
			return;
		}
	}

	// does not yet exist, add it
	addItem(key, value);
}

void F13KeyValueList::fillList() {
	if (!mdfile) return;

	// fill with top default keys
	const std::vector<DefaultKey*> keys = config->getKeys();
	for (std::vector<DefaultKey*>::const_iterator i=keys.begin(); i!=keys.end(); ++i) {	
		if ((*i)->order==DefaultKey::TOP) addItem((*i)->name, emptyString, (*i)->description, DEFAULT_KEY);
	}

	for (std::vector<MDProperty*>::iterator i=mdfile->properties.begin(); i!=mdfile->properties.end(); ++i) {
		setValueOrAddItem((*i)->key, (*i)->value);
	}
	addItem(emptyString, emptyString);
/*
	// fill with bottom default keys
	const std::vector<DefaultKey*> keys = config->getKeys();
	for (std::vector<DefaultKey*>::const_iterator i=keys.begin(); i!=keys.end(); ++i) {	
		if ((*i)->order==DefaultKey::BOTTOM) addItem((*i)->name, emptyString, (*i)->description, DEFAULT_KEY);
	}
*/
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
