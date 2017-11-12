#include "F13KeyValueList.h"

//#include <iostream> // only for debugging

#include "MDFile.h"
#include "MDDir.h"
#include "MDProperty.h"
#include "MConfig.h"
#include "Fltk13GUI.h"
#include "F13KeyValueGroup.h"
#include "MCore.h"

#include "FL/Fl_Box.H"
#include "FL/Fl_Input.H"
#include "FL/Fl_Tooltip.H"

//#include <FL/names.h> // defines fl_eventnames[]

/*static*/ const std::string F13KeyValueList::emptyString;



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
//		mdfile->changeKeyValue(origSelectedKey, newKey, newValue);
		origSelectedKey = selectedKeyValue->getKey();
		origSelectedValue = selectedKeyValue->getValue();

//TODO
		const MDDir* mdd = Fltk13GUI::fgui->mCore->getMDDir();
		for (auto mdf=mdd->getFiles().begin(); mdf!=mdd->getFiles().end(); ++mdf) {
			//if ((origSelectedKey != newKey) || (origSelectedValue != newValue)) {
			(*mdf)->changeKeyValue(origSelectedKey, newKey, newValue);
	//?						origSelectedKey = selectedKeyValue->getKey();
	//?						origSelectedValue = selectedKeyValue->getValue();
			//}
		}

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
	for ( int t=0; t<children(); t++ ) {
		Fl_Widget *w = child(t);
		w->resize(w->x(), w->y(), W-20, w->h());    // W-20: leave room for scrollbar
	}
	// Tell scroll children changed in size
	init_sizes();
	Fl_Scroll::resize(X,Y,W,H);
}
