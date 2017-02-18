#include <iostream> 
#include <sstream> 
#include <stdlib.h> 
#include <pthread.h> 
#include <vector> 
#include <string> 
#include <semaphore.h>
#include "opera.h"
#include "reader.h"
#include "escribe.h"

//Vector de mutex
std::vector < pthread_mutex_t > Opera::mutetables;

//Mutex al escribir y multiples selects
pthread_mutex_t escribo;
pthread_mutex_t selects;

//Vector donde guardo la salida.
std::vector < std::string > salida;

//Consultas por hebra
std::vector < std::vector < std::string > > ThreadQueries;

//Estructura de la DB
//Tuplas
struct Tuple {
  int id;
  std::string att;
};
//Tablas
struct Table {
  std::string nametable;
  std::vector < Tuple > tuples;

};
//Vector de tablas
std::vector < Table > tables;

int entres;
sem_t x, y;

Opera::Opera() {}

Opera::~Opera() {}

//Me dice si el indice ya existe
int Opera::idexiste(int nutabla,
  const int id) {

  for (unsigned int j = 0; j < tables[nutabla].tuples.size(); ++j) {

    if (tables[nutabla].tuples[j].id == id) {
      return 1;
    }
  }

  return -1;

}

int Opera::start(int nhebras,
  const char * out) {
  void * status;
  //Arreglo las hebras.
  pthread_t callThd[nhebras];
  //vector de mutex

  unsigned int ntablas = tables.size();

  for (unsigned int i = 0; i < ntablas; ++i) {
    pthread_mutex_t mutex;
    pthread_mutex_init( & mutex, NULL);
    Opera::mutetables.push_back(mutex);
  }

  sem_init( & x, 1, 1);

  //Para que no escriban dos en el vector de salida
  pthread_mutex_t escribo;
  pthread_mutex_init( & escribo, NULL);
  //Para que s epuedan multiples selects
  pthread_mutex_t selects;
  pthread_mutex_init( & selects, NULL);
  entres = 0;

  //Atributos de las hebras (prioridades and stuff)
  pthread_attr_t attr;

  //Se inicializan deslos atributos
  pthread_attr_init( & attr);
  // uno de los atributos sera que sea JOINABLE, con lo que podremos esperar por su ejecución(C&P)
  pthread_attr_setdetachstate( & attr, PTHREAD_CREATE_JOINABLE);

  int pthread_create(pthread_t * thread,
    const pthread_attr_t * attr, void * ( * start_routine)(void * ), void * arg);

  //se crean NUMTHRDS hebras, cada una de las cuales comienza con la rutina execute
  //como la estructura esta definida globalmente, no es necesario pasarsela como argumento a pthread_create
  //int * pasito;
  for (int j = 0; j < nhebras; j++) {

    size_t pasito = j;
    if (pthread_create( & callThd[j], & attr, & Opera::execute, (void * ) pasito) != 0) {
      perror("Error del sistema");
      return -14;
    }

  }
  //Destruyo atributos
  pthread_attr_destroy( & attr);

  // como las hebras fueron creadas JOINABLE, podemos esperar con ellas con la funcion pthread_join
  for (unsigned l = 0; l < nhebras; l++) {
    pthread_join(callThd[l], & status);
  }

  //De aqui en adelante no se continuara hasta que todas las hebras hayan terminado su ejecución
  //Destruyo mutex
  for (unsigned int h = 0; h < ntablas - 1; h++) {
    //pthread_mutex_destroy(&Opera::mutetables[h]);
  }
  pthread_mutex_destroy( & escribo);
  pthread_mutex_destroy( & selects);

  Escribe escritor;

  escritor.escribir(salida, out);

}

