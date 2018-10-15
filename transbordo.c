#include <nSystem.h>
#include "transbordo.h"





int[] enPargua;
int[] enChacao;
int enViaje;
int numTRansbordadores;
nMonitor mon;

void inicializar(int p){
    mon = nMakeMonitor();
    enPargua = new int[p];
    enChacao = new int[p];
    enViaje = 0;
    numTRansbordadores = p;
    for (i=0; i<numTRansbordadores; i++) {
        enPargua[i] = 1;
        enChacao[i] = 0;
  }
}
void finalizar(){}
void transbordoAChacao(int v){}
void transbordoAPargua(int v){}