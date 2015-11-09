#include "Fltk13GUI.h"
#include "F13KeyValueGroup.h"

KeyValueGroup::KeyValueGroup(int X, int Y, int W, int H, const char* key, const char* value, std::string const& keyDescription, F13KeyValueList::KeyType keyType /*= F13KeyValueList::USER_KEY*/) : Fl_Group(X,Y,W,H,0) {
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

std::string /*const&*/ KeyValueGroup::getKey() const {
	return keyInput->value();
}

std::string /*const&*/ KeyValueGroup::getValue() const {
	return valueInput->value();
}

void KeyValueGroup::setValue(const char* newValue) {
	valueInput->value(newValue);
}

int KeyValueGroup::handle(int eventn) {
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

			// apply to all selected files not just one

//TODO
			//KeyValueGroup* selectedKeyValue = ((F13KeyValueList*) parent())->getSelectedKeyValue();
			//if (!selectedKeyValue) return;
			//std::string newKey = getKey();
			//std::string newValue = getValue();
			Fltk13GUI::fgui->applyChangesOfSelectedKeyValue();
/*

?				MDDir* mdd = ;
			for (auto mdf=mdd->files.begin(); mdf!=mdd->files.end(); ++mdf) {
				std::string origSelectedKey = selectedKeyValue->getKey();
				std::string origSelectedValue = selectedKeyValue->getValue();
				if ((origSelectedKey != newKey) || (origSelectedValue != newValue)) {
					mdf->changeKeyValue(origSelectedKey, newKey, newValue);
				}
			}
			// only last one in list:
			//((F13KeyValueList*) parent())->applyChangesOfSelectedKeyValue();
			// //				propFile->setKeyValue(selectedKeyValue->fixedBox->value(), selectedKeyValue->stretchBox->value());
			// // TODO: how to remove?
			// //return 1; 
*/
			return Fl_Group::handle(eventn);
			}
		default:
			return Fl_Group::handle(eventn);
	};
}
// KeyValueGroup
