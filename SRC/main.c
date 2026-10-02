/*
 * Cargador Universal de Baterias - TP Integrador Informatica II
 *
 * Bruno Vega 
 * 2-11-R / 2026
 */

 
#include "funciones.h"

void app_main(void)
{
    static cargador_t cargador;
    estado_t (*const fsm[EST_CANT])(cargador_t *) = {
        [EST_INIT]          = f_init,
        [EST_MENU]          = f_menu,
        [EST_MODO_CARGA]    = f_modo,
        [EST_MODO_DESCARGA] = f_modo,
        [EST_CONFIG]        = f_config,
        [EST_CARGA]         = f_proceso,
        [EST_DESCARGA]      = f_proceso,
        [EST_IDLE]          = f_idle,
    };
    estado_t estado = inicio(&cargador);

    while (1) {
        leer_entradas(&cargador);
        estado = (*fsm[estado])(&cargador);
        mostrar(&cargador, estado);
    }
}


