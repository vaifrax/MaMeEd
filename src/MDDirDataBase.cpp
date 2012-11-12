#include "MDDirDataBase.h"
#include "MDDir.h"
#include "MDPropFile.h"

#include <iostream>
#include <fstream>
#include <assert.h>

MDDirDataBase::MDDirDataBase(MDDir& dir) : dir(dir) {
	fileName = dir.getPath() + "/.metadata.mmd";
	readFromFile(); // Marcel's Metadata Database (file format)
}

// first token is key, second value
// '=' are treated as whitespaces (so they can be used)
// whitespaces are ignored (before and after value)
// value containing whitespaces can be enclosed in "" (meaning " are ignored)
// key has to start with alphabetical value (ignored otherwise)
bool MDDirDataBase::processLine(std::string line) {
	// ignore empty lines
	if (!line.size()) return false;

	// ignore comment lines starting with ';','#' or '%'
	if (line.front() == ';') return false;
	if (line.front() == '%') return false;
	if (line.front() == '#') return false;

	// sections []
	if ((line.front() == '[') && (line.back() == ']')) {
		//tmpPropFile = new MDPropFile(line.substr(1, line.size()-2));
		///propFiles
		tmpSection = line.substr(1, line.size()-2);
		return true;
	}

	int posEqual = line.find_first_of('=');
	if (posEqual == std::string::npos) return false;

	std::string key = line.substr(0, posEqual);
	std::string value = line.substr(posEqual+1);

	if (value.size()) {
		//properties[key] = value;
		//tmpPropFile->addKeyValue(key, value);
		propFiles[tmpSection].addKeyValue(key, value);
		return true;
	} else {
		return false;
	}
}

bool MDDirDataBase::readFromFile() {
	std::ifstream confFile(fileName);
	if (!confFile.is_open()) {
		std::cerr << "Error loading database file " << fileName << std::endl;
		return false;
	}

	// process lines
	int lineNumber = 0;
	std::string line;
	while (std::getline(confFile, line)) {
		lineNumber++;
		if (!line.empty()) {
			processLine(line);
		}
	}

	confFile.close();
	return true;
}

bool MDDirDataBase::writeToFile() {
	std::ofstream os;

	os.open(fileName.c_str());
	if (!os) return false; // file couldn't be opened
	os << "# meta data for files in this folder (file written by MaMeEd)\n\n";

	//for (std::map<std::string, MDProperty>::iterator i=properties.begin(); i!=properties.end(); ++i) {
	//	os << i->first << '=' << i->second << '\n';
	//}

	for (std::map<std::string, MDPropFile>::iterator i=propFiles.begin(); i!=propFiles.end(); ++i) {
		i->second.writeToFile(os);
	}

	os.close();

	return true;
}
/*
std::string MDDirDataBase::getString(char* key) {
	return properties[key];
}

std::string MDDirDataBase::getString(std::string key) {
	return properties[key];
}
*/