// a single (meta data) property of a file

#include <stdlib.h>
#include <string>
class MDFile;

class MDProperty {
  public:
	MDProperty(MDFile& file);
	std::string const getStringValue() {return strValue;};

	enum MDPropertyType {UNKNOWN, FLOAT32, INT32, UINT32, STRING, DATETIME};

  protected:
	MDFile& file;
	MDPropertyType type;
	std::string strValue;
};
