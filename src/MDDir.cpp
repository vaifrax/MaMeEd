
#ifdef WIN32
#include <windows.h>
#endif // WIN32
//#include <dirent.h>

#include "MDDir.h"
#include "MDFile.h"

#include <iostream>
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
}
