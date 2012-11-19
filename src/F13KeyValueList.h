#ifndef F13KEYVALUELIST_HEADER_INCLUDED
#define F13KEYVALUELIST_HEADER_INCLUDED

#include "FL/Fl_Scroll.H"
#include <string>

class KeyValueGroup;
class MDFile;

class F13KeyValueList : public Fl_Scroll {
  public:
	F13KeyValueList(int X, int Y, int W, int H, MDFile* mdfile);
	void applyChangesOfSelectedKeyValue(); // data from gui -> database
	void setSelectedKeyValue(KeyValueGroup* selectedKeyValue);
	KeyValueGroup const* getSelectedKeyValue() const {return selectedKeyValue;};

	static void keyCallback(Fl_Widget *w, void *data);

  protected:
	void addItem(std::string const& key, std::string const& value);
	void fillList();
	int itemNum;
	MDFile* mdfile;

	KeyValueGroup* selectedKeyValue;
	std::string origSelectedKey; // original values before modifying them
	std::string origSelectedValue;
	static const std::string emptyString;
};

#endif // F13KEYVALUELIST_HEADER_INCLUDED
