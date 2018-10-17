#include <nSystem.h>
#include "transbordo.h"
#include "fifoqueues.h"




int *enPargua;
int *enChacao;
int transHaciaPargua;
int transHaciaChacao;
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
    enPargua = nMalloc(sizeof(int) * p);
	enChacao = nMalloc(sizeof(int) * p);
	transHaciaChacao = 0;
	transHaciaPargua = 0;
    numTRansbordadores = p;
    for (int i=0; i<numTRansbordadores; i++) {
        enPargua[i] = 1;
        enChacao[i] = 0;
    }
}
void finalizar(){
	DestroyFifoQueue(esperaHaciaChacao);
	DestroyFifoQueue(esperaHaciaPargua);
	nDestroyCondition(transEnChacao);
	nDestroyCondition(transEnPargua);
	nDestroyMonitor(mon);
	nFree(enChacao);
	nFree(enPargua);
}
void transbordoAChacao(int v){
    int vehiculoATransportar = v;
    int primerVehiculo;
    nEnter(mon);
    PutObj(esperaHaciaChacao, &vehiculoATransportar);
    int transdisponible = -1;
    while (transdisponible == -1){
        int *vehiculo = (int *)GetObj(esperaHaciaChacao);
        primerVehiculo = *vehiculo;
        if (vehiculoATransportar == primerVehiculo){
			// Caso: primero en la fila
			for (int i = 0; i < numTRansbordadores ; ++i) {
				if (enPargua[i] == 1){
					transdisponible = i;
					break;
				}
			}
			if (transdisponible == -1){
				// Caso: no hay transbordador disponible en Pargua
                if (transHaciaPargua == 0){
					// Caso: ningun transbordador se dirige a Pargua
                    if (transHaciaChacao == numTRansbordadores){
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
							transHaciaPargua++;
							nExit(mon);
							haciaPargua(transdisponible, -1);
							nEnter(mon);
							transHaciaPargua--;
							enPargua[transdisponible] = 1;
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
	transHaciaChacao++;
    nExit(mon);
    haciaChacao(transdisponible, v);
    nEnter(mon);
    //nPrintf("LLego a Chacao %i en %i\n", v, transdisponible);
	transHaciaChacao--;
    enChacao[transdisponible] = 1;
    nSignalCondition(transEnChacao);
    nExit(mon);
}
void transbordoAPargua(int v){
    int vehiculoATransportar = v;
    int primerVehiculo;
    nEnter(mon);
    PutObj(esperaHaciaPargua, &vehiculoATransportar);
    int transdisponible = -1;
    while (transdisponible == -1){
        int *vehiculo = (int *)GetObj(esperaHaciaPargua);
        primerVehiculo = *vehiculo;
        if (vehiculoATransportar == primerVehiculo){
			// Caso: primero en la fila
			for (int i = 0; i < numTRansbordadores ; ++i) {
				if (enChacao[i] == 1){
					transdisponible = i;
					break;
				}
			}
			if (transdisponible == -1){
				// Caso: no hay transbordador disponible en Chacao
                if (transHaciaChacao == 0){
					// Caso: ningun transbordador se dirige a Chacao
                    if (transHaciaPargua == numTRansbordadores){
						// Caso: todos los transbordadores se dirigen a Pargua
						nWaitCondition(transEnPargua);
						continue;
					} else{
						// Caso: existe transbordador en Pargua
						int l = LengthFifoQueue(esperaHaciaChacao);
						if (l > 0){
							// Caso: existe vehiculo esperando en Pargua
							nSignalCondition(transEnPargua);
							nWaitCondition(transEnChacao);
						} else{
							// Caso: no existe vehiculo esperando en Pargua
							for (int i = 0; i < numTRansbordadores ; ++i) {
								if (enPargua[i] == 1){
									transdisponible = i;
									break;
								}
							}
							enPargua[transdisponible] = 0;
							transHaciaChacao++;
							nExit(mon);
							haciaChacao(transdisponible, -1);
							nEnter(mon);
							transHaciaChacao--;
							enChacao[transdisponible] = 1;
						}
					}
                } else{
					// Caso: algun transbordador se dirige a Chacao
					PushObj(esperaHaciaPargua, vehiculo);
					nWaitCondition(transEnChacao);
				}
            }
		} else{
			// Caso: no es primero en fila
			PushObj(esperaHaciaPargua, vehiculo);
			nSignalCondition(transEnChacao);
			nWaitCondition(transEnChacao);
		}
    }
    enChacao[transdisponible] = 0;
	transHaciaPargua++;
    nExit(mon);
    haciaPargua(transdisponible, v);
    nEnter(mon);
    //nPrintf("LLego a Pargua %i en %i\n", v, transdisponible);
	transHaciaPargua--;
    enPargua[transdisponible] = 1;
    nSignalCondition(transEnPargua);
    nExit(mon);
}