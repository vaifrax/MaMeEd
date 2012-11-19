#include "MCore.h"
#include "MDDir.h"

MCore::MCore() : mddir(NULL) {
}

MCore::~MCore() {
	if (mddir) delete mddir;
}

bool MCore::openDir(std::string const& path) {
	MDDir* newMddir = new MDDir(path);
	if (newMddir->getDirPath().empty()) {
		delete newMddir;
		return false; // error
	}

	if (mddir) {
		mddir->writeToFile();
		delete mddir;
	}
	mddir = newMddir;

	return true;
}
