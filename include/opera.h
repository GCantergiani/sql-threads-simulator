#ifndef OPERA_H
#define OPERA_H

#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <pthread.h>

#include <ctype.h> 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>


#include <sys/types.h>
#include <dirent.h>



using namespace std;

class Opera{
	
public:
	Opera();

	~Opera();
	//reparto las consultas por el numero de hebras
	int RoundRobin(const int nhebras,const char *consultas);
	//Cargo las tablas de /bd
	int Loadtables();
	//Para mostrar ( -w 1 | -w 2 | -w 3)
	int showtime(const int w);
	//
	int start(int nhebras,const char *out);
	//
	int idexiste(int nutabla, const int id);
	//Donde opero las consutlas
	static void* execute(void* args);
	//Escribo el vector final
	int escribe(std::string entrada);
	//Mutex
	static std::vector<pthread_mutex_t> mutetables;
	static std::vector<pthread_mutex_t> select;

private:

};

#endif
