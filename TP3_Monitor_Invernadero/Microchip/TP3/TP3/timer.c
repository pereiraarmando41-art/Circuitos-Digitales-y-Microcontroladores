#include "timer.h"

static volatile uint8_t flagClk = 0;
static uint16_t contador_ticks = 0;
static uint16_t ticks_por_periodo = 20;  // 2 segundos por defecto

/************************************************************************
Utiliza el timer 1 de 16bits para lograr interrupciones cada 100ms
Debe contar hasta 25.000 con un prescaler de 64 para este fin,
logrando interrupciones precisas de 100ms;
************************************************************************/

void TIMER_Init(void)
{	
	TCCR1A = 0;											//COM1A=0
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);	//modo CTC con prescaler 64
	OCR1A = 0x61A8;										//TOP=25000
}

void TIMER_Enable(void)
{
	TCNT1 = 0;					// Inicializa el contador
	TIMSK1 |= (1 << OCIE1A);	// Habilitar interrupción por comparación A
}

void TIMER_Disable(void)
{
	TIMSK1 &= ~(1 << OCIE1A);	// deshabilita interrupción
}

/*
 * Cambia el intervalo de reporte.
 * segundos: valor entre 2 y 60.
 * Como la ISR dispara cada 100 ms, limite = segundos * 10.
 */
void TIMER_SetPeriodo(uint8_t segundos)
{
	ticks_por_periodo = segundos * 10;  // Convertir a ticks de 100ms
	contador_ticks = 0;
}

uint8_t TIMER_GetClkFlag(void)
{
	return flagClk;
}

void TIMER_ClearClkFlag(void)
{
	flagClk = 0;
}

// Rutina de servicio para el timer que se activa "On Compare Match", definido en el timer a 100ms
ISR(TIMER1_COMPA_vect)
{
	contador_ticks++;
	if (contador_ticks >= ticks_por_periodo)
	{
		contador_ticks = 0;
		flagClk = 1;
	}
}