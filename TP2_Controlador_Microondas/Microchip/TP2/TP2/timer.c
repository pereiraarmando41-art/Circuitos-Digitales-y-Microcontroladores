#include "TIMER.h"
#include <avr/io.h>
#define F_CPU 16000000UL
#include <avr/interrupt.h>

// Variables volátiles (compartidas con interrupción)
volatile uint16_t contador_segundos = 0;
volatile uint16_t contador_milisegundos = 0;
volatile uint8_t flag_un_segundo = 0;

// Variables para el período de la MEF (10ms)
volatile uint8_t Flag_MEF = 0;      // Flag que main va a leer
volatile uint8_t contador_MEF = 0;  // Contador interno de la ISR

#define TIMER0_COMPARE_VALUE 249

// Interrupción por comparación del Timer0 (cada 1ms)
ISR(TIMER0_COMPA_vect) {
	// ========================================
	// 1. Contar milisegundos (para main)
	// ========================================
	if (++contador_milisegundos >= 1000) {
		contador_milisegundos = 0;
		contador_segundos++;
		flag_un_segundo = 1;
	}
	
	// ========================================
	// 2. Período de la MEF (cada 10ms)
	// ========================================
	if (++contador_MEF >= 10) {  // 10 × 1ms = 10ms
		contador_MEF = 0;
		Flag_MEF = 1;            // Avisar a main que ejecute MEF
	}
}

void TIMER_inicializar(void) {
	// Configurar PB5 como salida (Magnetrón)
	DDRB |= (1 << DDB5);
	TIMER_apagar_magnetron();
	
	// Configurar Timer0 en modo CTC
	TCCR0A = 0x02;  // Modo CTC
	
	// Configurar Prescaler a 64
	TCCR0B = 0x00;  // Limpiar primero
	TCCR0B |= (1 << CS01) | (1 << CS00);
	
	// Configurar valor de comparación para 1 ms
	OCR0A = TIMER0_COMPARE_VALUE;
	
	// Habilitar interrupción por comparación A
	TIMSK0 |= (1 << OCIE0A);
	
	// NOTA: No habilitar sei() aquí, se hará en main
}

uint16_t TIMER_get_milisegundos(void) {
	uint16_t ms;
	cli();
	ms = contador_milisegundos;
	sei();
	return ms;
}

uint16_t TIMER_get_segundos(void) {
	uint16_t seg;
	cli();
	seg = contador_segundos;
	sei();
	return seg;
}

void TIMER_reset(void) {
	cli();
	contador_milisegundos = 0;
	contador_segundos = 0;
	flag_un_segundo = 0;
	contador_MEF = 0;     // También reiniciar el contador de la MEF
	Flag_MEF = 0;
	sei();
}

uint8_t TIMER_ha_pasado_un_segundo(void) {
	uint8_t resultado = 0;
	cli();
	if (flag_un_segundo) {
		flag_un_segundo = 0;
		resultado = 1;
	}
	sei();
	return resultado;
}

uint8_t TIMER_debe_ejecutar_MEF(void) {
	uint8_t resultado = 0;
	cli();
	if (Flag_MEF) {
		Flag_MEF = 0;
		resultado = 1;
	}
	sei();
	return resultado;
}

void TIMER_encender_magnetron(void) {
	PORTB |= (1 << PORTB5);
}

void TIMER_apagar_magnetron(void) {
	PORTB &= ~(1 << PORTB5);
}