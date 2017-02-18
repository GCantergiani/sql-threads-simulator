#include "valida.h"
#include "reader.h"
#include <iostream> 
#include <sstream> 
#include <stdlib.h> 
#include <sys/stat.h> 
#include <stdexcept>

Valida::Valida() {}

Valida::~Valida() {}

int Valida::esdirectorio(const char * directorio) {
  struct stat sb;

  if (stat(directorio, & sb) == 0 && S_ISDIR(sb.st_mode)) {
    return 1;
  }
  return -1;

}

int Valida::idtablaposible(const char * idtabla) {

  //Reviso si es un numero.
  istringstream iss(idtabla);

  int obj;
  iss >> ws >> obj >> ws;
  if (!iss.eof()) {
    //cout<<"No es numero 1"<<endl;
    return -1;
  }

  //Reviso si el ID tabla no produce Overflow
  tmp = strtol(idtabla, & end, 10);
  if ((LONG_MIN == tmp || LONG_MAX == tmp) && ERANGE == errno) {
    return -1;
  }

  //Reviso si el ID no es negativo
  int idtmp = atoi(idtabla);
  if (idtmp < 0) {
    return -1;
  }

  return 1;

}

int Valida::nhebras(const char * nhebras,
  const char * consultas) {

  istringstream iss(nhebras);
  int obj;
  iss >> ws >> obj >> ws;
  if (!iss.eof()) {
    return -1;
  }

  //Reviso si el ID tabla no produce Overflow
  tmp = strtol(nhebras, & end, 10);
  if ((LONG_MIN == tmp || LONG_MAX == tmp) && ERANGE == errno) {
    return -1;
  }

}

