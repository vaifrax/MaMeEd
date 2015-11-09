#ifndef F13KEYVALUEGROUP_HEADER_INCLUDED
#define F13KEYVALUEGROUP_HEADER_INCLUDED

#include "FL/Fl_Group.H"
#include "FL/Fl_Input.H"
#include <string>

#include "F13KeyValueList.h";

// Combo widget to appear in the scroll, two boxes: one fixed, the other stretches
//class KeyValueGroup;
class KeyValueGroup : public Fl_Group {
  protected:
	Fl_Input *keyInput;
	Fl_Input *valueInput;
  public:
	KeyValueGroup(int X, int Y, int W, int H, const char* key, const char* value, std::string const& keyDescription, F13KeyValueList::KeyType keyType = F13KeyValueList::USER_KEY);
	std::string /*const&*/ getKey() const;
	std::string /*const&*/ getValue() const;
	void setValue(const char* newValue);
	int handle(int eventn);
};

#endif // F13KEYVALUEGROUP_HEADER_INCLUDED
