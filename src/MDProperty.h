// a single (meta data) property of a file

#include <stdlib.h>
#include <string>
class MDFile;

class MDProperty {
  public:
	//MDProperty(MDFile& file, std::string const& key, std::string const& value);
	MDProperty(std::string const& key, std::string const& value);
	//std::string const getStringValue() {return strValue;};
	std::string key;
	std::string value;

	enum MDPropertyType {UNKNOWN, FLOAT32, INT32, UINT32, STRING, DATETIME} type;

  protected:
	//MDFile& file;
	//std::string strValue;
};
