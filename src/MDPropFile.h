#include <fstream>
#include <map>

//class MDProperty;

class MDPropFile {
  public:
	MDPropFile() {};
	MDPropFile(std::string fileName);
	void writeToFile(std::ofstream& os);
	void addKeyValue(std::string key, std::string value) {properties[key] = value;}
	std::map<std::string, std::string>& const getProperties() {return properties;}

  protected:
	std::string fileName;
	//std::map<std::string, MDProperty> properties;
	std::map<std::string, std::string> properties;
};
