#ifndef F13FILELIST_HEADER_INCLUDED
#define F13FILELIST_HEADER_INCLUDED

#include "FL/Fl_Scroll.H"
#include <string>

class MDFile;
class MDDir;
class FileGroup;

class F13FileList : public Fl_Scroll {
  public:
	F13FileList(int X, int Y, int W, int H, MDDir const* mddir = NULL);
	FileGroup* getSelectedFile() {return selectedFile;}
	const char* getSelectedFileName() const;
	void setSelectedFile(FileGroup* sel) {selectedFile = sel;}

	MDDir const* getMDDir() const {return mddir;};

	void resize(int X, int Y, int W, int H);

	static int thumbnailSize;

  protected:
	void addItem(std::string const& fileName, std::string const& dateStr, bool isDirectory, MDFile* mdf);
	void fillList();
	int itemNum;
	MDDir const* mddir;
	FileGroup* selectedFile;
};

#endif // F13FILELIST_HEADER_INCLUDED
