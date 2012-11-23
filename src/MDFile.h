// MetaData File
// all metadata for a single file (or subdirectory)

#include <string>
#include <vector>
#include <fstream>

class MDProperty;

class MDFile {
  public:
	MDFile(std::string fileName, std::string const& dirPath); // file name (without complete path)
	bool const isDirectory() {return isDir;}
	std::string const& getName() {return fileName;}
	std::string const& getDateStr() {return dateStr;}
	//std::string const& getAbsPath() {return path;} // return complete absolute path
	void writeToFile(std::ofstream& os);
	void addKeyValue(std::string key, std::string value);

	void changeKeyValue(std::string const& oldKey, std::string const& newKey, std::string const& newValue);

	std::vector<MDProperty*> properties;

  protected:
	std::string fileName;
	std::string dateStr;
	bool isDir;
};
