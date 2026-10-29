#ifndef KEYPAD_H
#define KEYPAD_H

#include <avr/io.h>
#define F_CPU 16000000UL		// 16 MHz
#include <util/delay.h>
#include <stdint.h>

// Pines de FILAS (salidas)
#define FILA0_PIN   4   // PORTB4
#define FILA1_PIN   3   // PORTB3
#define FILA2_PIN   0   // PORTB0
#define FILA3_PIN   7   // PORTD7

// Pines de COLUMNAS (entradas)
#define COL0_PIN    3   // PORTD3
#define COL1_PIN    5   // PORTD5
#define COL2_PIN    4   // PORTD4
#define COL3_PIN    2   // PORTD2

// Mascaras
#define MASK_FILAS_PORTB  ((1<<FILA0_PIN) | (1<<FILA1_PIN) | (1<<FILA2_PIN))
#define MASK_FILAS_PORTD  (1<<FILA3_PIN)
#define MASK_COLUMNAS     ((1<<COL0_PIN) | (1<<COL1_PIN) | (1<<COL2_PIN) | (1<<COL3_PIN))

void KEYPAD_Init(void);         
uint8_t KepadUpdate(void);
uint8_t KEYPAD_Scan (uint8_t *pkey);

#endif