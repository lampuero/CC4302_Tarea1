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
        if (barcosHaciaPargua > 0){
            // Caso: Viene algun transbordador a Pargua
            nWaitCondition(transbordadorEnOrilla);
            continue;

        } else if(barcosHaciaChacao == numeroTransbordadores){
            // Caso: Todos los transbordadores se dirigen a Chacao
            nWaitCondition(transbordadorEnOrilla);
            continue;
        } else if(EmptyFifoQueue(filaEsperaHaciaPargua) && !esperaParaPargua){
            // Caso: No existe vehiculo esperando en Chacao
            for (int i = 0; i < numeroDeBarcos ; ++i) {
                if (barcosEnChacao[i] == 1){
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
    esperaParaChacao = FALSE;
    if (!EmptyFifoQueue(filaEsperaHaciaChacao)){
        Espera *primer = (espera *)GetObj(filaEsperaHaciaChacao);
        nSignalCondition(primer->condition);
    }
    nExit(mon);
    haciaChacao(transdisponible, v);
    nEnter(mon);
    barcosHaciaChacao--;
    barcosEnChacao[transbordadorDisponible] = 1;
    nSignalCondition(transbordadorEnOrilla);
    nExit(mon);
}

