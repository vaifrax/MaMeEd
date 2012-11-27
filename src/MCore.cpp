#include "MCore.h"
#include "MDDir.h"

MCore::MCore(char* argv0) : mddir(NULL) {
	std::string configFileName(argv0);
	configFileName = configFileName.substr(0, configFileName.size()-3) + "ini";
	config = new MConfig(configFileName);
	config->readFromFile();
}

MCore::~MCore() {
	if (mddir) delete mddir;
	delete config;
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
