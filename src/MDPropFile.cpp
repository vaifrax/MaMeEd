#include "MDPropFile.h"
//#include "MDProperty.h"
#include <string>

MDPropFile::MDPropFile(std::string fileName) : fileName(fileName) {
}

void MDPropFile::writeToFile(std::ofstream& os) {
	os << '[' << fileName << "]\n";
//	for (std::map<std::string, MDProperty>::iterator i=properties.begin(); i!=properties.end(); ++i) {
	for (std::map<std::string, std::string>::iterator i=properties.begin(); i!=properties.end(); ++i) {
//		os << i->first << '=' << i->second.getStringValue() << '\n';
		os << i->first << '=' << i->second << '\n';
	}
}
