#include "MDFile.h"
#include "MDDir.h"
#include "MDProperty.h"

MDFile::MDFile(std::string fileName) : fileName(fileName) {
	isDir = MDDir::isDirectory(this->fileName.c_str());
}

void MDFile::writeToFile(std::ofstream& os) {
	os << '[' << fileName << "]\n";
//	for (std::map<std::string, MDProperty>::iterator i=properties.begin(); i!=properties.end(); ++i) {
//	for (std::map<std::string, std::string>::iterator i=properties.begin(); i!=properties.end(); ++i) {
	for (std::vector<MDProperty*>::iterator i=properties.begin(); i!=properties.end(); ++i) {
//		os << i->first << '=' << i->second.getStringValue() << '\n';
		//os << i->first << '=' << i->second << '\n';
		os << (*i)->key << '=' << (*i)->value << '\n';
	}
}

void MDFile::addKeyValue(std::string key, std::string value) {
	properties.push_back(new MDProperty(key, value));
}
