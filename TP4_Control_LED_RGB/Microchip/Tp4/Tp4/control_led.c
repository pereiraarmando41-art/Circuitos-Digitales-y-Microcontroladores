#include "control_led.h"
#include "pwm.h"

static volatile uint8_t  color_r = 0, color_g = 0, color_b = 0; // Color objetivo (usuario)
static uint16_t periodo_T_ms = 4500;                    // Periodo de parpadeo actual
static estado_envolvente_t estado_actual = FASE_SUBIDA;
static uint32_t inicio_fase_ms = 0;
static uint16_t apagado_ms = 0;

void ControlLed_Init(void) {
	PWM_Init();
}

void ControlLed_SetColorObjetivo(uint8_t r, uint8_t g, uint8_t b) {
	color_r = r;
	color_g = g;
	color_b = b;
}

void ControlLed_ActualizarPeriodo(uint8_t adc_val) {
	// Mas luz -> adc_val alto -> periodo menor (parpadeo mas rapido)
	periodo_T_ms = 6000UL - (((uint8_t)adc_val * 3000UL) / 255UL);

}

static uint8_t Calcular_Envolvente(uint32_t tiempo_ms) {
	uint32_t tiempo_en_fase = tiempo_ms - inicio_fase_ms;
	uint8_t salida = 0;
	
	switch(estado_actual) {
		case FASE_SUBIDA:
		if (tiempo_en_fase >= 1000) {
			estado_actual = FASE_MESETA;
			inicio_fase_ms = tiempo_ms;
			salida = 255;
			} else {
			salida = (uint8_t)((tiempo_en_fase * 255UL) / 1000);
		}
		break;
		
		case FASE_MESETA:
		if (tiempo_en_fase >= 1000) {
			estado_actual = FASE_BAJADA;
			inicio_fase_ms = tiempo_ms;
			salida = 255;
			} else {
			salida = 255;
		}
		break;
		
		case FASE_BAJADA:
		if (tiempo_en_fase >= 1000) {
			estado_actual = FASE_APAGADO;
			inicio_fase_ms = tiempo_ms;
			// Calcular tiempo de apagado (puede ser 0 si periodo=3000ms)
			apagado_ms = (periodo_T_ms > 3000) ? (periodo_T_ms - 3000) : 0;
			salida = 0;
			} else {
			salida = (uint8_t)(255 - ((tiempo_en_fase * 255UL) / 1000));
		}
		break;
		
		case FASE_APAGADO:
		if (apagado_ms == 0) {
			// Si no hay apagado, reiniciar inmediatamente
			estado_actual = FASE_SUBIDA;
			inicio_fase_ms = tiempo_ms;
			salida = 0;
		}
		else if (tiempo_en_fase >= apagado_ms) {
			// Fin del apagado, reiniciar ciclo
			estado_actual = FASE_SUBIDA;
			inicio_fase_ms = tiempo_ms;
			salida = 0;
			} else {
			// Durante el apagado
			salida = 0;
		}
		break;
	}
	
	return salida;
}


void ControlLed_Actualizar(uint32_t tiempo_ms) {
	uint8_t envolvente = Calcular_Envolvente(tiempo_ms);

	PWM_Set_Rojo(((uint16_t)color_r * envolvente) / 255);
	PWM_Set_Verde(((uint16_t)color_g * envolvente) / 255);
	PWM_Set_Azul(((uint16_t)color_b * envolvente) / 255);
}