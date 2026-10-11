#include "funciones.h"
#include "pantallas.h"
#include <stdio.h>


#define QUIM_CANT 8
#define MV_MIN 500
#define MV_MAX 16000
#define MV_PASO 50
#define MA_MIN 100
#define MA_MAX 3000
#define MA_PASO 100
#define BEEP_MS 500

const quimica_t quimicas[QUIM_CANT] = {
//    Nombre     Vmin Vmax     Idesc Icarga
    {"Li-Ion",  {3000,4200},   {500,1000}},
    {"LiPo",    {3300,4200},   {500,1000}},
    {"LiFePO4", {2500,3650},   {500,1000}},
    {"SLA",     {10500,14400}, {500,1000}},
    {"AGM",     {10500,14700}, {500,1000}},
    {"Gel",     {10500,14100}, {500,1000}},
    {"NiMH",    {1000,1450},   {300,500}},
    {"NiCd",    {1000,1450},   {300,500}},
};

int limitar(int valor, int min, int max){
    return valor < min ? min : valor > max ? max : valor;  
    //Si el valor es menor a min devuelve min, si es mayor a max devuelve max, sino devuelve el valor
} 

static void mover(cargador_t *c, int cant){
    if (c->evento == EV_ABAJO) c->sel = (c->sel + 1) % cant;
    if (c->evento == EV_ARRIBA) c->sel = (c->sel + cant - 1) % cant;
}


void leer_entradas(cargador_t *c){
    uint32_t antes = c->ahora_ms;

    c->evento = hw_leer(&c->bat);
    c->ahora_ms = hw_millis();
    c->dt_ms = c->ahora_ms - antes;
}


void mostrar(const cargador_t *c, estado_t estado){
    static const char *nombres[EST_CANT] = {
        "Init", "Menu", "Modo de Carga", "Modo de Descarga",
         "Config", "Carga", "Descarga", "Idle"
    };
    static estado_t estado_anterior = EST_CANT;
    static int marca_anterior;
    int marca = 0;
    char linea[24];

    if (estado == EST_INIT){
        marca = (int)c->ahora_ms;
    } else if (estado == EST_CARGA || estado == EST_DESCARGA){
        marca = c->fin ? -1 : (int)c->bat.seg;
    } else if (estado == EST_IDLE){
        marca = (int)((c->ahora_ms-c->desde_ms)/500);
    }

    if (estado != estado_anterior){
        snprintf(linea, sizeof(linea), "> %s\r\n", nombres[estado]);
        hw_log(linea);
    } else if (c->evento == EV_NINGUNO && marca == marca_anterior){
        return; // No hay cambios, no redibuja
    }
    estado_anterior = estado;
    marca_anterior = marca;
    pantalla_dibujar(hw_pantalla(), c, estado);
}

estado_t f_init(cargador_t *c){}

estado_t f_menu(cargador_t *c){}

estado_t f_modo(cargador_t *c){}

estado_t f_config(cargador_t *c){}

estado_t f_proceso(cargador_t *c){}

estado_t f_idle(cargador_t *c){}