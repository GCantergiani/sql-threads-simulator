#ifndef VALIDA_H
#define VALIDA_h

#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <errno.h>
#include <limits.h>

#include <sys/types.h>
#include <dirent.h>
#include <iostream>

#include "reader.h"

using namespace std;

class Valida {

public:
	Valida();

	~Valida();

	//Valido si el numero de hebra es valido(overflow) y si es posible por las consultas
	int nhebras(const char *nhebras, const char *consultas);
	//Valido si el id de tabla es posible
	int idtablaposible(const char *idtabla);
	//Valido si las consultas poseen el formato
	int consultas(const char *consultas);
	//Valido si las tablas poseen el formato
	int tablas();
	int esdirectorio(const char *directorio);

private:

	char *end;
  	int tmp;

};

#endif 