void * Opera::execute(void * args) {
    bool flag;
    size_t soylahebra;
    soylahebra = (size_t) args;
    Reader token;

    //Para cada linea de la hebra.
    for (unsigned int a = 0; a < ThreadQueries[soylahebra].size(); a++) {

      //cout<<a;

      flag = false;

      //La linea A la comparo con todas las tablas para saber cual hago el mutex
      for (unsigned int b = 0; b < tables.size(); b++) {
        //Separo por palabra.
        std::vector < std::string > word;
        token.tokenize(ThreadQueries[soylahebra][a], word, " ");

        //Si el el nombre de la tabla es igual.
        if (word[1].compare(tables[b].nametable) == 0) {

          flag = true;

          //Si la consulta es select
          if (word[0].compare("select") == 0) {
            //Para que puedan entrar multiples selects.
            //Problema escritor lector (Stallings 242)
            //pthread_mutex_lock(&selects);
            sem_wait( & x);
            entres++;
            if (entres == 1) {
              pthread_mutex_lock( & Opera::mutetables[b]);
            }
            //pthread_mutex_unlock(&selects);
            sem_post( & x);

            Opera Sopera;
            std::string tmp;
            std::string Cid;
            unsigned int flagselect = 0;

            tmp.append(word[2]);
            tmp.erase(3, tmp.size() - 1);
            if (tmp.compare("att") == 0) {
              flagselect = 1;
              //Si es att debo borrar los tres primeros caracteres "att="
              word[2].erase(0, 4);

              //Ahora debo juntar todo el select en un string.
              std::string Cselect;
              for (unsigned int cs = 2; cs < word.size(); cs++) {
                if (cs != word.size()) {
                  Cselect += word[cs] + " ";
                } else {
                  Cselect += word[cs];
                }

              }

              Cselect.erase(0, 1);
              Cselect.erase(Cselect.size() - 2, Cselect.size() - 1);

              std::vector < std::string > salidita;

              for (int t = 0; t < tables[b].tuples.size(); ++t) {
                if (Cselect.compare(tables[b].tuples[t].att) == 0) {

                  std::string idstring;
                  std::stringstream out;
                  out << tables[b].tuples[t].id;
                  idstring = out.str();

                  pthread_mutex_lock( & escribo);
                  //salida.push_back(ThreadQueries[soylahebra][a] + ": OK, ("+ idstring + "," + Cselect + ")" );		    
                  salidita.push_back("(" + idstring + "," + Cselect + ")");
                  pthread_mutex_unlock( & escribo);

                }
              }

              std::string saliditatmp;

              for (int st = 0; st < salidita.size(); ++st) {
                if (st != salidita.size() - 1) {
                  saliditatmp += salidita[st] + ",";
                }
                if (st == salidita.size() - 1) {
                  saliditatmp += salidita[st];
                }
              }

              //cout<<"Mi tamaño es: "<<saliditatmp.size();	
              if (saliditatmp.size() != 0) {
                pthread_mutex_lock( & escribo);
                salida.push_back(ThreadQueries[soylahebra][a] + ": OK, " + saliditatmp);
                pthread_mutex_unlock( & escribo);
              } else {
                pthread_mutex_lock( & escribo);
                salida.push_back(ThreadQueries[soylahebra][a] + ": OK, nulo");
                //salidita.push_back("("+ idstring + "," + Cselect + ")");
                pthread_mutex_unlock( & escribo);
              }

            } //final ATT select

            if (flagselect == 0) {

              //Verifico si el select es de una ID o un atributo
              tmp.append(word[2]);
              tmp.erase(2, tmp.size() - 1);
              if (tmp.compare("id") == 0) {
                Cid = word[2].substr(3, word[2].size());
              }

              //Si existe la ID puedo seleccionar
              if (Sopera.idexiste(b, atoi(Cid.c_str())) == 1) {
                int encontre;

                for (int k = 0; k < tables[b].tuples.size(); ++k) {

                  if (tables[b].tuples[k].id == atoi(Cid.c_str())) {
                    encontre = k;
                  }
                }

                std::string idstring;
                std::stringstream out;
                out << tables[b].tuples[encontre].id;
                idstring = out.str();

                //Si se pudo seleccionar un elemento → OK, (id0 , att0 ), ..., (idn , attn )
                pthread_mutex_lock( & escribo);

                salida.push_back(ThreadQueries[soylahebra][a] + ": OK, (" + idstring + "," + tables[b].tuples[encontre].att + ")");

                pthread_mutex_unlock( & escribo);

              } else //No existe la ID no puedo seleccionar.
              {
                pthread_mutex_lock( & escribo);
                salida.push_back(ThreadQueries[soylahebra][a] + ": OK, nulo.");
                pthread_mutex_unlock( & escribo);
              }
            } //Final ID select 

            //Para que puedan entrar multiples selects. (cerrar)
            //pthread_mutex_lock(&selects);
            sem_wait( & x);
            entres--;
            if (entres == 0) {
              pthread_mutex_unlock( & Opera::mutetables[b]);
            }
            //pthread_mutex_unlock(&selects);
            sem_post( & x);

          } //if del select  

          //Mutex por tabla.
          pthread_mutex_lock( & Opera::mutetables[b]);

          //Si la consulta es insert
          if (word[0].compare("insert") == 0) {
            Opera Iopera;
            std::string Cinsert;

            word[2].erase(0, 1);
            word[2].erase(word[2].size() - 1, word[2].size());

            int entre = Iopera.idexiste(b, atoi(word[2].c_str()));

            if (entre == 1) {
              //Mutex para escribir en salida
              pthread_mutex_lock( & escribo);
              salida.push_back(ThreadQueries[soylahebra][a] + ": Error, Entrada duplicada.");
              pthread_mutex_unlock( & escribo);

            } else //La ID no existe puedo insertar
            {

              std::string Cselect;
              for (unsigned int cs = 3; cs < word.size(); cs++) {
                if (cs != word.size()) {
                  Cselect += word[cs] + " ";
                } else {
                  Cselect += word[cs];
                }

              }

              Cselect.erase(0, 1);
              Cselect.erase(Cselect.size() - 3, Cselect.size() - 1);

              //Mutex para escribir en salida
              pthread_mutex_lock( & escribo);

              unsigned int posi = tables[b].tuples.size();
              tables[b].tuples.resize(posi + 1);
              tables[b].tuples[posi].id = atoi(word[2].c_str());
              tables[b].tuples[posi].att = Cselect;
              salida.push_back(ThreadQueries[soylahebra][a] + ": OK.");

              pthread_mutex_unlock( & escribo);

            }

          } //Final del insert

          //Si la consulta es delete
          if (word[0].compare("delete") == 0) {
            //Reader Dread;
            Opera Dopera;
            //word[0] : Operacion
            //word[2] : id donde voy a insertar.
            std::string Cdelete;
            // Le borro el primer caracter
            word[2].erase(0, 1);
            //Le borro el ultimo caracter.
            word[2].erase(word[2].size() - 1, word[2].size());
            //word[2] : id donde voy a deletear exito.s

            //Existe la ID puedo borrar
            if (Dopera.idexiste(b, atoi(word[2].c_str())) == 1) {

              //Cuando se borro correctamente y existe
              pthread_mutex_lock( & escribo);
              for (int k = 0; k < tables[b].tuples.size(); ++k) {
                if (tables[b].tuples[k].id == atoi(word[2].c_str())) {
                  tables[b].tuples.erase(tables[b].tuples.begin() + k);
                }
              }
              salida.push_back(ThreadQueries[soylahebra][a] + ": OK.");
              pthread_mutex_unlock( & escribo);
            } else // NO existe la ID no puedo borrar
            {
              //Mutex para escribir
              pthread_mutex_lock( & escribo);
              salida.push_back(ThreadQueries[soylahebra][a] + ": OK, nulo.");
              pthread_mutex_unlock( & escribo);
            }

          } //Final del update

          //Si la consulta es update
          if (word[0].compare("update") == 0) {
            Opera Uopera;

            std::string Cupdate;

            // Le borro el primer caracter
            word[2].erase(0, 1);
            //Le borro el ultimo caracter.
            word[2].erase(word[2].size() - 1, word[2].size());
            //word[2] : id donde voy a updatear.

            //Existe la ID puedo Actualizar
            if (Uopera.idexiste(b, atoi(word[2].c_str())) == 1) {

              //Ahora debo juntar todo el select en un string.
              std::string Cupdate;
              for (unsigned int cs = 3; cs < word.size(); cs++) {
                if (cs != word.size()) {
                  Cupdate += word[cs] + " ";
                } else {
                  Cupdate += word[cs];
                }
              }

              Cupdate.erase(0, 1);
              Cupdate.erase(Cupdate.size() - 3, Cupdate.size() - 1);

              //Cuando la tabla esta ya actulizada
              pthread_mutex_lock( & escribo);

              for (int k = 0; k < tables[b].tuples.size(); ++k) {
                if (tables[b].tuples[k].id == atoi(word[2].c_str())) {
                  tables[b].tuples[k].att = Cupdate;
                }
              }

              //Escribo en mi vector Log
              salida.push_back(ThreadQueries[soylahebra][a] + ": OK.");
              pthread_mutex_unlock( & escribo);

            } else // No existe la ID no puedo actulizar.
            {
              pthread_mutex_lock( & escribo);
              salida.push_back(ThreadQueries[soylahebra][a] + ": OK, nulo.");
              pthread_mutex_unlock( & escribo);
            }

          } //final del update  

          //Vuelvo a dejar libre el mutex por tabla
          pthread_mutex_unlock( & Opera::mutetables[b]);

        } //Si encuentro la tabla    

      } //For comparo tablas para el mutex

      //Si no se encuentra la tabla
      if (flag == false) {
        pthread_mutex_lock( & escribo);
        salida.push_back(ThreadQueries[soylahebra][a] + ": Error, Tabla no encontrada");
        pthread_mutex_unlock( & escribo);
      }
    } //Para cada linea de la hebra

    pthread_exit((void * ) 0);

  } //void

