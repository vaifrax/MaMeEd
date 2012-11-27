// configuration

#ifndef MCONFIG_HEADER_INCLUDED
#define MCONFIG_HEADER_INCLUDED

#include <string>
#include <vector>

class DefaultKey {
  public:
	enum KeyType{STRING, INTEGER, FLOAT};
	enum OrderEnum{TOP, BOTTOM};

	DefaultKey() : type(STRING), order(BOTTOM) {};

	std::string name;
	std::string description;
	KeyType type;
	OrderEnum order; // top or bottom for gui
};

class MConfig {
  public:
	MConfig(std::string const& path); // path to config file
	~MConfig();

	std::vector<DefaultKey*> const& getKeys() const {return keys;}
	DefaultKey* getDefaultKey(std::string const& keyName) const;

	bool readFromFile();
	//bool writeToFile() const;

  protected:
	bool processLine(std::string line);
	DefaultKey* tmpKey; // used during file reading

	std::string configFileName;
	std::vector<DefaultKey*> keys;
};

#endif // MCONFIG_HEADER_INCLUDED
