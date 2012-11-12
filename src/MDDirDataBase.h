// small database for a single directory
// can be saved to or read from a file

#include <stdlib.h>
#include <string>
#include <map>

class MDDir;
//class MDProperty;
class MDPropFile;

class MDDirDataBase {
  public:
	MDDirDataBase(MDDir& dir);
	bool readFromFile();
	bool writeToFile();
	//std::string getString(std::string key);
	//std::string getString(char* key);
	MDPropFile& const getPropFile(std::string fileName) {return propFiles[fileName];};

  protected:
	//std::map<std::string, MDProperty> properties;
	std::map<std::string, MDPropFile> propFiles;

	bool processLine(std::string line);

	MDDir& dir;
	std::string fileName;

  private:
	//MDPropFile* tmpPropFile; // used during file reading
	std::string tmpSection;
};
