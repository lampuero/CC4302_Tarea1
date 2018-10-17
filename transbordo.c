#include <nSystem.h>
#include "transbordo.h"
#include "fifoqueues.h"

typedef struct {
    nCondition condition;
} Espera;

int *barcosEnPargua;
int *barcosEnChacao;

int barcosHaciaPargua;
int barcosHaciaChacao;

int esperaParaChacao;
int esperaParaPargua;

int numeroDeBarcos;

nMonitor mon;

FifoQueue filaEsperaHaciaPargua;
FifoQueue filaEsperaHaciaChacao;

nCondition transbordadorEnOrilla;



void inicializar(int p){
    mon = nMakeMonitor();

    transbordadorEnOrilla = nMakeCondition(mon);

    filaEsperaHaciaChacao = MakeFifoQueue();
    filaEsperaHaciaPargua = MakeFifoQueue();

    barcosEnChacao = nMalloc(sizeof(int) * p);
    barcosEnPargua = nMalloc(sizeof(int) * p);

    barcosHaciaChacao = 0;
    barcosHaciaPargua = 0;

    esperaParaChacao = FALSE;
    esperaParaPargua = FALSE;

    numeroDeBarcos = p;

    for (int i=0; i<numeroDeBarcos; i++) {
        barcosEnPargua[i] = 1;
        barcosEnChacao[i] = 0;
    }
}
void finalizar(){
	DestroyFifoQueue(filaEsperaHaciaChacao);
	DestroyFifoQueue(filaEsperaHaciaPargua);

	nDestroyCondition(transbordadorEnOrilla);

	nDestroyMonitor(mon);

	nFree(barcosEnChacao);
	nFree(barcosEnPargua);
}
void transbordoAChacao(int v){
    Espera *espera;
    nEnter(mon);
    espera->condition = nMakeCondition(mon);
    if (!EmptyFifoQueue(filaEsperaHaciaChacao) || esperaParaChacao){
        PutObj(filaEsperaHaciaChacao, espera);
        //Espera *primer = (espera *)GetObj(esperaHaciaChacao);
        //nSignalCondition(primer->condition);
        nWaitCondition(espera->condition);
    }
    esperaParaChacao = TRUE;
    int tengoTransbordador = FALSE;
    int transbordadorDisponible;
    while (!tengoTransbordador){
        for (int i = 0; i < numeroDeBarcos ; ++i) {
            if (barcosEnPargua[i] == 1){
                transbordadorDisponible = i;
                tengoTransbordador = TRUE;
                break;
            }
        }
        if(tengoTransbordador){
            break;
        }
        if (transbordadoresHaciaPargua > 0){
            // Caso: Viene algun transbordador a Pargua
            nWaitCondition(transbordadorEnOrilla);
            continue;

        } else if(transHaciaChacao == numeroTransbordadores){
            // Caso: Todos los transbordadores se dirigen a Chacao
            nWaitCondition(transbordadorEnOrilla);
            continue;
        } else if(EmptyFifoQueue(esperaHaciaPargua) && !esperaParaPargua){
            // Caso: No existe vehiculo esperando en Chacao
            for (int i = 0; i < numTRansbordadores ; ++i) {
                if (tranbordadoresEnChacao[i] == 1){
                    transbordadorDisponible = i;
                    tengoTransbordador = TRUE;
                    break;
                }
            }
            if(tengoTransbordador){
                barcosEnChacao[transdisponible] = 0;
                barcosHaciaPargua++;
                nExit(mon);
                haciaPargua(transdisponible, -1);
                nEnter(mon);
                barcosHaciaPargua--;
                barcosEnPargua[transdisponible] = 1;
                break;
            }
        } else{
            // Caso: Existe vehiculo esperando en Chacao
            nSignalCondition(transbordadorEnOrilla);
            nWaitCondition(transbordadorEnOrilla);
            continue;
        }
    }
    barcosEnPargua[transbordadorDisponible] = 0;
    barcosHaciaChacao++;
    nExit(mon);
    haciaChacao(transdisponible, v);
    nEnter(mon);
    barcosHaciaChacao--;
    barcosEnChacao[transbordadorDisponible] = 1;
    nSignalCondition(transbordadorEnOrilla);
    if (!EmptyFifoQueue(filaEsperaHaciaChacao)){
        Espera *primer = (espera *)GetObj(filaEsperaHaciaChacao);
        nSignalCondition(primer->condition);
    }
    nExit(mon);
}
void transbordoAPargua(int v){
    //int primerVehiculo;
    nEnter(mon);
    int *p = ++pointerPargua;
    PutObj(esperaHaciaPargua, p);
    int transdisponible = -1;
    while (transdisponible == -1){
        if (EmptyFifoQueue(esperaHaciaPargua)){
            nExit(mon);
            return;
        }
        int *vehiculo = (int *)GetObj(esperaHaciaPargua);
        //nPrintf("Sacado Pargua %d\n", vehiculo);
        //primerVehiculo = *vehiculo;
        if (vehiculo == 0){
            continue;
        }
        if (p == vehiculo){
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
                            transbordadoresHaciaChacao++;
							nExit(mon);
							haciaChacao(transdisponible, -1);
							nEnter(mon);
                            transbordadoresHaciaChacao--;
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
    transbordadoresHaciaPargua++;
    nExit(mon);
    haciaPargua(transdisponible, v);
    nEnter(mon);
    //nPrintf("LLego a Pargua %i en %i\n", v, transdisponible);
    transbordadoresHaciaPargua--;
    enPargua[transdisponible] = 1;
    nSignalCondition(transEnPargua);
    nExit(mon);
}