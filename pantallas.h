#ifndef PANTALLAS_H
#define PANTALLAS_H

#include "funciones.h"
#include "u8g2.h"

/* Dibuja la pantalla del estado actual y la envia al OLED */
void pantalla_dibujar(u8g2_t *u8g2, const cargador_t *c, estado_t estado);

#endif
