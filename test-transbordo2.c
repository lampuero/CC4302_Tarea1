#include <nSystem.h>
#include "transbordo.h"

int debugging= TRUE;
int verificar, achacao, apargua;

typedef struct {
    int i; /* transbordador */
    int v; /* vehiculo */
    nTask t;
    int haciaChacao;
} Viaje;

/* Guarda el identificador de la tarea nMain que sirve para controlar
   el avance del resto de las tareas */
nTask ctrl;

/* Procedimientos para los tests */

Viaje *esperarTransbordo();
void continuarTransbordo(Viaje *);


int norteno(int v);
int isleno(int v);


int nMain( int argc, char **argv ) {
    ctrl= nCurrentTask();
    inicializar(4);
    verificar= TRUE;
    // Todos los transbordadores estan en Pargua
    nPrintf("Test\n");
    int i0= testUnTransbordo(norteno, 0); // entrega el transbordador usado
    int i1= testUnTransbordo(norteno, 1);
    if (i0==i1){
        nFatalError("nMain", "Los transbordadores debieron ser distintos\n");
    }
    int i2= testUnTransbordo(isleno, 2);
    if (i2!=i0 && i2!=i1){
        nFatalError("nMain", "Aparecio un transbordador fantasma en la otra orilla\n");
    }
    int i3 = testUnTransbordo(isleno, 3);
    if (i2==i3){
        nFatalError("nMain", "Los transbordadores debieron ser distintos\n");
    }
    if (i3!=i0 && i3!=i1){
        nFatalError("nMain", "Aparecio un transbordador fantasma en la otra orilla\n");
    }
    finalizar();
    nPrintf("\n\nBien! Su tarea funciono correctamente con estos tests.  Si\n");
    nPrintf("al finalizar este programa nSystem no indica ninguna operacion\n");
    nPrintf("pendiente, Ud. ha completado exitosamente todos los tests\n");
    nPrintf("de la tarea.  Ud. puede entregar su tarea.\n\n");
    return 0;
}

int testUnTransbordo(int (*tipo)(), int v) {
  // Precondicion: hay transbordadores disponibles en la misma orilla
  // del vehiculo v.
  // Embarca, transborda y desembarca vehiculo v.  Retorna el transbordador
  // usado que queda estacionado en la otra orilla.
  nTask vehiculoTask= nEmitTask(tipo, v); /* vehiculo v */
  Viaje *viaje= esperarTransbordo();
  int i= viaje->i; /* el transbordador usado */
  if (viaje->v!=v)
    nFatalError("testUnTransbordo", "Se transborda el vehiculo incorrecto\n");
  if ( !(0<=i && i<4) )
    nFatalError("testUnTransbordo", "El trabordador debe estar entre 0 y 3\n");
  continuarTransbordo(viaje);
  nWaitTask(vehiculoTask);
  return i;
}

int norteno(int v) {
  transbordoAChacao(v);
  return 0;
}


int isleno(int v) {
  transbordoAPargua(v);
  return 0;
}

// haciaChacao: Ud. invoca esta funcion en transbordo.c cuando se invoca
// transbordoAChacao.
void haciaChacao(int i, int v) {
  if (!verificar)
    achacao++;
  else {
    Viaje viaje;
    viaje.i= i;
    viaje.v= v;
    viaje.haciaChacao= TRUE;
    // Notifica al nMain que se invoco haciaChacao.  El vehiculo v va
    // hacia Chacao.  Llegara cuando nMain invoque continuarTransbordo(&viaje).
    nSend(ctrl, &viaje);
    // Ahora si se desembarco v.  La funcion retorna con lo que
    // transbordoAChacao puede retornar.
  }
}


// haciaPargua: Ud. invoca esta funcion en transbordo.c cuando se invoca
// transbordoAPargua.
void haciaPargua(int i, int v) {
  if (!verificar)
    apargua++;  // Solo valido para el test de esfuerzo
  else {
    Viaje viaje;
    viaje.i= i;
    viaje.v= v;
    viaje.haciaChacao= FALSE;
    // Notifica al nMain que se invoco haciaPargua.  El vehiculo v va
    // hacia Pargua.  Llegara cuando nMain invoque continuarTransbordo(&viaje).
    nSend(ctrl, &viaje);
    // Termino el transbordo.  La funcion retorna con lo que
    // transbordoAPargua puede retornar.
  }
}

Viaje *esperarTransbordo() {
  nTask t;
  // Espera la notificacion del inicio de cualquier transbordo llamando
  // haciaPargua o haciaChacao.
  // Retorna en viaje el transbordador usado y el vehiculo que lleva.
  Viaje *viaje= nReceive(&t, -1);
  viaje->t= t;
  return viaje;
}

void continuarTransbordo(Viaje *viaje) {
  // Invocada por nMain.
  // Hace que termine el transbordo de viaje->i con el vehiculo viaje->v.
  // Ese transbordador llego a la orilla y por lo tanto la
  // llamada a haciaPargua o haciaChacao retorna.  El transbordador queda
  // libre para otro viaje.
  nReply(viaje->t, 0);
}
