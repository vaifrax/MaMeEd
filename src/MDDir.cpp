
#ifdef WIN32
#include <windows.h>
#endif // WIN32
//#include <dirent.h>

#include "MDDir.h"
#include "MDFile.h"

#include <iostream>
#include <fstream>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode)  (((mode) & S_IFMT) == S_IFREG)
#endif

/*static*/ bool MDDir::isFile(const char* pathName) {
	struct stat myStat;
	return (stat(pathName, &myStat) == 0) && (S_ISREG(myStat.st_mode));
}

/*static*/ bool MDDir::isDirectory(const char* pathName) {
	struct stat myStat;
	return (stat(pathName, &myStat) == 0) && (S_ISDIR(myStat.st_mode));
}


// path to a directory or a file
MDDir::MDDir(std::string const& path) {
	// is it the path to a file?
	if (isFile(path.c_str())) {
		// it is a file
		size_t p=path.find_last_of("/\\");
		if (p == std::string::npos) {
			return; // error
		} else {
			dirPath = path.substr(0, path.size() - p - 1);
			initiallySelectedFileName = path.substr(p);
		}
	} else {
		// not a file; strip tailing / or \ if it exists
		char lastCh = path.back();
		if ((lastCh == '/') || (lastCh == '\\')) {
			dirPath = path.substr(0, path.size() - 1);
		} else {
			dirPath = path;
		}
	}

	// TODO: dirPath could also be just the drive, e.g. "C:", which currently fails ??
	if (!isDirectory(dirPath.c_str())) {
		dirPath = "";
		return; // error
	}

	// list all subdirectories(TODO?) and files in this directory
	#ifdef WIN32
	HANDLE hFind;
	WIN32_FIND_DATA FindData;
	hFind = FindFirstFile((dirPath + "/*").c_str(), &FindData);
	if (hFind == INVALID_HANDLE_VALUE) {
		dirPath = "";
		return; // error
	}
	MDFile* f = new MDFile(FindData.cFileName);
	files.push_back(f);
	while (FindNextFile(hFind, &FindData)) {
		f = new MDFile(FindData.cFileName);
		files.push_back(f);
	}
	FindClose(hFind);
	#endif // WIN32

	// read properties from database
	dbFileName = dirPath + "/.metadata.mmd";
	readFromFile(); // Marcel's Metadata Database (file format)
}

MDDir::~MDDir() {
	for (std::vector<MDFile*>::iterator i=files.begin(); i!=files.end(); ++i) {
		delete (*i);
	}
}

// TODO: for performance: use an additional map<string fileName, int arrayIndex> ??
MDFile* MDDir::getMDFile(std::string const& fileName) const {
	for (std::vector<MDFile*>::const_iterator i=files.begin(); i!=files.end(); ++i) {
		if ((*i)->getName() == fileName) return *i;
	}
	return NULL;
};

// first token is key, second value
// '=' are treated as whitespaces (so they can be used)
// whitespaces are ignored (before and after value)
// value containing whitespaces can be enclosed in "" (meaning " are ignored)
// key has to start with alphabetical value (ignored otherwise)
bool MDDir::processLine(std::string line) {
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
		tmpFile = getMDFile(sectionName);

		return true;
	}

	int posEqual = line.find_first_of('=');
	if (posEqual == std::string::npos) return false;

	std::string key = line.substr(0, posEqual);
	std::string value = line.substr(posEqual+1);

	if (value.size() && tmpFile) {
		//properties[key] = value;
		//tmpPropFile->addKeyValue(key, value);
		tmpFile->addKeyValue(key, value);
		return true;
	} else {
		return false;
	}
}

bool MDDir::readFromFile() {
	std::ifstream confFile(dbFileName);
	if (!confFile.is_open()) {
		std::cerr << "Error loading database file " << dbFileName << std::endl;
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
