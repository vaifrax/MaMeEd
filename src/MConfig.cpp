#include "MConfig.h"

#include <iostream>
#include <fstream>
#include <assert.h>

// path to config file
MConfig::MConfig(std::string const& path) : configFileName(path) {
}

MConfig::~MConfig() {
	for (std::vector<DefaultKey*>::iterator i=keys.begin(); i!=keys.end(); ++i) {
		delete (*i);
	}
}

// TODO: for performance: use an additional map<string fileName, int arrayIndex> ??
DefaultKey* MConfig::getDefaultKey(std::string const& keyName) const {
	for (std::vector<DefaultKey*>::const_iterator i=keys.begin(); i!=keys.end(); ++i) {
		if ((*i)->name == keyName) return *i;
	}
	return NULL;
};

// first token is key, second value
// '=' are treated as whitespaces (so they can be used)
// whitespaces are ignored (before and after value)
// value containing whitespaces can be enclosed in "" (meaning " are ignored)
// key has to start with alphabetical value (ignored otherwise)
bool MConfig::processLine(std::string line) {
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
		std::string sectionName = line.substr(1, line.size()-2);
		tmpKey = new DefaultKey();
		tmpKey->name = sectionName;
		keys.push_back(tmpKey);
		return true;
	}

	int posEqual = line.find_first_of('=');
	if (posEqual == std::string::npos) return false;

	std::string key = line.substr(0, posEqual);
	std::string value = line.substr(posEqual+1);

	if (value.size() && tmpKey) {
		if (key == "description") tmpKey->description = value;
		if (key == "type") {
			if (value == "STRING") tmpKey->type = DefaultKey::STRING;
			if (value == "INTEGER") tmpKey->type = DefaultKey::INTEGER;
			if (value == "FLOAT") tmpKey->type = DefaultKey::FLOAT;
		}
		if (key == "order") tmpKey->order = DefaultKey::TOP;

		return true;
	} else {
		return false;
	}
}

bool MConfig::readFromFile() {
	std::ifstream confFile(configFileName);
	if (!confFile.is_open()) {
		std::cerr << "Error loading config file " << configFileName << std::endl;
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

/*
bool MDDir::writeToFile() const {
	std::ofstream os;

	os.open(dbFileName.c_str());
	if (!os) return false; // file couldn't be opened
	os << "# meta data for files in this folder (file written by MaMeEd)\n\n";

	//for (std::map<std::string, MDProperty>::iterator i=properties.begin(); i!=properties.end(); ++i) {
	//	os << i->first << '=' << i->second << '\n';
	//}

	//for (std::map<std::string, MDPropFile>::iterator i=propFiles.begin(); i!=propFiles.end(); ++i) {
	for (std::vector<MDFile*>::const_iterator i=files.begin(); i!=files.end(); ++i) {
		(*i)->writeToFile(os);
	}

	os.close();

	return true;
}
*/
