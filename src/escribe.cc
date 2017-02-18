#include "escribe.h"

Escribe::Escribe() {}

Escribe::~Escribe() {}

void Escribe::escribir(std::vector < std::string > salida,
  const char * out) {
  std::ofstream f(out);
  for (int i = 0; i < salida.size(); i++) {

    f << salida[i] << '\n';

  }
}