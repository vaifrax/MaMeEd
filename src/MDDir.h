// MetaData Directory
// all metadata for one set of files

#include <string>
#include <vector>
class MDFile;

class MDDir {
  public:
	MDDir(std::string const& path); // path to a directory or a file; if dirPath is empty, an error occured
	std::string const& getDirPath() const {return dirPath;}
	std::string const& getInitiallySelectedFileName() const {return initiallySelectedFileName;}
	std::vector<MDFile*> const& getFiles() const {return files;}

	static bool isFile(const char const* pathName);
	static bool isDirectory(const char const* pathName);

  protected:
	std::string dirPath; // absolute path to directory, e.g. "C:\Photos\2012" or "~/photos/2012", i.e. without a final '\' or '/'
	std::string initiallySelectedFileName; // without path, just file name

	std::vector<MDFile*> files;
};
