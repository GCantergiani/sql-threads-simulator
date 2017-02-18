#ifndef READER_H
#define READER_H

#include <string>
#include <fstream>
#include <vector>
#include <stdlib.h> 


using namespace std;

class Reader{
	
public:
	Reader();
	~Reader();

	int existe(const std::string& file);

	std::string leo(const std::string& path) const;

	void tokenize(const string& str,vector<string>& tokens,const string& delimiters);	

private:

};

#endif