int Valida::consultas(const char * consultas) {

  //Comprueblo si el archivo de consulta es un texto plano.
  int istxt;
  std::string one = "file -i ";
  std::string two = " | grep 'text/plain' | wc -l";
  std::string nombrearchivo(consultas);

  std::string final = one + nombrearchivo + two;

  FILE * fp = popen(final.c_str(), "r");
  char buffer[256];
  while (fgets(buffer, sizeof(buffer), fp)) {
    istxt = atoi(buffer); // devuelve un 1 si es un text/plain
  }
  pclose(fp);

  if (istxt == 0) {
    return -8;
  }

  Reader reader;

  std::string consultasLeidos = reader.leo(consultas);
  //Tengo todo el texto en un string

  std::vector < std::string > Listaconsultas;
  reader.tokenize(consultasLeidos, Listaconsultas, "\n");
  //Tengo un vector gigante donde cada espacio es un caracter.

  std::vector < std::string > ::const_iterator it = Listaconsultas.begin();
  std::vector < std::string > ::const_iterator end = Listaconsultas.end();

  //Paso cada linea a una palabra, delimitador es " "

  //Voy separando linea por linea, dentro de cada linea separo por palabra.
  //trabajo con lista consulta que es un string
  for (; it != end; ++it) {
    std::string linea = * it;
    std::vector < std::string > Listaconsultas;
    reader.tokenize(linea, Listaconsultas, " ");

    //Si la primera palabra es insert.
    if (Listaconsultas[0].compare("insert") == 0) {

      //Si es un insert minino es 4 palabras
      if (Listaconsultas.size() < 4) {
        return -1;
      }

      if (Listaconsultas[2].size() < 3) {
        return -1;
      }

      // Le borro el primer caracter
      Listaconsultas[2].erase(0, 1);
      //Le borro el ultimo caracter.
      Listaconsultas[2].erase(Listaconsultas[2].size() - 1, Listaconsultas[2].size());
      //Reviso si el ID tabla es posible

      if (idtablaposible(Listaconsultas[2].c_str()) == -1) {
        return -1;
      }

      //Reviso si la palabra 4 empieza con un "
      try {
        int ascii = (int) Listaconsultas[3].at(0);
        if (ascii != 34) {
          return -1;
        }
      } catch (out_of_range & oor) {

        return -1;
      }

      //Reviso si todas palabras terminan con un ")
      try {
        int csize = Listaconsultas.size();
        int ascii = (int) Listaconsultas[csize - 1].at(Listaconsultas[csize - 1].size() - 1);
        if (ascii != 41) {
          return -1;
        }
      } catch (out_of_range & oor) {
        return -1;
      }

      //Reviso si el penultimo caracter es un "
      try {

        int csize = Listaconsultas.size();
        int ascii = (int) Listaconsultas[csize - 1].at(Listaconsultas[csize - 1].size() - 2);
        if (ascii != 34) {
          return -1;
        }
      } catch (out_of_range & oor) {
        return -1;
      }

    } //Valide todas las que empiezan con insert (en teoria <:)

    //Si la primera palabra es delete
    if (Listaconsultas[0].compare("delete") == 0) {
      //Si es un delete solo 4 palabras
      if (Listaconsultas.size() < 3 || Listaconsultas.size() > 3) {
        return -1;
      }

      if (Listaconsultas[2].size() < 3) {
        return -1;
      }

      // Le borro el primer caracter
      Listaconsultas[2].erase(0, 1);
      //Le borro el ultimo caracter.
      Listaconsultas[2].erase(Listaconsultas[2].size() - 1, Listaconsultas[2].size());

      //Reviso si el ID tabla es posible
      if (idtablaposible(Listaconsultas[2].c_str()) == -1) {
        return -1;
      }

      //Aca tengo el idtabla donde debo eliminar
      int idtabla = atoi(Listaconsultas[2].c_str());

    } //Valide todas las que empiezan con delete.

    //Si la primera palabra es insert.
    if (Listaconsultas[0].compare("update") == 0) {
      //Si es un update minino es 4 palabras
      if (Listaconsultas.size() < 4) {
        return -1;
      }


      if (Listaconsultas[2].size() < 3) {
        return -1;
      }

      // Le borro el primer caracter
      Listaconsultas[2].erase(0, 1);
      //Le borro el ultimo caracter.
      Listaconsultas[2].erase(Listaconsultas[2].size() - 1, Listaconsultas[2].size());
      //Reviso si el ID tabla es posible

      if (idtablaposible(Listaconsultas[2].c_str()) == -1) {
        return -1;
      }

      //Aca tengo el idtabla donde debo actulizar.
      int idtabla = atoi(Listaconsultas[2].c_str());

      //Reviso si la palabra 4 empieza con un "

      try {
        int ascii = (int) Listaconsultas[3].at(0);
        if (ascii != 34) {
          return -1;
        }
      } catch (out_of_range & oor) {
        return -1;
      }

      //Reviso si todas palabras terminan con un ")
      try {
        int csize = Listaconsultas.size();
        int ascii = (int) Listaconsultas[csize - 1].at(Listaconsultas[csize - 1].size() - 1);
        if (ascii != 41) {
          return -1;
        }
      } catch (out_of_range & oor) {
        return -1;
      }

      //Reviso si el penultimo caracter es un "
      try {
        int csize = Listaconsultas.size();
        int ascii = (int) Listaconsultas[csize - 1].at(Listaconsultas[csize - 1].size() - 2);
        if (ascii != 34) {
          return -1;
        }

      } catch (out_of_range & oor) {
        return -1;
      }

    } //Valide todas las que empiezan con update

    if (Listaconsultas[0].compare("select") == 0) {
      //Si es un select minino es 3 palabras
      if (Listaconsultas.size() < 3) {
        return -1;
      }

      int flag = 0;

      //Creo y guardo un string temporal
      std::string tmp;
      tmp.append(Listaconsultas[2]);

      if (tmp.size() < 4) {
        return -1;
      }

      //me quedo con los tres primeros caracteres.
      tmp.erase(3, tmp.size() - 1);

      if (tmp.compare("att") == 0) {
        flag = 1;
        //Si es att debo borrar los tres primeros caracteres "att="
        Listaconsultas[2].erase(0, 4);
        //Ahora debo verificar que comiencen y termine con ""
        //Ejemplo "El perro bailarin"
        //Empieza con "?
        try {
          int ascii = (int) Listaconsultas[2].at(0);
          if (ascii != 34) {
            return -1;
          }

        } catch (out_of_range & oor) {
          return -1;
        }

        //Termina con "?
        try {
          int ascii = (int) Listaconsultas[2].at(0);
          ascii = Listaconsultas[Listaconsultas.size() - 1].at(Listaconsultas[Listaconsultas.size() - 1].size() - 1);
          if (ascii != 34) {
            return -1;
          }
        } catch (out_of_range & oor) {
          return -1;
        }

      } //Termino de validar los att  

      tmp.erase(2, tmp.size() - 1);

      if (tmp.compare("id") == 0) {
        flag = 1;
        //Si es id tomo la palabra 3 y le quito los tres primeros caracteres "id="
        Listaconsultas[2].erase(0, 3);

        //Aca tengo el id que deseo actulizar
        //Listaconsultas[2];
      }

      if (flag == 0) {
        return -1;
      }

    } //Valide todas las que empiezan con select

  }

  return 0;
}

//Valido las tablas que estan dentro de la carpeta bd
int Valida::tablas() {
  DIR * dp;
  string dir;
  dir = "bd";
  string tmp;

  vector < string > files;
  struct dirent * dirp;
  if ((dp = opendir(dir.c_str())) == NULL) {
    //cout << "Error(" << errno << ") opening " << dir << endl;
    return -1;
  }

  while ((dirp = readdir(dp)) != NULL) {

    tmp.clear();
    tmp.append(dirp - > d_name);
    if (tmp.compare(".") != 0 && tmp.compare("..") != 0) {
      files.push_back(string(dirp - > d_name));
    }
  }

  closedir(dp);
  //Aca esta la lista de tablas.

  //Si no hay archivos en DB salgo.
  if (files.size() == 0) {
    return -1;
  }

  //reviso si tienen la extension .bd
  for (int i = 0; i < files.size(); i++) {
    std::string filetmp = files[i].substr(files[i].size() - 3, files[i].size() - 1);
    if (filetmp.compare(".bd") != 0) {
      return -1;
    }
  }
  return 1;
}