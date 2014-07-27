#include "MDFile.h"
#include "MDDir.h"
#include "MDProperty.h"
#include <sys/types.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <time.h>
#include <sstream>

#include "ExifFile.h"

#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif


MDFile::MDFile(std::string fileName, std::string const& dirPath) : fileName(fileName) {
	//isDir = MDDir::isDirectory((dirPath + '/' + this->fileName).c_str());

	fullPath = dirPath + '/' + this->fileName;
	struct stat myStat;
	int sr = stat(fullPath.c_str(), &myStat);
	isDir = ((sr == 0) && (S_ISDIR(myStat.st_mode)));

	struct tm* clock = gmtime(&(myStat.st_mtime));	// Get the last modified time and put it into the time structure
std::stringstream sstream;
sstream << clock->tm_mday << '.' << clock->tm_mon << '.' << (clock->tm_year+1900);
dateStr = sstream.str();//std::string("") + clock->tm_mday + '.' + clock->tm_mon + '.' + (clock->tm_year+1900);
// clock->tm_year returns the year (since 1900)
// clock->tm_mon returns the month (January = 0)
// clock->tm_mday returns the day of the month
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

MDProperty* MDFile::getPropertyByKey(std::string key) {
	for (std::vector<MDProperty*>::iterator i=properties.begin(); i!=properties.end(); ++i) { // todo: use additional map for faster access?
		if ((*i)->key == key) return *i;
	}
	return NULL;
}

void MDFile::changeKeyValue(std::string const& oldKey, std::string const& newKey, std::string const& newValue) {
	if (oldKey.empty()) {
		setKeyValue(newKey, newValue);
// TODO: warn when double key!
		return;
	}

	for (std::vector<MDProperty*>::iterator i=properties.begin(); i!=properties.end(); ++i) { // todo: use additional map for faster access?
		if ((*i)->key == oldKey) {
			(*i)->key = newKey;
			(*i)->value = newValue;
			return;
		}
	}
	// for now, empty keys are kept; when storing, they are not written, i.e. get lost

	// must be a predefined key, add it now
	setKeyValue(newKey, newValue);
}

void MDFile::setKeyValue(std::string key, std::string value) {
	MDProperty* existingProp = getPropertyByKey(key);
	if (existingProp) {
		existingProp->value = value;
	} else {
		properties.push_back(new MDProperty(key, value));
	}
}

bool MDFile::importEmbeddedMetadata() {
	// extract extension and convert to upper case
	std::string ext;
	int fileNameStrLen = fileName.length();
	if (fileNameStrLen > 1) {
		std::string::size_type p = fileName.rfind('.', fileNameStrLen-1);
		if (p != std::string::npos) {
			ext = fileName.substr(p+1);
			for (unsigned int i=0; i<ext.length(); i++) ext[i] = toupper(ext[i]);
		}
	}

	// extension is upper case now, test for .JPG or .JPEG
	if ((ext == "JPG") || (ext == "JPEG")) {
		ExifFile ex(fullPath.c_str(), this);
	}

	return true;
}
