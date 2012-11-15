#ifndef F13FILELIST_HEADER_INCLUDED
#define F13FILELIST_HEADER_INCLUDED

#include "FL/Fl_Scroll.H"
#include <string>

class MDDir;
class FileGroup;

class F13FileList : public Fl_Scroll {
  public:
	F13FileList(int X, int Y, int W, int H, MDDir const* mddir = NULL);
	FileGroup* getSelectedFile() {return selectedFile;}
	const char* getSelectedFileName() const;
	void setSelectedFile(FileGroup* sel) {selectedFile = sel;}

  protected:
	void addItem(std::string const& fileName);
	void fillList();
	int itemNum;
	MDDir const* mddir;
	FileGroup* selectedFile;
};

#endif // F13FILELIST_HEADER_INCLUDED
