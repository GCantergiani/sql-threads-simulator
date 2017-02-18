#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <reader.h>
#include <valida.h>
#include <opera.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * * argv) {

  char * nthread;
  char * fqueries;
  char * out;
  char * w;

  int threadflag = 0;
  int queriesflag = 0;
  int outflag = 0;
  int wflag = 0;

  int index;
  int c;

  //Elimino los errores por defecto 
  opterr = 0;

  while ((c = getopt(argc, argv, "t:q:o:w:")) != -1)
    switch (c) {
    case 't':
      threadflag = 1;
      nthread = optarg;
      break;

    case 'q':
      queriesflag = 1;
      fqueries = optarg;
      break;

    case 'w':
      wflag = 1;
      w = optarg;
      break;

    case 'o':
      outflag = 1;
      out = optarg;
      break;

    case '?':
      if (optopt == 't') // No ingresa argumento -t
      {
        perror("Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
        return -4;
      }

      if (optopt == 'q') // No ingresa argumento i
      {
        perror("Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
        return -5;
      } else if (optopt == 'o') // No ingresa argumento o
      {
        perror("Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
        return -6;
      }

      //Si ingresa una opcion que no esta entre las validas.
      else if (isprint(optopt))
        fprintf(stderr, "Opción `[%c]' no válida", optopt);
      else
        fprintf(stderr, "Opción `[%c]' no válida", optopt);
      return -10;

    default:
      abort();
    }

  //Errores si no ingresa argumento

  if (threadflag == 0) // No ingresa argumento -t
  {
    fprintf(stderr, "Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
    //perror ("Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
    //cout<<"soy el 1";
    return -1; //OK
  }

  if (queriesflag == 0) // No ingresa argumento -i
  {
    fprintf(stderr, "Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
    //perror ("Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
    //cout<<"soy el 2";
    return -2; //OK
  }

  if (outflag == 0) // No ingresa argumento -o
  {
    fprintf(stderr, "Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
    //perror ("Uso válido: ./minisql -t [hebra] -q [archivo_consulta] -o [archivo_salida]");
    //cout<<"soy el 3";
    return -3; //OK
  }

  //Paso los errores del 1 al 6, reviso el 7, 8
  Reader reader;

  int exists = reader.existe(fqueries);
  if (exists == -1) {
    fprintf(stderr, "El archivo: `%s' no existe", fqueries);
    //perror (El archivo: `[archivo_consulta]' no existe);
    return -7;
  }

  //Valido el archivo de entrada de consultas como las tablas  
  Valida valida;

  //Valido si se cumple el formato del archivo de consultas.
  int esposible = valida.consultas(fqueries);
  if (esposible == -1) {
    //formato invalido
    fprintf(stderr, "Imposible reconocer formato de archivo: `%s'", fqueries);
    //perror("Imposible reconocer formato de archivo: `[archivo_consulta]'");
    return -9;
  }

  if (esposible == -8) {
    //no texto
    //Los Archivos de las tablan no cumplen el formato correcto.
    fprintf(stderr, "El archivo: `%s' no es válido", fqueries);
    //perror("El archivo: `[archivo_consulta]' no es válido");
    return -8;
  }

  if ((atoi(nthread)) <= 0) {
    perror("El argumento de -t debe ser un entero mayor que cero");
    return -11;
  }

  //A diferencia de la anterior valida overflow
  esposible = valida.nhebras(nthread, fqueries);
  if (esposible == -1) {
    perror("El argumento de -t debe ser un entero mayor que cero");
    return -11;
  }

  //Existe la carpeta bd??  
  std::string dirtmp;
  dirtmp = "./bd";
  esposible = valida.esdirectorio(dirtmp.c_str());
  if (esposible == -1) {
    fprintf(stderr, "No se encontró bases de datos en directorio ./bd/");
    return -12;
  }

  //Valido si todas las tablas se llaman .bd
  esposible = valida.tablas();
  if (esposible == -1) {
    //Los Archivos de las tablan no cumplen el formato correcto.
    fprintf(stderr, "No se encontró bases de datos en directorio ./bd/");
    return -12;
  }

  Opera opera;
  //Cargo los datos de la DB en la estructura    
  opera.Loadtables();
  //Reparto las consultas por cantidad de hebras
  opera.RoundRobin(atoi(nthread), fqueries);

  if (wflag == 1) {
    opera.showtime(atoi(w));
  }

  opera.start(atoi(nthread), out);

  return 0;
}