// abstract class (interface), defining functionality exposed to main app
// this is done to facilitate exchanging the GUI

#ifndef MGUI_HEADER_INCLUDED
#define MGUI_HEADER_INCLUDED

#include <stdlib.h>
#include <string>

class MCore;

class MGUI {
  public:
	MGUI(MCore* mCore) {this->mCore = mCore; argc = 0; argv = NULL;};
	virtual ~MGUI() {};
	virtual void openDir(std::string) {};
	virtual void setCmdParams(int argc, char* argv[]) {this->argc = argc; this->argv = argv;};
	virtual int showWindow() {return 0;}; // returns exit code

  protected:
	int argc;
	char** argv;
	MCore* mCore;
};

#endif // MGUI_HEADER_INCLUDED
