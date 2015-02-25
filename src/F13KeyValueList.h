#ifndef F13KEYVALUELIST_HEADER_INCLUDED
#define F13KEYVALUELIST_HEADER_INCLUDED

#include "FL/Fl_Scroll.H"
#include <string>

class KeyValueGroup;
class MDFile;
class MConfig;

class F13KeyValueList : public Fl_Scroll {
  public:
	F13KeyValueList(int X, int Y, int W, int H, MConfig const* config);
	~F13KeyValueList();
	void setMDFile(MDFile* mdfile);

	void applyChangesOfSelectedKeyValue(); // data from gui -> database
	void setSelectedKeyValue(KeyValueGroup* selectedKeyValue);
	KeyValueGroup const* getSelectedKeyValue() const {return selectedKeyValue;};

	static void keyCallback(Fl_Widget *w, void *data);

	void resize(int X, int Y, int W, int H);

	enum KeyType {DEFAULT_KEY, USER_KEY};

//protected:
	void setValueOrAddItem(std::string const& key, std::string const& value);

  protected:
	void addItem(std::string const& key, std::string const& value, std::string const& keyDescription = emptyString, KeyType keyType = USER_KEY);

	void fillList();
	int itemNum;
	MDFile* mdfile;
	MConfig const* config;

	KeyValueGroup* selectedKeyValue;
	std::string origSelectedKey; // original values before modifying them
	std::string origSelectedValue;
	static const std::string emptyString;
};

#endif // F13KEYVALUELIST_HEADER_INCLUDED
