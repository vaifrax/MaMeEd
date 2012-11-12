#include "MDFile.h"
#include "MDDir.h"

MDFile::MDFile(std::string path) : path(path) {
	isDir = MDDir::isDirectory(this->path.c_str());
}
