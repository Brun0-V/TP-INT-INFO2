#ifndef FUNCIONES_H
#define FUNCIONES_H

/*
 * Cargador Universal de Baterias - FSM (ver DiagBloques.svg)
 *
 *   Init -> Menu -> Modo de Carga / Modo de Descarga -> Config / Carga / Descarga
 *   Carga / Descarga <-> IDLE (sobretemperatura)
 *
 * Cada estado es una funcion no bloqueante que devuelve el proximo estado.
 */

#include <stdint.h>

#define CARGADOR_VERSION "0.1"

#define CICLO_MS     40      /* periodo del bucle principal */
#define INIT_MS      1500    /* duracion de la pantalla de inicio */
#define MAX_TEMP     45.0f   /* bat.temp > MAX_TEMP -> IDLE */
#define TEMP_OK      35.0f   /* bat.temp < TEMP_OK  -> vuelve a Carga/Descarga */
#define MV_MARGEN_CV 10      /* a menos de 10 mV de la tension de carga se considera CV */

typedef enum {
    EST_INIT, EST_MENU, EST_MODO_CARGA, EST_MODO_DESCARGA,
    EST_CONFIG, EST_CARGA, EST_DESCARGA, EST_IDLE, EST_CANT
} estado_t;

typedef enum { EV_NINGUNO, EV_ARRIBA, EV_ABAJO, EV_PULSADO } evento_t;

/* Los valores sirven de indice en mv[] y ma[] */
typedef enum { SAL_DESCARGA, SAL_CARGA, SAL_APAGADO} salida_t;

typedef struct {
    const char *nombre;
    int mv[2];   /* [0] corte de descarga, [1] fin de carga */
    int ma[2];   /* [0] descarga, [1] carga */
} quimica_t;

typedef struct {
    int quimica;
    int mv[2];
    int ma[2];
} bat_config_t;

typedef struct {
    float v, i, temp;   /* medidos: V, A (magnitud), C */
    float mah, seg;     /* acumulados del proceso */
} bat_medicion_t;

typedef struct {
    uint32_t ahora_ms, dt_ms;
    uint32_t desde_ms;   /* entrada al estado actual */
    evento_t evento;     /* evento de este ciclo */
    int carga;           /* 1 = carga, 0 = descarga (elegido en el menu) */
    int sel;             /* opcion seleccionada en la pantalla actual */
    int editando;        /* Config: editando un valor. Carga/Descarga: dialogo "Detener?" */
    int fin;             /* Carga/Descarga terminada: se muestra el resumen */
    bat_config_t cfg;
    bat_medicion_t bat;
} cargador_t;

extern const quimica_t quimicas[];

/* Bucle principal */
estado_t inicio(cargador_t *c);
void leer_entradas(cargador_t *c);
void mostrar(const cargador_t *c, estado_t estado);

/* Estados */
estado_t f_init(cargador_t *c);
estado_t f_menu(cargador_t *c);
estado_t f_modo(cargador_t *c);      /* Modo de Carga / Modo de Descarga */
estado_t f_config(cargador_t *c);
estado_t f_proceso(cargador_t *c);   /* Carga / Descarga */
estado_t f_idle(cargador_t *c);

int limitar(int v, int min, int max);

#endif
