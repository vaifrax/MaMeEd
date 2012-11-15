#ifndef F13KEYVALUELIST_HEADER_INCLUDED
#define F13KEYVALUELIST_HEADER_INCLUDED

#include "FL/Fl_Scroll.H"
#include <string>

class KeyValueGroup;
class MDFile;

class F13KeyValueList : public Fl_Scroll {
  public:
	F13KeyValueList(int X, int Y, int W, int H, MDFile* mdfile);
	KeyValueGroup* getSelectedKeyValue() {return selectedKeyValue;}
	void setSelectedKeyValue(KeyValueGroup* sel) {selectedKeyValue = sel;}

  protected:
	void addItem(std::string const& key, std::string const& value);
	void fillList();
	int itemNum;
	MDFile* mdfile;
	KeyValueGroup* selectedKeyValue;
};

#endif // F13KEYVALUELIST_HEADER_INCLUDED
