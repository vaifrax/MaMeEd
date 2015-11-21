#ifndef F13FILELIST_HEADER_INCLUDED
#define F13FILELIST_HEADER_INCLUDED

#include "FL/Fl_Scroll.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Button.H"
#include <string>
#include <list>

class MDFile;
class MDDir;

class FileGroup : public Fl_Group {
  public:
	FileGroup(int X, int Y, int W, int H, const char* fileName, MDFile* mdf, const char* dateStr, bool isDirectory, int index, const char* L=0);
	const char* getFileName() const {return nameBox->label();}
	int handle(int eventn);
	MDFile* mdf;
	int index; // index of item list, assuming no files are removed
  protected:
	Fl_Button *thumbnailButton;
	Fl_Box *nameBox;
	Fl_Box *dateTimeBox;
};

class F13FileList : public Fl_Scroll {
  public:
	F13FileList(int X, int Y, int W, int H, MDDir const* mddir = NULL);
	FileGroup* getActiveFile(int offset = 0) const; // offset for previous/next file, returns NULL if it doesn't exist
//	const char* getActiveFileName(int offset = 0) const;
	void setActiveFile(FileGroup* sel);
	void setActiveFileNext();
	void clearSelection() {selectedFiles.clear();}
	const std::list<FileGroup*>& getSelectedFiles() {return selectedFiles;};
	//int handle(int eventn);

	MDDir const* getMDDir() const {return mddir;};

	void resize(int X, int Y, int W, int H);

	static int thumbnailSize;

  protected:
	void addItem(std::string const& fileName, std::string const& dateStr, bool isDirectory, MDFile* mdf);
	void fillList();
	int itemNum;
	MDDir const* mddir;
	FileGroup* activeFile;
	std::list<FileGroup*> selectedFiles;
};

#endif // F13FILELIST_HEADER_INCLUDED
