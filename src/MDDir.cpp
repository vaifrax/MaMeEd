
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
MDDir::MDDir(std::string path) : path(path) {
	std::string selectedFileName;

	if (isFile(this->path.c_str())) {
		size_t p=path.find_last_of("/\\");
		if (p == std::string::npos) {
			// TODO: error
		} else {
			this->path = this->path.substr(0, this->path.size() - p - 1);
		}
	} else {
		char lastCh = this->path.back();
		if ((lastCh == '/') || (lastCh == '\\')) {
			this->path = this->path.substr(0, this->path.size() - 1);
		}
	}

	if (!isDirectory(this->path.c_str())) {
		// TODO: error
	}

	// list all directories and files in this directory
	#ifdef WIN32
	HANDLE hFind;
	WIN32_FIND_DATA FindData;
	hFind = FindFirstFile((this->path + "/*").c_str(), &FindData);
	MDFile* f = new MDFile(FindData.cFileName);
	files.push_back(f);
	while (FindNextFile(hFind, &FindData)) {
		f = new MDFile(FindData.cFileName);
		files.push_back(f);
	}
	FindClose(hFind);
	#endif // WIN32
}
