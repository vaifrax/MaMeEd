// MetaData Directory
// all metadata for one set of files

#include <string>
#include <vector>
class MDFile;

class MDDir {
  public:
	MDDir(std::string path); // path to a directory or a file
	std::string const getPath() {return path;};

	static bool isFile(const char* pathName);
	static bool isDirectory(const char* pathName);

//  protected:
	std::string path; // absolute path, e.g. "C:\Photos\2012" or "~/photos/2012", i.e. without a final '\' or '/'
	std::vector<MDFile*> files;
};
