#include "timer2.h"

static volatile uint32_t contador_milisegundos = 0;
static volatile uint16_t contador_100ms = 0;
static volatile uint8_t  muestra_lista  = 0;

void TIMER2_Init(void) {
	TCCR2A = (1 << WGM21); // CTC
	TCCR2B = (1 << CS22);  // prescaler = 64
	OCR2A  = 249;          // 16MHz/64/1000Hz - 1 = 1ms exacto
	TIMSK2 = (1 << OCIE2A);
}

ISR(TIMER2_COMPA_vect) {
	contador_milisegundos++;
	
	// Cada 100ms avisa al loop principal que es hora de medir el LDR
	if (++contador_100ms >= 100) {
		contador_100ms = 0;
		muestra_lista = 1;
	}
}


uint32_t TIMER2_Get_Milisegundos(void) {
	uint32_t ms;
	cli();
	ms = contador_milisegundos;
	sei();
	return ms;
}

uint8_t TIMER2_MuestraLista(void) {
	return muestra_lista;
}

void TIMER2_MuestraProcesada(void) {
	muestra_lista = 0;
}