#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

void TIMER_inicializar(void);
uint16_t TIMER_get_milisegundos(void);
uint16_t TIMER_get_segundos(void);
void TIMER_reset(void);
uint8_t TIMER_ha_pasado_un_segundo(void);
void TIMER_encender_magnetron(void);
void TIMER_apagar_magnetron(void);

// Nueva función para que main sepa si debe ejecutar la MEF
uint8_t TIMER_debe_ejecutar_MEF(void);

#endif