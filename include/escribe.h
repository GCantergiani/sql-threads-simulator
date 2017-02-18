#ifndef ESCRIBRE_H
#define ESCRIBRE_H

#include <string>
#include <fstream>
#include <vector>


using namespace std;

class Escribe{
	
public:
	Escribe();
	~Escribe();

	void escribir(std::vector<std::string> salida,const char *out);
};

#endif
