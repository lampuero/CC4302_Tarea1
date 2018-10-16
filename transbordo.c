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
void finalizar(){
	DestroyFifoQueue(esperaHaciaChacao);
	DestroyFifoQueue(esperaHaciaPargua);
	nDestroyCondition(transEnChacao);
	nDestroyCondition(transEnPargua);
	nDestroyMonitor(mon)
}
void transbordoAChacao(int v){
    nEnter(mon);
	PutObj(esperaHaciaChacao, v);
    int transdisponible = -1;
    while (transdisponible == -1){
		int vehiculo = GetObj(esperaHaciaChacao);
		if (vehiculo == v){
			// caso primero en la fila
			for (int i = 0; i < numTRansbordadores ; ++i) {
				if (enPargua[i] == 1){
					transdisponible = i;
					break;
				}
			}
			if (transdisponible == -1){
				// caso no hay transbordador disponible en Pargua
                if (haciaPargua == 0){
					// Caso: ningun transbordador se dirige a Pargua
                    if (haciaChacao = numTRansbordadores){
						// Caso: todos los transbordadores se dirigen a Chacao
						nWaitCondition(transEnChacao);
						continue;
					} else{
						// Caso: existe transbordador en Chacao
						int l = LengthFifoQueue(esperaHaciaPargua);
						if (l > 0){
							// Caso: existe vehiculo esperando en Chacao
							nSignalCondition(transEnChacao);
							nWaitCondition(transEnPargua);
						} else{
							// Caso: no existe vehiculo esperando en Chacao
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
					// Caso: algun transbordador se dirige a Pargua
					PushObj(esperaHaciaChacao, vehiculo);
					nWaitCondition(transEnPargua);
				}
            }
		} else{
			// Caso: no es primero en fila
			PushObj(esperaHaciaChacao, vehiculo);
			nSignalCondition(transEnPargua);
			nWaitCondition(transEnPargua);
		}
    }
    enPargua[transdisponible] = 0;
    haciaChacao++;
    nExit(mon);
    haciaChacao(transdisponible, v);
    nEnter(mon);
    haciaChacao--;
    enChacao[transdisponible] = 1;
    nSignalCondition(transEnChacao);
    nExit(mon);
}
void transbordoAPargua(int v){
	nEnter(mon);
	PutObj(esperaHaciaPargua, v);
    int transdisponible = -1;
    while (transdisponible == -1){
		int vehiculo = GetObj(esperaHaciaPargua);
		if (vehiculo == v){				
			for (int i = 0; i < numTRansbordadores ; ++i) {
				if (enChacao[i] == 1){
					transdisponible = i;
					break;
				}
			}
			if (transdisponible == -1){
                if (haciaChacao == 0){
                    if (haciaPargua = numTRansbordadores){
						nWaitCondition(transEnPargua);
						continue;
					} else{
						int l = LengthFifoQueue(esperaHaciaChacao);
						if (l > 0){
							nSignalCondition(transEnChacao);
							nWaitCondition(transEnPargua);
						} else{
							for (int i = 0; i < numTRansbordadores ; ++i) {
								if (enPargua[i] == 1){
									transdisponible = i;
									break;
								}
							}
							enPargua[transdisponible] = 0;
							haciaChacao++;
							nExit(mon);
							haciaChacao(transdisponible, -1);
							nEnter(mon);
							haciaChacao--;
							enChacao[transdisponible] = 1;
						}
					}
                } else{
					PushObj(esperaHaciaPargua, vehiculo);
					nWaitCondition(transEnChacao);
				}
            }
		} else{			
			PushObj(esperaHaciaPargua, vehiculo);
			PutObj(esperaHaciaPargua, v);
			nSignalCondition(transEnChacao);
			nWaitCondition(transEnChacao);
		}
    }
    enChacao[transdisponible] = 0;
    haciaPargua++;
    nExit(mon);
    haciaPargua(transdisponible, v);
    nEnter(mon);
    haciaPargua--;
    enPargua[transdisponible] = 1;
    nSignalCondition(transEnPargua);
    nExit(mon);
}