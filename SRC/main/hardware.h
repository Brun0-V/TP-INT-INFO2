#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>
#include "funciones.h"
#include "u8g2.h"

#define PIN_TEMP_ADC  0
#define PIN_ENC_CLK   1
#define PIN_ENC_DT    3
#define PIN_ENC_SW    4
#define PIN_I2C_SDA   8
#define PIN_I2C_SCL   9
#define PIN_CHG_PWM   5
#define PIN_DCHG_PWM  10
#define PIN_BUZZER    20

void hw_init(void);
uint32_t hw_millis(void);
void hw_log(const char *s);

void hw_beep(uint32_t ms);
evento_t hw_leer(bat_medicion_t *b);
u8g2_t *hw_pantalla(void);

#endif