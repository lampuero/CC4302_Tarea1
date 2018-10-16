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
	PutObj(esperaHaciaChacao, v);
    int transdisponible = -1;
    while (transdisponible == -1){
		int vehiculo = GetObj(esperaHaciaChacao);
		if (vehiculo == v){				
			for (int i = 0; i < numTRansbordadores ; ++i) {
				if (enPargua[i] == 1){
					transdisponible = i;
					break;
				}
			}
			if (transdisponible == -1){
                if (haciaPargua == 0){
                    if (haciaChacao = numTRansbordadores){
						nWaitCondition(transEnChacao);
						continue;
					} else{
						int l = LengthFifoQueue(transbordoAPargua);
						if (l > 0){
							nSignalCondition(transEnChacao);
							nWaitCondition(transEnPargua);
						} else{
							for (int i = 0; i < numTRansbordadores ; ++i) {
								if (enChacao[i] == 1){
									transdisponible = i;
									break;
								}
							}
							enChacao[transdisponible] = 0;
							haciaPargua++;
							nExit(mon);
							haciaPargua(transdisponible, -1);
							nEnter(mon);
							haciaPargua--;
							enPargua[transdisponible] = 1;
							transdis = transchacao;
						}
					}
                } else{
					PushObj(esperaHaciaChacao, vehiculo);
					nWaitCondition(transEnPargua);
				}
            }
		} else{			
			PushObj(esperaHaciaChacao, vehiculo);
			PutObj(esperaHaciaChacao, v);
			nSignalCondition(transEnPargua);
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