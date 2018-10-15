#include <nSystem.h>
#include "transbordo.h"
#include "fifoqueues.h"




int[] enPargua;
int[] enChacao;
int haciaPargua;
int haciaChacao;
int numTRansbordadores;
nMonitor mon;
FifoQueue esperaHaciaPargua;
FifoQueue esperaHaciaChacao;
nCondition transEnChacao;
nCondition transEnPargua;

void inicializar(int p){
    mon = nMakeMonitor();
    transEnChacao = nMakeCondition(mon);
    transEnPargua = nMakeCondition(mon);
    esperaHaciaChacao = MakeFifoQueue();
    esperaHaciaPargua = MakeFifoQueue();
    enPargua = new int[p];
    enChacao = new int[p];
    haciaChacao = 0;
    haciaPargua = 0;
    numTRansbordadores = p;
    for (i=0; i<numTRansbordadores; i++) {
        enPargua[i] = 1;
        enChacao[i] = 0;
  }
}
void finalizar(){}
void transbordoAChacao(int v){
    nEnter(mon);
    int transdis = -1;
    while (transdis == -1){
        if (EmptyFifoQueue(esperaHaciaChacao)){
            for (int i = 0; i < numTRansbordadores ; ++i) {
                if (enPargua[i] == 1){
                    transdis = i;
                    break;
                }
            }
            if (transdis == -1){
                if (haciaPargua == 0){
                    // casos de trans en Chacao
                }
                nWaitCondition(transEnPargua);
            }
        } else{
            PutObj(esperaHaciaChacao, v);
            nNotifyAll(mon);
            nWaitCondition(transEnPargua);
        }

    }
    enPargua[transdis] = 0;
    haciaChacao++;
    nExit(mon);
    haciaChacao(transdis, v);
    nEnter(mon);
    haciaChacao--;
    enChacao[transdis] = 1;
    nSignalCondition(transEnChacao);
    nExit(mon);
}
void transbordoAPargua(int v){}