int Opera::showtime(int w) {

  cout << "Mi base de datos tiene " << tables.size() << " tablas." << endl;

  for (unsigned int i = 0; i < tables.size(); ++i) {
    if (w == 1 || w == 2) {
      cout << "La tabla " << i << " se llama: " << tables[i].nametable << endl;
      cout << "Tiene " << tables[i].tuples.size() << " filas" << endl;
    }

    if (w == 2) {
      for (unsigned int j = 0; j < tables[i].tuples.size(); ++j) {
        cout << "id: " << tables[i].tuples[j].id << " att: " << tables[i].tuples[j].att << endl;
      }
      cout << endl;
    }

  }

  if (w == 3) {
    cout << "Tengo " << ThreadQueries.size() << " hebras" << endl;
    for (unsigned int i = 0; i < ThreadQueries.size(); ++i) {
      cout << "La hebra " << i << " posee " << ThreadQueries[i].size() << " consultas" << endl;
      for (int j = 0; j < ThreadQueries[i].size(); ++j) {
        cout << ThreadQueries[i][j] << endl;
      }
      cout << endl;
    }

  }
}

int Opera::Loadtables() {

  DIR * dp1;
  string dir;
  dir = "bd";
  string tmp;
  vector < string > files;
  std::string filetmp;

  struct dirent * dirp;
  if ((dp1 = opendir(dir.c_str())) == NULL) {
    //cout << "Error(" << errno << ") opening " << dir << endl;
    return -1;
  }

  while ((dirp = readdir(dp1)) != NULL) {
    tmp.clear();
    tmp.append(dirp - > d_name);
    if (tmp.compare(".") != 0 && tmp.compare("..") != 0) {
      //Guardo el archivo sin el nombre db		
      filetmp = string(dirp - > d_name);
      filetmp.erase(filetmp.size() - 3, filetmp.size() - 1);
      //cout<<filetmp<<endl;

      files.push_back(filetmp);
    }
  }
  closedir(dp1);

  //files.size() me dice cuanta tablas tengo que cargar
  //Inicializo el vector tables.
  tables.resize(files.size());

  //Ahora leo y lleno los datos
  Reader reader;
  std::string tablasLeidos;

  for (unsigned int i = 0; i < files.size(); i++) {

    tablasLeidos.clear();
    //Agrego los nombres de las tablas
    tables[i].nametable.assign(files[i]);

    //cout<<tables[i].nametable<<endl;

    files[i].insert(0, "./bd/");
    files[i].insert(files[i].size(), ".bd");
    //cout<<files[i]<<endl;
    //Leo cada archivo
    tablasLeidos = reader.leo(files[i]);

    std::vector < std::string > Listaconsultas;
    reader.tokenize(tablasLeidos, Listaconsultas, "\n");

    //Hago el espacio para las tuplas.
    tables[i].tuples.resize(Listaconsultas.size());

    std::vector < std::string > ::const_iterator it = Listaconsultas.begin();
    std::vector < std::string > ::const_iterator end = Listaconsultas.end();

    unsigned int counter = 0;

    for (; it != end; ++it) {
      std::string linea = * it;
      std::vector < std::string > Listaconsultas;
      reader.tokenize(linea, Listaconsultas, " ");

      //Agrego el id.
      tables[i].tuples[counter].id = atoi(Listaconsultas[0].c_str());

      std::string tmp;
      for (unsigned int j = 1; j < Listaconsultas.size(); j++) {
        if (j == 1) {
          tmp = Listaconsultas[j];
        }

        if (j != 1) {
          tmp = tmp + " " + Listaconsultas[j];
        }
      }

      //Guardo los att
      tables[i].tuples[counter].att = tmp;

      counter++;
    }

  }

}

int Opera::RoundRobin(const int nhebras,
  const char * consultas) {
  Reader reader;

  ThreadQueries.resize(nhebras);

  std::string consultasLeidos = reader.leo(consultas);

  std::vector < std::string > Listaconsultas;
  reader.tokenize(consultasLeidos, Listaconsultas, "\n");

  for (int i = 0; i < Listaconsultas.size(); ++i) {
    ThreadQueries[i % nhebras].push_back(Listaconsultas[i]);
  }

}