#include <stdio.h>
#include "grafo.h"

//------------------------------------------------------------------------------

int main(void) {

  grafo g = le_grafo();

  escreve_grafo(g);

  grafo f = decompoe(g);

  for (grafo sg = agfstsubg(f); sg; sg = agnxtsubg(sg)) {
    escreve_grafo(sg);
  }

  destroi_grafo(g);

  return 0;
}
