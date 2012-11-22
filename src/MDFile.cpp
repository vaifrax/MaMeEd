#include "MDFile.h"
#include "MDDir.h"
#include "MDProperty.h"

MDFile::MDFile(std::string fileName, std::string const& dirPath) : fileName(fileName) {
	isDir = MDDir::isDirectory((dirPath + '/' + this->fileName).c_str());
}

void MDFile::writeToFile(std::ofstream& os) {
	if (properties.empty()) return;

	os << '[' << fileName << "]\n";
//	for (std::map<std::string, MDProperty>::iterator i=properties.begin(); i!=properties.end(); ++i) {
//	for (std::map<std::string, std::string>::iterator i=properties.begin(); i!=properties.end(); ++i) {
	for (std::vector<MDProperty*>::iterator i=properties.begin(); i!=properties.end(); ++i) {
//		os << i->first << '=' << i->second.getStringValue() << '\n';
		//os << i->first << '=' << i->second << '\n';
		if ((*i)->key.size()) { // ignore entries with empty keys
			os << (*i)->key << '=' << (*i)->value << '\n';
		}
	}
	os << '\n';
}

void MDFile::changeKeyValue(std::string const& oldKey, std::string const& newKey, std::string const& newValue) {
	if (oldKey.empty()) {
		addKeyValue(newKey, newValue);
// TODO: warn when double key!
		return;
	}

	for (std::vector<MDProperty*>::iterator i=properties.begin(); i!=properties.end(); ++i) { // todo: use additional map for faster access?
		if ((*i)->key == oldKey) {
			(*i)->key = newKey;
			(*i)->value = newValue;
		}
	}

	// for now, empty keys are kept; when storing, they are not written, i.e. get lost
}

void MDFile::addKeyValue(std::string key, std::string value) {
	properties.push_back(new MDProperty(key, value));
}
