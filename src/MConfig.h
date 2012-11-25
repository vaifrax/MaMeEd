// configuration

#ifndef MCONFIG_HEADER_INCLUDED
#define MCONFIG_HEADER_INCLUDED

#include <string>
#include <vector>

class MConfig {
  public:
	MConfig(std::string const& path); // path to config file
	~MConfig();

	enum KeyType{STRING, INTEGER, FLOAT};
	struct KeyStruct {
		std::string name;
		std::string description;
	
	};


	std::vector<KeyStruct*> const& getKeys() const {return keys;}
	MDFile* getMDFile(std::string const& fileName) const;

	bool readFromFile();
	bool writeToFile() const;

  protected:
	bool processLine(std::string line);
	KeyType* tmpSection; // used during file reading

	std::vector<KeyStruct*> keys;
};

#endif // MCONFIG_HEADER_INCLUDED
