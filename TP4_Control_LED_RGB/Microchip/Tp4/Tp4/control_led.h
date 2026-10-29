#ifndef CONTROL_LED_H
#define CONTROL_LED_H

#include "utils.h"
// ---------- Periodo de parpadeo (ms) ----------
#define PERIODO_MIN_MS 3000UL // Maxima luz
#define PERIODO_MAX_MS 6000UL // Minima luz

typedef enum {
	FASE_SUBIDA,
	FASE_MESETA,
	FASE_BAJADA,
	FASE_APAGADO
} estado_envolvente_t;


void ControlLed_Init(void);
void ControlLed_SetColorObjetivo(uint8_t r, uint8_t g, uint8_t b);
void ControlLed_ActualizarPeriodo(uint8_t adc_val);
void ControlLed_Actualizar(uint32_t tiempo_ms); // Recalcula envolvente y aplica al PWM

#endif