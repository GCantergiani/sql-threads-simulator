#include "reader.h"
#include <errno.h>

Reader::Reader() {}

Reader::~Reader() {}

int Reader::existe(const std::string & file) {

  //Tipo para tamano de lectura.
  std::ifstream::pos_type size;
  // valor de retorno
  std::string return_val;
  // descriptor de archivo en C++. 
  std::ifstream fileDescriptor;

  // Abrir archivo a puntado por file. file.c_str() transforma el string a const char.
  fileDescriptor.open(file.c_str());

  // Si es nulo, entonces no se pudo abrir el archivo
  if (fileDescriptor == NULL) // Falta eliminar el error por defecto
  {
    return (-1);
  }
  return 1;

}

std::string Reader::leo(const std::string & file) const {
  std::ifstream::pos_type size;
  std::string return_val;
  std::ifstream fileDescriptor;

  // Abrir archivo a puntado por file. file.c_str() transforma el string a const char.
  fileDescriptor.open(file.c_str());

  // Si se puede abrir
  if (fileDescriptor.is_open()) {

    // Buscar dentro del archivo el fin de archivo (std::ios::end)
    fileDescriptor.seekg(0, std::ios::end);
    // Asignar el valor a size
    size = fileDescriptor.tellg();
    // Volver al inicio.
    fileDescriptor.seekg(0, std::ios::beg);

    // Asignar memoria a memblock de tamano size
    char * memblock = new char[size];
    // Leer memoria y guardarla en memblock
    fileDescriptor.read(memblock, size);
    // Cerrar descriptor
    fileDescriptor.close();

    // Si pudo leer
    if (memblock && memblock != 0) {
      // Agregar a return_val
      return_val = std::string(memblock, size);

    }
  }
  return return_val;
}

//void Reader::tokenize(const string& str,vector<string>& tokens,const string& delimiters = ".")
void Reader::tokenize(const string & str, vector < string > & tokens,
  const string & delimiters) {
  string::size_type lastPos = str.find_first_not_of(delimiters, 0);

  string::size_type pos = str.find_first_of(delimiters, lastPos);

  while (string::npos != pos || string::npos != lastPos) {
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
  }
}