#include "pantallas.h"
#include <stdio.h>

// Pantallas del OLED 128x64

#define FUENTE u8g2_font_6x10_tf
#define MARCO(n) (c->sel != (n) ? 0 : c->editando ? 2 : 1)

static u8g2_t *u;

/* alinear: 0 = izquierda en x, 1 = centrado en x, 2 = derecha en x */
static void texto(int x, int y, const char *s, int alinear)
{
    u8g2_DrawUTF8(u, x - u8g2_GetUTF8Width(u, s) * alinear / 2, y, s);
}

static void titulo(const char *s, const char *der)
{
    u8g2_DrawBox(u, 0, 0, 128, 11);
    u8g2_SetDrawColor(u, 0);
    texto(2, 1, s, 0);
    if (der) {
        texto(126, 1, der, 2);
    }
    u8g2_SetDrawColor(u, 1);
}

/* Fila "etiqueta .... valor". marco: 0 nada, 1 seleccionada, 2 editandose */
static void fila(int y, const char *etiqueta, const char *valor, int marco)
{
    char txt[24];

    if (marco == 2) {   /* editandose: invertida y con flechas */
        u8g2_DrawRBox(u, 0, y - 1, 128, 12, 2);
        u8g2_SetDrawColor(u, 0);
        snprintf(txt, sizeof(txt), "<%s>", valor);
        valor = txt;
    } else if (marco == 1) {
        u8g2_DrawRFrame(u, 0, y - 1, 128, 12, 2);
    }
    texto(3, y, etiqueta, 0);
    texto(124, y, valor, 2);
    u8g2_SetDrawColor(u, 1);
}

static void boton(int x, int y, int w, const char *s, int sel)
{
    if (sel) {
        u8g2_DrawRBox(u, x, y, w, 14, 3);
        u8g2_SetDrawColor(u, 0);
    } else {
        u8g2_DrawRFrame(u, x, y, w, 14, 3);
    }
    texto(x + w / 2, y + 2, s, 1);
    u8g2_SetDrawColor(u, 1);
}

/* Numero grande con su unidad al lado */
static void numero(int x, int y, const char *num, const char *unidad)
{
    u8g2_SetFont(u, u8g2_font_logisoso16_tn);
    u8g2_DrawStr(u, x, y, num);
    x += u8g2_GetStrWidth(u, num) + 2;
    u8g2_SetFont(u, FUENTE);
    u8g2_DrawUTF8(u, x, y + 7, unidad);
}

/* Bateria, nivel 0..100 */
static void bateria(int x, int y, int w, int h, int nivel)
{
    u8g2_DrawFrame(u, x, y, w, h);
    u8g2_DrawBox(u, x + w, y + h / 4, 2, h - 2 * (h / 4));
    u8g2_DrawBox(u, x + 2, y + 2, (w - 4) * limitar(nivel, 0, 100) / 100, h - 4);
}

/* Termometro de 20 a 60 C con marca en MAX_TEMP */
static void termometro(int x, int y, float temp)
{
    int alto = limitar((int)((temp - 20.0f) * 26.0f / 40.0f), 0, 26);

    u8g2_DrawRFrame(u, x, y, 9, 34, 4);
    u8g2_DrawDisc(u, x + 4, y + 38, 7, U8G2_DRAW_ALL);
    u8g2_DrawBox(u, x + 2, y + 30 - alto, 5, alto + 4);
    u8g2_DrawHLine(u, x + 10, y + 30 - (int)((MAX_TEMP - 20.0f) * 26.0f / 40.0f), 3);
}


