// MetaData File
// all metadata for a single file (or subdirectory)

#include <string>

class MDFile {
  public:
	MDFile(std::string path); // absolute path to the file
	bool const isDirectory() {return isDir;}
	std::string const& getName() {return path;}

  protected:
	std::string path;
	bool isDir;
};
