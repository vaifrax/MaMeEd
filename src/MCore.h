// the logic of the application

#ifndef MCORE_HEADER_INCLUDED
#define MCORE_HEADER_INCLUDED

#include <stdlib.h>
#include <string>

class MDDir;

class MCore {
  public:
	MCore();
	~MCore();

	bool openDir(std::string const& path);

	MDDir const* getMDDir() const {return mddir;};

  protected:
	MDDir* mddir;
};

#endif // MCORE_HEADER_INCLUDED