void pantalla_dibujar(u8g2_t *u8g2, const cargador_t *c, estado_t estado)
{
    const bat_medicion_t *b = &c->bat;
    const bat_config_t *cfg = &c->cfg;
    int k = c->carga;
    int s = (int)b->seg;
    int nivel;
    char v[20], i[20], t[20];

    u = u8g2;
    u8g2_ClearBuffer(u);
    u8g2_SetFontPosTop(u);
    u8g2_SetFontMode(u, 1);
    u8g2_SetDrawColor(u, 1);
    u8g2_SetFont(u, FUENTE);

    snprintf(v, sizeof(v), "%d.%02d V", cfg->mv[k] / 1000, cfg->mv[k] % 1000 / 10);
    snprintf(i, sizeof(i), "%d.%02d A", cfg->ma[k] / 1000, cfg->ma[k] % 1000 / 10);
    snprintf(t, sizeof(t), "%02d:%02d:%02d", s / 3600, s / 60 % 60, s % 60);

    switch (estado) {
    case EST_INIT:
        u8g2_SetFont(u, u8g2_font_helvB08_tf);
        texto(64, 16, "Cargador Universal", 1);
        u8g2_SetFont(u, FUENTE);
        texto(64, 30, "de Baterias", 1);
        u8g2_DrawFrame(u, 14, 50, 100, 6);
        u8g2_DrawBox(u, 16, 52, limitar((int)(96 * (c->ahora_ms - c->desde_ms) / INIT_MS), 0, 96), 2);
        break;

    case EST_MENU:
        titulo("MENU", "v" CARGADOR_VERSION);
        boton(4, 17, 120, "Carga", c->sel == 0);
        boton(4, 39, 120, "Descarga", c->sel == 1);
        /* bateria llena para carga, vacia para descarga (invertida si esta seleccionada) */
        u8g2_SetDrawColor(u, c->sel != 0);
        bateria(10, 21, 14, 7, 100);
        u8g2_SetDrawColor(u, c->sel != 1);
        bateria(10, 43, 14, 7, 25);
        u8g2_SetDrawColor(u, 1);
        break;

    case EST_MODO_CARGA:
    case EST_MODO_DESCARGA:
    case EST_CONFIG:
        if (estado == EST_CONFIG) {
            titulo(k ? "CONFIG CARGA" : "CONFIG DESCARGA", NULL);
            boton(44, 50, 40, "OK", c->sel == 3);
        } else {
            titulo(k ? "MODO CARGA" : "MODO DESCARGA", NULL);
            boton(0, 50, 41, "Inicio", c->sel == 0);
            boton(43, 50, 42, "Config", c->sel == 1);
            boton(87, 50, 41, "Salir", c->sel == 2);
        }
        fila(14, "Quimica", quimicas[cfg->quimica].nombre, estado == EST_CONFIG ? MARCO(0) : 0);
        fila(26, k ? "Tension" : "V corte", v, estado == EST_CONFIG ? MARCO(1) : 0);
        fila(38, "Corriente", i, estado == EST_CONFIG ? MARCO(2) : 0);
        break;

    case EST_CARGA:
    case EST_DESCARGA:
        if (c->fin) {
            titulo(k ? "CARGA COMPLETA" : "DESCARGA COMPLETA", NULL);
            snprintf(i, sizeof(i), "%d mAh", (int)b->mah);
            fila(14, k ? "Cargado" : "Capacidad", i, 0);
            fila(26, "Tiempo", t, 0);
            snprintf(v, sizeof(v), "%.2f V", b->v);
            fila(38, "V final", v, 0);
            boton(44, 50, 40, "OK", 1);
            break;
        }
        titulo(k ? "CARGANDO" : "DESCARGANDO",
               !k ? NULL : b->v * 1000.0f >= cfg->mv[1] - MV_MARGEN_CV ? "CV" : "CC");
        snprintf(v, sizeof(v), "%.2f", b->v);
        numero(2, 15, v, "V");
        snprintf(v, sizeof(v), "%.2f A", b->i);
        texto(126, 14, v, 2);
        snprintf(v, sizeof(v), "%.1f°C", b->temp);
        texto(126, 25, v, 2);

        /* nivel estimado por tension, entre corte y fin de carga */
        nivel = limitar((int)(100 * (b->v * 1000.0f - cfg->mv[0]) / (cfg->mv[1] - cfg->mv[0])), 0, 100);
        bateria(2, 37, 50, 9, nivel);
        snprintf(v, sizeof(v), "%d mAh", (int)b->mah);
        texto(126, 37, v, 2);

        texto(2, 53, t, 0);
        boton(72, 50, 56, "Detener", 1);

        if (c->editando) {   /* dialogo "Detener?" */
            u8g2_SetDrawColor(u, 0);
            u8g2_DrawBox(u, 10, 14, 108, 40);
            u8g2_SetDrawColor(u, 1);
            u8g2_DrawRFrame(u, 10, 14, 108, 40, 3);
            texto(64, 19, k ? "Detener carga?" : "Detener descarga?", 1);
            boton(20, 35, 40, "No", !c->sel);
            boton(68, 35, 40, "Si", c->sel);
        }
        break;

    case EST_IDLE:
        if ((c->ahora_ms - c->desde_ms) / 500 % 2) {
            titulo("PAUSA: TEMP ALTA", NULL);
        } else {
            texto(2, 1, "PAUSA: TEMP ALTA", 0);
        }
        termometro(8, 14, b->temp);
        snprintf(v, sizeof(v), "%.1f", b->temp);
        numero(34, 16, v, "°C");
        snprintf(v, sizeof(v), "Reanuda < %d°C", (int)TEMP_OK);
        texto(34, 38, v, 0);
        texto(126, 52, k ? "Carga en pausa" : "Descarga en pausa", 2);
        break;

    default:
        break;
    }

    u8g2_SendBuffer(u);
}
