// the logic of the application

#ifndef MCORE_HEADER_INCLUDED
#define MCORE_HEADER_INCLUDED

#include <stdlib.h>
#include <string>

#include "MConfig.h"
class MDDir;

class MCore {
  public:
	MCore(char* argv0);
	~MCore();

	bool openDir(std::string const& path);

	MDDir const* getMDDir() const {return mddir;};
	MConfig const* getConfig() const {return config;};

  protected:
	MDDir* mddir;
	MConfig* config;
};

#endif // MCORE_HEADER_INCLUDED
