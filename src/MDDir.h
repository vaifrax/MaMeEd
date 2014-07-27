// MetaData Directory
// all metadata for one set of files

#ifndef MDDIR_HEADER_INCLUDED
#define MDDIR_HEADER_INCLUDED

#include <string>
#include <vector>
class MDFile;

class MDDir {
  public:
	MDDir(std::string const& path); // path to a directory or a file; if dirPath is empty, an error occured
	~MDDir();
	std::string const& getDirPath() const {return dirPath;}
	std::string const& getInitiallySelectedFileName() const {return initiallySelectedFileName;}
	std::vector<MDFile*> const& getFiles() const {return files;}
	MDFile* getMDFile(std::string const& fileName) const;

	bool readFromFile();
	bool writeToFile() const;

	void importMetadataFromFiles(); // EXIF, etc.

	static bool isFile(const char const* pathName);
	static bool isDirectory(const char const* pathName);

  protected:
	bool processLine(std::string line);
	//std::string tmpSection; // used during file reading
	MDFile* tmpFile; // used during file reading

	std::string dirPath; // absolute path to directory, e.g. "C:\Photos\2012" or "~/photos/2012", i.e. without a final '\' or '/'
	std::string initiallySelectedFileName; // without path, just file name
	std::string dbFileName; // absolute path of database file name

	std::vector<MDFile*> files;
};

#endif // MDDIR_HEADER_INCLUDED
