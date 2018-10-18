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
    nEnter(mon);
    nCondition condition = nMakeCondition(mon);
    if (!EmptyFifoQueue(filaEsperaHaciaChacao)){
        PutObj(filaEsperaHaciaChacao, &condition);
        nWaitCondition(condition);
    }
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
            PushObj(filaEsperaHaciaChacao, &condition);
            nWaitCondition(condition);
            continue;

        } else if(barcosHaciaChacao == numeroDeBarcos){
            // Caso: Todos los transbordadores se dirigen a Chacao
            PushObj(filaEsperaHaciaChacao, &condition);
            nWaitCondition(condition);
            continue;
        } else if(!EmptyFifoQueue(filaEsperaHaciaPargua)){
            // Caso: Existe vehiculo esperando en Chacao
            nCondition *con = (nCondition *)GetObj(filaEsperaHaciaPargua);
            PushObj(filaEsperaHaciaChacao, &condition);
            nSignalCondition(*con);
            nWaitCondition(condition);
            continue;

        } else{
            // Caso: No existe vehiculo esperando en Chacao
            for (int i = 0; i < numeroDeBarcos ; ++i) {
                if (barcosEnChacao[i] == 1){
                    transbordadorDisponible = i;
                    tengoTransbordador = TRUE;
                    break;
                }
            }
            if(tengoTransbordador){
                barcosEnChacao[transbordadorDisponible] = 0;
                barcosHaciaPargua++;
                nExit(mon);
                haciaPargua(transbordadorDisponible, -1);
                nEnter(mon);
                barcosHaciaPargua--;
                barcosEnPargua[transbordadorDisponible] = 1;
                break;
            }
        }
    }
    barcosEnPargua[transbordadorDisponible] = 0;
    barcosHaciaChacao++;
    nExit(mon);
    haciaChacao(transbordadorDisponible, v);
    nEnter(mon);
    barcosHaciaChacao--;
    barcosEnChacao[transbordadorDisponible] = 1;
    if (!EmptyFifoQueue(filaEsperaHaciaPargua)){
        nCondition *primer = (nCondition *)GetObj(filaEsperaHaciaPargua);
        nSignalCondition(*primer);
    } else if (!EmptyFifoQueue(filaEsperaHaciaChacao)){
        nCondition *primer = (nCondition *)GetObj(filaEsperaHaciaChacao);
        nSignalCondition(*primer);
    }
    nExit(mon);
}

void transbordoAPargua(int v){
    nEnter(mon);
    nCondition condition = nMakeCondition(mon);
    if (!EmptyFifoQueue(filaEsperaHaciaPargua)){
        PutObj(filaEsperaHaciaPargua, &condition);
        nWaitCondition(condition);
    }
    int tengoTransbordador = FALSE;
    int transbordadorDisponible;
    while (!tengoTransbordador){
        for (int i = 0; i < numeroDeBarcos ; ++i) {
            if (barcosEnChacao[i] == 1){
                transbordadorDisponible = i;
                tengoTransbordador = TRUE;
                break;
            }
        }
        if(tengoTransbordador){
            break;
        }
        if (barcosHaciaChacao > 0){
            // Caso: Viene algun transbordador a Chacao
            PushObj(filaEsperaHaciaPargua, &condition);
            nWaitCondition(condition);
            continue;

        } else if(barcosHaciaPargua == numeroDeBarcos){
            // Caso: Todos los transbordadores se dirigen a Pargua
            PushObj(filaEsperaHaciaPargua, &condition);
            nWaitCondition(condition);
            continue;
        } else if(!EmptyFifoQueue(filaEsperaHaciaChacao)){
            // Caso: Existe vehiculo esperando en Pargua
            nCondition *con = (nCondition *)GetObj(filaEsperaHaciaChacao);
            PushObj(filaEsperaHaciaPargua, &condition);
            nSignalCondition(*con);
            nWaitCondition(condition);
            continue;
        } else{
            // Caso: No existe vehiculo esperando en Pargua
            for (int i = 0; i < numeroDeBarcos ; ++i) {
                if (barcosEnPargua[i] == 1){
                    transbordadorDisponible = i;
                    tengoTransbordador = TRUE;
                    break;
                }
            }
            if(tengoTransbordador) {
                barcosEnPargua[transbordadorDisponible] = 0;
                barcosHaciaChacao++;
                nExit(mon);
                haciaChacao(transbordadorDisponible, -1);
                nEnter(mon);
                barcosHaciaChacao--;
                barcosEnPargua[transbordadorDisponible] = 1;
                break;
            }
        }
    }
    barcosEnChacao[transbordadorDisponible] = 0;
    barcosHaciaPargua++;
    nExit(mon);
    haciaPargua(transbordadorDisponible, v);
    nEnter(mon);
    barcosHaciaPargua--;
    barcosEnPargua[transbordadorDisponible] = 1;
    if (!EmptyFifoQueue(filaEsperaHaciaChacao)){
        nCondition *primer = (nCondition *)GetObj(filaEsperaHaciaChacao);
        nSignalCondition(*primer);
    } else if (!EmptyFifoQueue(filaEsperaHaciaPargua)){
        nCondition *primer = (nCondition *)GetObj(filaEsperaHaciaPargua);
        nSignalCondition(*primer);
    }
    nExit(mon);
}