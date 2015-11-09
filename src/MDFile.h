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
	bool importEmbeddedMetadata(); // embedded infos, e.g. from file header or EXIF data

	MDProperty* MDFile::getPropertyByKey(std::string key);

	void setKeyValue(std::string key, double value, bool overwriteExisting=false);
	void setKeyValue(std::string key, long value, bool overwriteExisting=false);
	void setKeyValue(std::string key, std::string value, bool overwriteExisting=false);

	// if possible, provide a source where the information is coming from, e.g. "EXIF"
	void setKeyValueSrc(std::string key, double value, std::string source, bool overwriteExisting=false);
	void setKeyValueSrc(std::string key, long value, std::string source, bool overwriteExisting=false);
	void setKeyValueSrc(std::string key, std::string value, std::string source, bool overwriteExisting=false);

	void changeKeyValue(std::string const& oldKey, std::string const& newKey, std::string const& newValue);

	std::vector<MDProperty*> properties;

  protected:
	std::string fileName;
	std::string fullPath;
	std::string dateStr;
	bool isDir;
};
